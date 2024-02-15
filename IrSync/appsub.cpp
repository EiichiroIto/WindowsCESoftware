#define	STRICT
#include	<windows.h>
#include	<windowsx.h>
#include	<commctrl.h>
#include	<tchar.h>
#include	"resource.h"
#include	"tools.h"
#include	"appmain.h"
#include	"appsub.h"
#include	"dialog.h"

const	LPCTSTR	c_szDir = TEXT("   <DIR>") ;
const	LPCTSTR	c_szPatDir = TEXT(" %s\t%s") ;
const	LPCTSTR	c_sPatFile = TEXT("%s\t%8d") ;
BOOL	ListDir( HWND hWnd, LPCTSTR szDir, LPCTSTR pPrevDir ) ;
BOOL	EnumDir( HWND hWnd, LPCTSTR szDir ) ;
HFONT	CreateListboxFont() ;
void	SetCBEnabled( HWND hwndCB, DWORD idCtrl, BOOL fState ) ;
LPTSTR	wcschr_quoted( LPTSTR ptr, TCHAR delim ) ;

/*-------------------------------------------------------------------------*
 * MainWindow
 *-------------------------------------------------------------------------*/
MainWindow::MainWindow( MainApp *pApp )
{
	m_pApp = pApp ;
	m_hIcon = 0 ;
	m_hFont = 0 ;
	m_bDeleteReg = FALSE ;
	*m_szCopyFile = 0 ;
	m_bCopy = FALSE ;
}

MainWindow::~MainWindow()
{
	if ( m_hIcon ) {
		::DeleteObject( m_hIcon ) ;
	}
	if ( m_hFont ) {
		::DeleteObject( m_hFont ) ;
	}
}

void
MainWindow::WMCreate( HWND hWnd )
{
	RECT		rect ;
	int			tabs[ 1 ] ;
	DWORD		height = 2, w, tw, th, y ;
	HINSTANCE	hInst = m_pApp->get_inst() ;
	const int	height_edit = 20 ;
	int			height_list ;

	m_hwnd = hWnd ;
	LONG	lStyle = GetWindowLong( hWnd, GWL_STYLE ) ;
#ifdef	_WIN32_WCE
	lStyle &= ~WS_CAPTION ;
#endif	/* _WIN32_WCE */
	SetWindowLong( hWnd, GWL_STYLE, lStyle ) ;
	/* アイコンを作成しタスクバーに登録する */
	m_hIcon = (HICON) LoadImage( m_pApp->get_inst(), MAKEINTRESOURCE(IDI_APPICON),
								 IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR ) ;
	if ( m_hIcon ) {
		SendMessage( hWnd, WM_SETICON, FALSE, (LPARAM)m_hIcon ) ;
	}
	/* コマンドバーを追加する */
	height += CreateCommandBar() ;
	/* 表示領域を求める */
	GetClientRect( m_hwnd, &rect ) ;
	tw = rect.right ;
	th = rect.bottom ;
	y = height + 1 ;
	height_list = th - height_edit - 6 - y ;
	/* 直接入力領域を作成する */
	m_hwndDirEdit = CreateWindow( TEXT("EDIT"),
						m_pApp->GetSrcDir(),
						WS_VISIBLE|WS_CHILD|WS_BORDER,
						1, y, tw - 2, height_edit,
						hWnd,
						(HMENU) IDC_DIR_EDIT,
						hInst,
						NULL ) ;
	y += height_edit + 2 ;
	/* ディレクトリ一覧のリストボックスを作成する */
	m_hwndDirList = CreateWindow( TEXT("LISTBOX"),
						TEXT(""),
						WS_VISIBLE|WS_CHILD|WS_VSCROLL|
						LBS_SORT|LBS_USETABSTOPS|LBS_NOTIFY|WS_BORDER,
						1, y, tw - 2, height_list,
						hWnd,
						(HMENU) IDC_DIR_LIST,
						hInst,
						NULL ) ;
	/* フォントを取得する */
	m_hFont = CreateListboxFont() ;
	/* ディレクトリリストのフォントを設定する */
	SendMessage( m_hwndDirList, WM_SETFONT, (WPARAM) m_hFont, (LPARAM) TRUE ) ;
	/* TABの位置を設定する */
	GetClientRect( hWnd, &rect ) ;
	w = rect.right - 8 * 10 ;
	tabs[0] = w / ((LOWORD(GetDialogBaseUnits()) + 3) / 4) ;
	SendMessage( m_hwndDirList, LB_SETTABSTOPS, 1, (LPARAM) tabs ) ;
	/* 初期的なディレクトリリストを作成する */
	if ( !ListDir( m_hwndDirList, m_pApp->GetSrcDir(), NULL ) ) {
		m_pApp->SetSrcDir( TEXT("\\") ) ;
		SetWindowText( m_hwndDirEdit, m_pApp->GetSrcDir() ) ;
	}
	/* 初期フォーカスを設定する */
	SetFocus( m_hwndDirList ) ;
}

void
MainWindow::WMDestroy()
{
	PostQuitMessage( 0 ) ;
}

LRESULT
MainWindow::WMCommand( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	WORD		idCtrl = LOWORD(wParam) ;
	WORD		notifyCode = HIWORD(wParam) ;
	int			index ;
	BOOL		bDelete ;
	HWND		hwndCtrl ;
	TCHAR		buf[ MAX_PATH ], sPrevDir[ MAX_PATH ], *ptr ;

	switch ( idCtrl ) {
	case IDM_ABOUT:
		if ( AboutDialogExec( m_pApp->get_inst(), hWnd, &bDelete ) ) {
			if ( bDelete ) {
				m_bDeleteReg = TRUE ;
				SendMessage( hWnd, WM_CLOSE, 0, 0 ) ;
			}
		}
		break ;
	case IDM_EXIT:
		SendMessage( hWnd, WM_CLOSE, 0, 0 ) ;
		break ;
	case IDC_DIR_LIST:
		switch ( notifyCode ) {
		case LBN_DBLCLK:
			index = SendMessage( m_hwndDirList, LB_GETCURSEL, 0, 0 ) ;
			if ( index == LB_ERR ) {
				/* 選択されていない場合 */
				break ;
			}
			SendMessage( m_hwndDirList, LB_GETTEXT, (WPARAM) index, (LPARAM) buf ) ;
			ptr = _tcschr( &buf[1], TEXT('\t') ) ;
			if ( !ptr ) {
				break ;
			}
			*ptr++ = 0 ;
			if ( _tcsicmp( ptr, c_szDir ) ) {
				/* ディレクトリでない場合 */
				_tcscpy( sPrevDir, m_pApp->GetSrcDir() ) ;
				catdir( sPrevDir, buf, NULL ) ;
				wsprintf( MessageBuf, TEXT("\"%s\""), sPrevDir ) ;
				if ( OpenDialogExec( m_pApp->get_inst(), hWnd, MessageBuf ) ) {
					ptr = wcschr_quoted( MessageBuf, TEXT(' ') ) ;
					if ( ptr ) {
						*ptr++ = 0 ;
					}
#ifdef	_WIN32_WCE
					SHELLEXECUTEINFO		sei ;

					memset( &sei, 0, sizeof sei ) ;
					sei.cbSize = sizeof sei ;
					sei.fMask = 0 ;
					sei.hwnd = NULL ;
					sei.lpVerb = TEXT("Open") ;
					sei.lpFile = MessageBuf ;
					sei.lpParameters = ptr ? ptr : NULL ;
					sei.lpDirectory = NULL ;
					sei.nShow = SW_SHOW ;
					if ( !ShellExecuteEx( &sei ) ) {
//						wsprintf( MessageBuf, TEXT("cannot start(%d)\n"),
//								  GetLastError() ) ;
//						Cputs( MessageBuf ) ;
					}
#else	/* _WIN32_WCE */
					MessageBox( hWnd, MessageBuf, TEXT("EXEC"), MB_OK ) ;
#endif	/* _WIN32_WCE */
				}
				break ;
			}
			catdir( m_pApp->GetSrcDir(), &buf[1], sPrevDir ) ;
			if ( !ListDir( m_hwndDirList, m_pApp->GetSrcDir(), sPrevDir ) ) {
				_tcscpy( m_pApp->GetSrcDir(), TEXT("\\") ) ;
				ListDir( hWnd, m_pApp->GetSrcDir(), NULL ) ;
			}
			SetWindowText( m_hwndDirEdit, m_pApp->GetSrcDir() ) ;
			break ;
		}
		break ;
	case IDC_BTN_SEND:
	case IDC_BTN_RECV:
		m_pApp->MakeTransPair( m_pApp->GetSrcDir(), NULL ) ;
		TransferDialogExec( m_pApp->get_inst(), hWnd, idCtrl == IDC_BTN_SEND ? 1 : 2, m_pApp ) ;
		ListDir( m_hwndDirList, m_pApp->GetSrcDir(), NULL ) ;
		break ;
	case IDM_ENTER:
		hwndCtrl = GetFocus() ;
		if ( hwndCtrl == m_hwndDirList ) {
			SendMessage( hWnd, WM_COMMAND, MAKEWPARAM( IDC_DIR_LIST, LBN_DBLCLK ), 0 ) ;
		} else if ( hwndCtrl == m_hwndDirEdit ) {
			GetWindowText( m_hwndDirEdit, buf, MAX_PATH ) ;
			if ( ListDir( m_hwndDirList, buf, NULL ) ) {
				m_pApp->SetSrcDir( buf ) ;
			}
		}
		break ;
	case IDM_TAB:
		hwndCtrl = GetFocus() ;
		if ( hwndCtrl == m_hwndDirEdit ) {
			SetFocus( m_hwndDirList ) ;
		} else if ( hwndCtrl == m_hwndDirList ) {
			SetFocus( m_hwndDirEdit ) ;
		}
		break ;
	case IDM_STAB:
		hwndCtrl = GetFocus() ;
		if ( hwndCtrl == m_hwndDirEdit ) {
			SetFocus( m_hwndDirList ) ;
		} else if ( hwndCtrl == m_hwndDirList ) {
			SetFocus( m_hwndDirEdit ) ;
		}
		break ;
	case IDM_CUT:
		CutCopySelectedFile( FALSE ) ;
		break ;
	case IDM_COPY:
		CutCopySelectedFile( TRUE ) ;
		break ;
	case IDM_PASTE:
		if ( PasteSelectedFile() ) {
			ListDir( m_hwndDirList, m_pApp->GetSrcDir(), NULL ) ;
		}
		break ;
	case IDM_DELETE:
		if ( DeleteSelectedFile() ) {
			ListDir( m_hwndDirList, m_pApp->GetSrcDir(), NULL ) ;
		}
		break ;
	}
	return 0 ;
}

BOOL
MainWindow::WMClose()
{
	if ( m_bDeleteReg ) {
		m_pApp->DeleteReg() ;
	} else {
		m_pApp->SetReg() ;
	}
	return FALSE ;
}

void
MainWindow::WMSetFocus()
{
	SetFocus( m_hwndDirList ) ;
}

BOOL
MainWindow::CutCopySelectedFile( BOOL bCopy )
{
	LPTSTR	ptr ;
	int		index ;
	TCHAR	buf[ MAX_PATH ] ;

	index = SendMessage( m_hwndDirList, LB_GETCURSEL, 0, 0 ) ;
	if ( index == LB_ERR ) {
		/* 何も選択していない場合 */
		MessageBeep( MB_ICONHAND ) ;
		return FALSE ;
	}
	SendMessage( m_hwndDirList, LB_GETTEXT, (WPARAM) index, (LPARAM) buf ) ;
	ptr = _tcschr( buf, TEXT('\t') ) ;
	if ( !ptr ) {
		/* TABセパレータが見つからない場合 */
		MessageBeep( MB_ICONHAND ) ;
		return FALSE ;
	}
	*ptr++ = 0 ;
	if ( _tcsicmp( ptr, c_szDir ) ) {
		/* ファイルの場合 */
		ptr = buf ;
	} else {
		/* ディレクトリの場合 */
		ptr = &buf[1] ;
		if ( !_tcscmp( ptr, TEXT(".") ) || !_tcscmp( ptr, TEXT("..") ) ) {
			/* カレントディレクトリ／親ディレクトリの場合 */
			MessageBeep( MB_ICONHAND ) ;
			return FALSE ;
		}
	}
	SetCBEnabled( m_hwndCB, IDM_PASTE, TRUE ) ;
	_tcscpy( m_szCopyDir, m_pApp->GetSrcDir() ) ;
	_tcscpy( m_szCopyFile, ptr ) ;
	m_bCopy = bCopy ;
	return TRUE ;
}

BOOL
MainWindow::PasteSelectedFile()
{
	DWORD	attr ;
	TCHAR	srcPath[ MAX_PATH ], dstPath[ MAX_PATH ] ;

	if ( !*m_szCopyFile ) {
		/* CUTもCOPYもしていない場合 */
		MessageBeep( MB_ICONHAND ) ;
		return FALSE ;
	}
	_tcscpy( srcPath, m_szCopyDir ) ;
	catdir( srcPath, m_szCopyFile, NULL ) ;
	_tcscpy( dstPath, m_pApp->GetSrcDir() ) ;
	catdir( dstPath, m_szCopyFile, NULL ) ;
	if ( !_tcscmp( srcPath, dstPath ) ) {
		/* 両者が同一の場合 */
		MessageBeep( MB_ICONHAND ) ;
		return FALSE ;
	}
	attr = GetFileAttributes( dstPath ) ;
	if ( attr != 0xFFFFFFFF ) {
		if ( MessageBox( m_hwnd, TEXT("Replace file?"), TEXT("Copy"), MB_ICONQUESTION|MB_YESNO ) != IDYES ) {
			return FALSE ;
		}
	}
	if ( m_bCopy ) {
		if ( !CopyFile( srcPath, dstPath, FALSE ) ) {
			wsprintf( MessageBuf, TEXT("Can't copy.\r\n[Error=%d]"), GetLastError() ) ;
			MessageBox( m_hwnd, MessageBuf, TEXT("Copy"), MB_ICONHAND|MB_OK ) ;
			return FALSE ;
		}
	} else {
		if ( attr != 0xFFFFFFFF ) {
			if ( attr == FILE_ATTRIBUTE_DIRECTORY ) {
				MessageBox( m_hwnd, TEXT("Dest is directory.\r\nCan't paste."), TEXT("Paste"), MB_ICONHAND|MB_OK ) ;
				return FALSE ;
			}
			if ( !DeleteFile( dstPath ) ) {
				wsprintf( MessageBuf, TEXT("Can't delete.\r\n[Error=%d]"), GetLastError() ) ;
				MessageBox( m_hwnd, MessageBuf, TEXT("Paste"), MB_ICONHAND|MB_OK ) ;
				return FALSE ;
			}
		}
		if ( !MoveFile( srcPath, dstPath ) ) {
			wsprintf( MessageBuf, TEXT("Can't rename.\r\n[Error=%d]"), GetLastError() ) ;
			MessageBox( m_hwnd, MessageBuf, TEXT("Paste"), MB_ICONHAND|MB_OK ) ;
			return FALSE ;
		}
	}
	SetCBEnabled( m_hwndCB, IDM_PASTE, FALSE ) ;
	*m_szCopyDir = 0 ;
	*m_szCopyFile = 0 ;
	return TRUE ;
}

BOOL
MainWindow::DeleteSelectedFile()
{
	LPTSTR	ptr ;
	DWORD	attr ;
	int		index ;
	TCHAR	buf[ MAX_PATH ], srcPath[ MAX_PATH ] ;

	index = SendMessage( m_hwndDirList, LB_GETCURSEL, 0, 0 ) ;
	if ( index == LB_ERR ) {
		/* 何も選択していない場合 */
		MessageBeep( MB_ICONHAND ) ;
		return FALSE ;
	}
	SendMessage( m_hwndDirList, LB_GETTEXT, (WPARAM) index, (LPARAM) buf ) ;
	ptr = _tcschr( &buf[1], TEXT('\t') ) ;
	if ( !ptr ) {
		/* TABセパレータが見つからない場合 */
		MessageBeep( MB_ICONHAND ) ;
		return FALSE ;
	}
	*ptr++ = 0 ;
	if ( _tcsicmp( ptr, c_szDir ) ) {
		/* ファイルの場合 */
		ptr = buf ;
	} else {
		/* ディレクトリの場合 */
		ptr = &buf[1] ;
		if ( !_tcscmp( ptr, TEXT(".") ) || !_tcscmp( ptr, TEXT("..") ) ) {
			/* カレントディレクトリ／親ディレクトリの場合 */
			MessageBeep( MB_ICONHAND ) ;
			return FALSE ;
		}
	}
	_tcscpy( srcPath, m_pApp->GetSrcDir() ) ;
	catdir( srcPath, ptr, NULL ) ;
	attr = GetFileAttributes( srcPath ) ;
	if ( attr == 0xFFFFFFFF ) {
		/* ディレクトリが存在しない場合 */
		MessageBeep( MB_ICONHAND ) ;
		return FALSE ;
	}
	if ( MessageBox( m_hwnd, TEXT("Delete file?"), TEXT("Delete"), MB_ICONQUESTION|MB_YESNO ) != IDYES ) {
		return FALSE ;
	}
	if ( attr == FILE_ATTRIBUTE_DIRECTORY ) {
		if ( !RemoveDirectory( srcPath ) ) {
			wsprintf( MessageBuf, TEXT("Can't delete.\r\n[Error=%d]"), GetLastError() ) ;
			MessageBox( m_hwnd, MessageBuf, TEXT("Delete"), MB_ICONHAND|MB_OK ) ;
			return FALSE ;
		}
	} else {
		if ( !DeleteFile( srcPath ) ) {
			wsprintf( MessageBuf, TEXT("Can't delete.\r\n[Error=%d]"), GetLastError() ) ;
			MessageBox( m_hwnd, MessageBuf, TEXT("Delete"), MB_ICONHAND|MB_OK ) ;
			return FALSE ;
		}
	}
	*m_szCopyDir = 0 ;
	*m_szCopyFile = 0 ;
	return TRUE ;
}

BOOL
MainWindow::Create()
{
	HWND	hWnd = CreateWindowEx( 0,
						GetClassName(),
						szTitleName,
						WS_VISIBLE,
#ifdef	_WIN32_WCE
						0, 0, CW_USEDEFAULT, CW_USEDEFAULT,
#else	/* _WIN32_WCE */
						0, 0, 480, 240,
#endif	/* _WIN32_WCE */
						NULL,
						NULL,
						m_pApp->get_inst(),
						this ) ;
	if ( hWnd == 0 ) {
		return FALSE ;
	}
    ShowWindow( hWnd, SW_SHOW ) ;
    UpdateWindow( hWnd ) ;
    return TRUE ;
}

DWORD
MainWindow::CreateCommandBar()
{
	HINSTANCE	hInst = m_pApp->get_inst() ;

#ifdef	_WIN32_WCE
	static TBBUTTON	tbButton[] = {
		{         0,              0, TBSTATE_ENABLED, TBSTYLE_SEP,    0, 0, 0, -1},
		{        19,   IDC_BTN_RECV, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, -1},
		{        18,   IDC_BTN_SEND, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, -1},
		{         0,              0, TBSTATE_ENABLED, TBSTYLE_SEP,    0, 0, 0, -1},
		{   STD_CUT,        IDM_CUT, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0,  0},
		{  STD_COPY,       IDM_COPY, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, -1},
		{ STD_PASTE,      IDM_PASTE, /*TBSTATE_ENABLED*/0, TBSTYLE_BUTTON, 0, 0, 0, -1},
		{         0,              0, TBSTATE_ENABLED, TBSTYLE_SEP,    0, 0, 0, -1},
		{STD_DELETE,     IDM_DELETE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, -1},
		{         0,              0, TBSTATE_ENABLED, TBSTYLE_SEP,    0, 0, 0, -1},
	} ;

	m_hwndCB = CommandBar_Create( hInst, m_hwnd, 0 ) ;
	CommandBar_InsertMenubar( m_hwndCB, hInst, IDR_MAIN_MENU, 0 ) ;
	CommandBar_AddBitmap( m_hwndCB, HINST_COMMCTRL, IDB_STD_SMALL_COLOR, 15, 0, 0 ) ;
    CommandBar_AddBitmap( m_hwndCB, hInst, IDR_TOOLBAR1, 8, 0, 0 ) ;
    CommandBar_AddButtons( m_hwndCB, sizeof(tbButton) / sizeof(*tbButton), tbButton ) ;
	CommandBar_AddAdornments( m_hwndCB, 0, 0 ) ;
	return CommandBar_Height( m_hwndCB ) ;
#else	/* _WIN32_WCE */
	return 0 ;
#endif	/* _WIN32_WCE */
}

BOOL
MainWindow::RegisterClass( HINSTANCE hInst )
{
	WNDCLASS	wc ;

	wc.style         = NULL ;
	wc.lpfnWndProc   = (WNDPROC) WndProc ;
	wc.cbClsExtra    = 0 ;
	wc.cbWndExtra    = 4 ;
	wc.hInstance     = hInst ;
	wc.hIcon         = NULL ;
	wc.hCursor       = NULL ;
	wc.hbrBackground = (HBRUSH) GetStockObject( WHITE_BRUSH ) ;
#ifdef	_WIN32_WCE
	wc.lpszMenuName  = 0 ;
#else	/* _WIN32_WCE */
	wc.lpszMenuName  = MAKEINTRESOURCE(IDR_MAIN_MENU) ;
#endif	/* _WIN32_WCE */
	wc.lpszClassName = 	GetClassName() ;
	return ::RegisterClass( &wc ) ;
}

BOOL
EnumDir( HWND hWnd, LPCTSTR szDir )
{
	DWORD			attr ;
	WIN32_FIND_DATA	find ;
	HANDLE			hFind ;
	TCHAR			findPath[ MAX_PATH ] ;

	attr = GetFileAttributes( szDir ) ;
	if ( attr == 0xFFFFFFFF || !(attr & FILE_ATTRIBUTE_DIRECTORY) ) {
		return FALSE ;
	}
	if ( szDir && _tcscmp( szDir, TEXT("\\") ) ) {
		/* 親ディレクトリへのエントリを作成する */
		wsprintf( findPath, c_szPatDir, TEXT(".."), c_szDir ) ;
		SendMessage( hWnd, LB_ADDSTRING, 0, (LPARAM) findPath ) ;
	}
	/* ディレクトリ検索用のワイルドカードを作成する */
	_tcscpy( findPath, szDir ) ;
	catdir( findPath, TEXT("*.*"), NULL ) ;
	/* ディレクトリを検索する */
	hFind = FindFirstFile( findPath, &find ) ;
	if ( hFind != INVALID_HANDLE_VALUE ) {
		do {
			if ( !_tcscmp( find.cFileName, TEXT(".") ) ) {
				continue ;
			} else if ( !_tcscmp( find.cFileName, TEXT("..") ) ) {
				continue ;
			} else if ( !(find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) {
				continue ;
			}
			/* リストボックスに項目を追加する */
			wsprintf( findPath, c_szPatDir, find.cFileName, c_szDir ) ;
			SendMessage( hWnd, LB_ADDSTRING, 0, (LPARAM) findPath ) ;
		} while ( FindNextFile( hFind, &find ) ) ;
		FindClose( hFind ) ;
	}
	/* ファイル検索用のワイルドカードを作成する */
	_tcscpy( findPath, szDir ) ;
	catdir( findPath, TEXT("*.*"), NULL ) ;
	/* ファイルを検索する */
	hFind = FindFirstFile( findPath, &find ) ;
	if ( hFind != INVALID_HANDLE_VALUE ) {
		do {
			if ( find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
				continue ;
#ifdef	_WIN32_WCE
			} else if ( find.dwFileAttributes & FILE_ATTRIBUTE_INROM ) {
				continue ;
			} else if ( find.dwFileAttributes & FILE_ATTRIBUTE_ROMSTATICREF ) {
				continue ;
			} else if ( find.dwFileAttributes & FILE_ATTRIBUTE_ROMMODULE ) {
				continue ;
#endif	/* _WIN32_WCE */
			}
			/* リストボックスに項目を追加する */
			wsprintf( findPath, c_sPatFile, find.cFileName, find.nFileSizeLow ) ;
			SendMessage( hWnd, LB_ADDSTRING, 0, (LPARAM) findPath ) ;
		} while ( FindNextFile( hFind, &find ) ) ;
		FindClose( hFind ) ;
	}
	return TRUE ;
}

BOOL
ListDir( HWND hWnd, LPCTSTR szDir, LPCTSTR pPrevDir )
{
	DWORD	index = 0 ;
	TCHAR	buf[ MAX_PATH ] ;

	/* リストボックスをクリアする */
	SendMessage( hWnd, LB_RESETCONTENT, 0, 0 ) ;
	/* ディレクトリ内容を列挙する */
	if ( !EnumDir( hWnd, szDir ) ) {
		return FALSE ;
	}
	if ( pPrevDir ) {
		wsprintf( buf, c_szPatDir, pPrevDir, c_szDir ) ;
		index = SendMessage( hWnd, LB_FINDSTRINGEXACT, (WPARAM) -1, (LPARAM) buf ) ;
		if ( index == LB_ERR ) {
			index = 0 ;
		}
	}
	SendMessage( hWnd, LB_SETCURSEL, (WPARAM) index, 0 ) ;
	return TRUE ;
}

/*
 * リストボックス用の固定ピッチフォントを作成する
 */
HFONT
CreateListboxFont()
{
	LOGFONT         lf ;

	/* フォントを取得する */
	memset( &lf, 0, sizeof lf ) ;
	lf.lfPitchAndFamily = FIXED_PITCH | FF_MODERN ;
	lf.lfCharSet = 128 ;
	lf.lfHeight = 13 ;
	return CreateFontIndirect( &lf ) ;
}

/*
 * コマンドバーボタンの状態を設定する
 */
void
SetCBEnabled( HWND hwndCB, DWORD idCtrl, BOOL fState )
{
#ifdef	_WIN32_WCE
	DWORD	oldState ;

	oldState = SendMessage( hwndCB, TB_GETSTATE, (WPARAM) idCtrl, (LPARAM) 0 ) ;
	if ( oldState == -1 ) {
		return ;
	}
	if ( !(oldState & TBSTATE_ENABLED) != !fState ) {
		oldState &= ~TBSTATE_ENABLED ;
		oldState |= fState ? TBSTATE_ENABLED : 0 ;
		SendMessage( hwndCB, TB_SETSTATE, (WPARAM) idCtrl, MAKELONG(oldState,0) ) ;
	}
#endif	/* _WIN32_WCE */
}

LPTSTR
wcschr_quoted( LPTSTR ptr, TCHAR delim )
{
	TCHAR	c ;

	while ( c = *ptr ) {
		if ( c == TEXT('"') ) {
			while ( c = *++ptr ) {
				if ( c == TEXT('"') ) {
					break ;
				}
			}
		} else if ( c == delim ) {
			return ptr ;
		} else {
			ptr ++ ;
		}
	}
	return NULL ;
}
