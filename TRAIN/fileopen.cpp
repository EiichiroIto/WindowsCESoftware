#define	STRICT
#include	<windows.h>
#include	<windowsx.h>
#include	<commctrl.h>
#include	<commdlg.h>
#include	<tchar.h>
#include	"basewin.h"
#include	"resource.h"
#include	"fileopen.h"

#define		GawaroBaseKey	TEXT("Software\\Gawaro")
#define		MAX_RECENT		10

extern	HFONT	CreateListboxFont() ;
extern	void	slash2backslash( LPTSTR str ) ;
extern	void	catdir( LPTSTR body, LPCTSTR adddir, LPTSTR pPrevDir ) ;

/*-------------------------------------------------------------------------*
 * FileOpen ダイアログ
 *-------------------------------------------------------------------------*/
class FileOpenDialog : public Window {
	BOOL			m_fSave ;
	OPENFILENAME	*m_pOfn ;
	TCHAR		m_sWildCard[ MAX_PATH ] ;
	TCHAR		m_sFile[ MAX_PATH ] ;
	TCHAR		m_sDir[ MAX_PATH ] ;
	HFONT		m_hFont ;
	LPCTSTR		m_pAppName ;
	BOOL		m_fRecent ;

public:
	FileOpenDialog( OPENFILENAME *pOfn, BOOL fSave, LPCTSTR pAppName ) ;
	~FileOpenDialog() ;

	LPCTSTR	GetText() const { return m_sFile ; }
	LPCTSTR	GetDir() const { return m_sDir ; }

	void	WMCreate( HWND hDlg ) ;
	LRESULT	WMCommand( HWND hDlg, UINT msg, WPARAM wp, LPARAM lp ) ;

	BOOL	EnumDir( HWND hWnd, LPDWORD pCount, LPDWORD pMem ) ;
	BOOL	ListDir( HWND hWnd, LPCTSTR pPrevDir ) ;
	void	InitListDir( HWND hWnd ) ;
	BOOL	ChangeDir( HWND hDlg, LPBOOL pDone ) ;
	BOOL	EnumRecent( HWND hWnd ) ;
	BOOL	SelectRecent( HWND hDlg ) ;

	static LPCTSTR	c_sDir ;
	static LPCTSTR	c_sPatDir ;
	static LPCTSTR	c_sPatFile ;
} ;

LPCTSTR	FileOpenDialog::c_sDir = TEXT("   <DIR>") ;
LPCTSTR	FileOpenDialog::c_sPatDir = TEXT(" %s\t%s") ;
LPCTSTR	FileOpenDialog::c_sPatFile = TEXT("%s\t%8d") ;

FileOpenDialog::FileOpenDialog( OPENFILENAME *pOfn, BOOL fSave, LPCTSTR pAppName )
{
	LPCTSTR			ptr ;

	m_fSave = fSave ;
	m_pOfn = pOfn ;
	/* フォントを取得する */
	m_hFont = CreateListboxFont() ;
	/* ワイルドカードを作成する */
	_tcscpy( m_sWildCard, TEXT("*.*") ) ;
	ptr = pOfn->lpstrFilter ;
	if ( ptr && *ptr ) {
		ptr += _tcslen( ptr ) + 1 ;
		if ( *ptr ) {
			_tcscpy( m_sWildCard, ptr ) ;
		}
	}
	_tcscpy( m_sFile, pOfn->lpstrFile       ? pOfn->lpstrFile       : TEXT("") ) ;
	_tcscpy( m_sDir,  pOfn->lpstrInitialDir ? pOfn->lpstrInitialDir : TEXT("\\") ) ;
	m_pAppName = pAppName ;
	m_fRecent = pAppName ? TRUE : FALSE ;
}

FileOpenDialog::~FileOpenDialog()
{
	DeleteObject( m_hFont ) ;
}

void
FileOpenDialog::WMCreate( HWND hDlg )
{
	DWORD	w ;
	RECT	rect ;
	int		tabs[ 1 ] ;
	HWND	hWnd = GetDlgItem( hDlg, IDC_LST_FILE ) ;
	LPCTSTR	pszTitle ;

	/* ダイアログタイトルを設定する */
	if ( m_pOfn->lpstrTitle ) {
		pszTitle = m_pOfn->lpstrTitle ;
	} else if ( m_fSave ) {
		pszTitle = TEXT("Save As") ;
	} else {
		pszTitle = TEXT("Open") ;
	}
	SetWindowText( hDlg, pszTitle ) ;
	/* ディレクトリリストのフォントを設定する */
	SendMessage( hWnd, WM_SETFONT, (WPARAM) m_hFont, (LPARAM) TRUE ) ;
	/* TABの位置を設定する */
	GetClientRect( hDlg, &rect ) ;
	w = rect.right - 8 * 10 ;
	tabs[0] = w / ((LOWORD(GetDialogBaseUnits()) + 3) / 4) ;
	SendMessage( hWnd, LB_SETTABSTOPS, 1, (LPARAM) tabs ) ;

	if ( m_fRecent ) {
		/* 最新のファイル一覧を得る */
		if ( EnumRecent( hWnd ) ) {
			/* カレントディレクトリを表示する */
			SetDlgItemText( hDlg, IDC_CAP_DIR, TEXT("Recent Files...") ) ;
			return ;
		}
	}
	/* ディレクトリ内容を初期表示する */
	InitListDir( hWnd ) ;
	/* カレントディレクトリを表示する */
	SetDlgItemText( hDlg, IDC_CAP_DIR, m_sDir ) ;
	/* ディレクトリリストの選択を解除する */
	SendMessage( hWnd, LB_SETCURSEL, (WPARAM) LB_ERR, 0 ) ;
}

LRESULT
FileOpenDialog::WMCommand( HWND hDlg, UINT msg, WPARAM wp, LPARAM lp )
{
	LPTSTR	ptr ;
	BOOL	fDone ;
	HWND	hCtrl ;
	DWORD	index ;
	TCHAR	buf[ MAX_PATH ] ;

	hCtrl = GetDlgItem( hDlg, IDC_LST_FILE ) ;
	switch ( GET_WM_COMMAND_ID( wp, lp ) ) {
	case IDCANCEL:
		EndDialog( hDlg, FALSE ) ;
		return TRUE ;
	case IDC_BTN_DIR:
		if ( m_fRecent ) {
			/* ディレクトリ内容を初期表示する */
			InitListDir( hCtrl ) ;
			/* カレントディレクトリを表示する */
			SetDlgItemText( hDlg, IDC_CAP_DIR, m_sDir ) ;
			/* ディレクトリ一覧にフォーカス移動 */
			SetFocus( hCtrl ) ;
		}
		return TRUE ;
	case IDC_LST_FILE:
		switch ( GET_WM_COMMAND_CMD( wp, lp ) ) {
		case LBN_SELCHANGE:
			if ( m_fRecent ) {
				break ;
			}
			index = SendDlgItemMessage( hDlg, IDC_LST_FILE, LB_GETCURSEL, 0, 0 ) ;
			if ( index == LB_ERR ) {
				break ;
			}
			SendDlgItemMessage( hDlg, IDC_LST_FILE, LB_GETTEXT, (WPARAM) index, (LPARAM) buf ) ;
			ptr = _tcschr( buf, TEXT('\t') ) ;
			if ( ptr ) {
				if ( !_tcsicmp( ptr + 1, c_sDir ) ) {
					*buf = 0 ;
				} else {
					*ptr = 0 ;
				}
			} else {
				*buf = 0 ;
			}
			SetDlgItemText( hDlg, IDC_EDT_FILE, buf ) ;
			return TRUE ;
		case LBN_DBLCLK:
			if ( m_fRecent ) {
				if ( SelectRecent( hDlg ) ) {
					EndDialog( hDlg, TRUE ) ;
				}
			} else {
				if ( ChangeDir( hDlg, &fDone ) ) {
					if ( fDone ) {
						EndDialog( hDlg, TRUE ) ;
					}
					return TRUE ;
				}
			}
			break ;
		}
		break ;
	case IDOK:
		if ( GetFocus() == hCtrl ) {
			SendMessage( hDlg, WM_COMMAND, MAKELONG(IDC_LST_FILE, LBN_DBLCLK), 0 ) ;
			return TRUE ;
		} else if ( m_fRecent ) {
			/* ディレクトリ内容を初期表示する */
			InitListDir( hCtrl ) ;
			/* カレントディレクトリを表示する */
			SetDlgItemText( hDlg, IDC_CAP_DIR, m_sDir ) ;
			/* ディレクトリ一覧にフォーカス移動 */
			SetFocus( hCtrl ) ;
			return TRUE ;
		}
		GetDlgItemText( hDlg, IDC_EDT_FILE, buf, MAX_PATH ) ;
		if ( !*buf ) {
			SetFocus( hCtrl ) ;
			return TRUE ;
		}
		ptr = _tcsrchr( buf, TEXT('.') ) ;
		if ( !ptr ) {
			/* 拡張子がついていない場合 */
			if ( m_pOfn->lpstrDefExt ) {
				/* デフォルトの拡張子をつける */
				_tcscat( buf, TEXT(".") ) ;
				_tcscat( buf, m_pOfn->lpstrDefExt ) ;
			}
		} else if ( !*(ptr + 1) ) {
			/* 最後につけたピリオドは削る */
			*ptr = 0 ;
		}
		_tcscpy( m_sFile, m_sDir ) ;
		catdir( m_sFile, buf, NULL ) ;
		EndDialog( hDlg, TRUE ) ;
		return TRUE ;
	}
	return 0 ;
}

BOOL
FileOpenDialog::EnumDir( HWND hWnd, LPDWORD pCount, LPDWORD pMem )
{
	WIN32_FIND_DATA	find ;
	HANDLE			hFind ;
	BOOL			fAdd = !pCount && !pMem ;
	TCHAR			findPath[ MAX_PATH ], *ptr ;
	DWORD			count = 0, num_mem = 0, attr ;

	attr = GetFileAttributes( m_sDir ) ;
	if ( attr == 0xFFFFFFFF || !(attr & FILE_ATTRIBUTE_DIRECTORY) ) {
		return FALSE ;
	}
	if ( m_sDir && _tcscmp( m_sDir, TEXT("\\") ) ) {
		/* 親ディレクトリへのエントリを作成する */
		wsprintf( findPath, c_sPatDir, TEXT(".."), c_sDir ) ;
		if ( fAdd ) {
			SendMessage( hWnd, LB_ADDSTRING, 0, (LPARAM) findPath ) ;
		} else {
			count ++ ;
			num_mem += _tcslen( findPath ) ;
		}
	}
	/* ディレクトリ検索用のワイルドカードを作成する */
	_tcscpy( findPath, m_sDir ) ;
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
			wsprintf( findPath, c_sPatDir, find.cFileName, c_sDir ) ;
			if ( fAdd ) {
				/* リストボックスに項目を追加する */
				SendMessage( hWnd, LB_ADDSTRING, 0, (LPARAM) findPath ) ;
			} else {
				count ++ ;
				num_mem += _tcslen( findPath ) + 1 ;
			}
		} while ( FindNextFile( hFind, &find ) ) ;
		FindClose( hFind ) ;
	}
	/* ファイル検索用のワイルドカードを作成する */
	_tcscpy( findPath, m_sDir ) ;
//	catdir( findPath, m_sWildCard, NULL ) ;
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
//			if ( !_tcscmp( m_sWildCard, TEXT("*.*") ) ) {
				/* 全ファイル対象の場合、バイナリファイルは除外 */
				ptr = _tcsrchr( find.cFileName, TEXT('.') ) ;
				if ( ptr ) {
					if ( !_tcsicmp( ptr, TEXT(".BMP") ) ) {
						continue ;
					} else if ( !_tcsicmp( ptr, TEXT(".2BP") ) ) {
						continue ;
					} else if ( !_tcsicmp( ptr, TEXT(".WAV") ) ) {
						continue ;
					} else if ( !_tcsicmp( ptr, TEXT(".LNK") ) ) {
						continue ;
					} else if ( !_tcsicmp( ptr, TEXT(".DLL") ) ) {
						continue ;
					} else if ( !_tcsicmp( ptr, TEXT(".TTF") ) ) {
						continue ;
					} else if ( !_tcsicmp( ptr, TEXT(".CPL") ) ) {
						continue ;
					} else if ( !_tcsicmp( ptr, TEXT(".EXE") ) ) {
						continue ;
					} else if ( !_tcsicmp( ptr, TEXT(".DAT") ) ) {
					} else if ( !_tcsicmp( ptr, TEXT(".TRI") ) ) {
					} else {
						continue ;
					}
				}
//			}
			wsprintf( findPath, c_sPatFile, find.cFileName, find.nFileSizeLow ) ;
			if ( fAdd ) {
				/* リストボックスに項目を追加する */
				SendMessage( hWnd, LB_ADDSTRING, 0, (LPARAM) findPath ) ;
			} else {
				count ++ ;
				num_mem += _tcslen( findPath ) + 1 ;
			}
		} while ( FindNextFile( hFind, &find ) ) ;
		FindClose( hFind ) ;
	}
	if ( pCount ) {
		*pCount = count ;
	}
	if ( pMem ) {
		*pMem = num_mem ;
	}
	return TRUE ;
}

BOOL
FileOpenDialog::ListDir( HWND hWnd, LPCTSTR pPrevDir )
{
	DWORD	index = 0 ;
	TCHAR	buf[ MAX_PATH ] ;

	/* リストボックスをクリアする */
	SendMessage( hWnd, LB_RESETCONTENT, 0, 0 ) ;
	/* ディレクトリ内容を列挙する */
	if ( !EnumDir( hWnd, NULL, NULL ) ) {
		return FALSE ;
	}
	if ( pPrevDir ) {
		wsprintf( buf, c_sPatDir, pPrevDir, c_sDir ) ;
		index = SendMessage( hWnd, LB_FINDSTRINGEXACT, (WPARAM) -1, (LPARAM) buf ) ;
		if ( index == LB_ERR ) {
			index = 0 ;
		}
	}
	SendMessage( hWnd, LB_SETCURSEL, (WPARAM) index, 0 ) ;
	return TRUE ;
}

/*
 * ディレクトリ内容の初期表示を行う
 */
void
FileOpenDialog::InitListDir( HWND hWnd )
{
	/* 最新のファイル表示モードを抜ける */
	m_fRecent = FALSE ;
	/* 初期ディレクトリのリストを作成する */
	if ( !ListDir( hWnd, NULL ) ) {
		_tcscpy( m_sDir, TEXT("\\") ) ;
		ListDir( hWnd, NULL ) ;
	}
}

BOOL
FileOpenDialog::ChangeDir( HWND hDlg, LPBOOL pDone )
{
	LPTSTR	ptr ;
	int		index ;
	TCHAR	buf[ MAX_PATH ], sPrevDir[ MAX_PATH ] ;
	HWND	hWnd = GetDlgItem( hDlg, IDC_LST_FILE ) ;

	index = SendMessage( hWnd, LB_GETCURSEL, 0, 0 ) ;
	if ( index == LB_ERR ) {
		return FALSE ;
	}
	SendMessage( hWnd, LB_GETTEXT, (WPARAM) index, (LPARAM) buf ) ;
	ptr = _tcschr( buf, TEXT('\t') ) ;
	if ( ptr ) {
		*ptr++ = 0 ;
		if ( !_tcsicmp( ptr, c_sDir ) ) {
			catdir( m_sDir, &buf[1], sPrevDir ) ;
			if ( !ListDir( hWnd, sPrevDir ) ) {
				_tcscpy( m_sDir, TEXT("\\") ) ;
				ListDir( hWnd, NULL ) ;
			}
			SetDlgItemText( hDlg, IDC_CAP_DIR, m_sDir ) ;
			*pDone = FALSE ;
		} else {
			_tcscpy( m_sFile, m_sDir ) ;
			catdir( m_sFile, buf, NULL ) ;
			*pDone = TRUE ;
		}
	}
	return TRUE ;
}

BOOL
FileOpenDialog::EnumRecent( HWND hWnd )
{
	HKEY	hKey ;
	LONG	lret ;
	LPTSTR	pBuffer ;
	TCHAR	buf[ MAX_PATH ], data[ MAX_PATH ] ;
	DWORD	index, cchName, dwType, cbData, i ;

	pBuffer = (LPTSTR) LocalAlloc( LPTR, sizeof (TCHAR) * MAX_PATH * MAX_RECENT ) ;
	if ( !pBuffer ) {
		return FALSE ;
	}
	/* レジストリから最近使ったファイルの一覧を得る */
	wsprintf( buf, TEXT("%s\\%s\\Recent"), GawaroBaseKey, m_pAppName ) ;
	lret = RegOpenKeyEx( HKEY_CURRENT_USER, buf, 0,
#ifdef	_WIN32_WCE
						0,
#else	/* _WIN32_WCE */
						KEY_ALL_ACCESS,
#endif	/* _WIN32_WCE */
						&hKey ) ;
	if ( lret != ERROR_SUCCESS ) {
		LocalFree( (HLOCAL) pBuffer ) ;
		return FALSE ;
	}
	index = 0 ;
	while ( 1 ) {
		cchName = sizeof buf / sizeof (TCHAR) ;
		cbData  = sizeof data ;
		lret = RegEnumValue( hKey, index ++,
							 buf, &cchName,
							 NULL, &dwType,
							 (LPBYTE) data, &cbData ) ;
		if ( lret != ERROR_SUCCESS ) {
			break ;
		} else if ( dwType != REG_SZ ) {
			continue ;
		}
		i = _ttol( buf ) ;
		if ( i >= 0 && i < MAX_RECENT ) {
			_tcscpy( &pBuffer[ i * MAX_PATH ], data ) ;
		}
	}
	RegCloseKey( hKey ) ;
	/* 最近使ったファイルの一覧をリストボックスに加える */
	SendMessage( hWnd, LB_RESETCONTENT, 0, 0 ) ;
	index = 0 ;
	for ( i = 0 ; i < MAX_RECENT ; i ++ ) {
		LPCTSTR	ptr = &pBuffer[ i * MAX_PATH ] ;
		if ( *ptr ) {
			index ++ ;
			SendMessage( hWnd, LB_INSERTSTRING, (WPARAM) -1, (LPARAM) ptr ) ;
		}
	}

	LocalFree( (HLOCAL) pBuffer ) ;
	return index ? TRUE : FALSE ;
}

BOOL
FileOpenDialog::SelectRecent( HWND hDlg )
{
	int		index ;
	HWND	hWnd = GetDlgItem( hDlg, IDC_LST_FILE ) ;

	index = SendMessage( hWnd, LB_GETCURSEL, 0, 0 ) ;
	if ( index == LB_ERR ) {
		return FALSE ;
	}
	SendMessage( hWnd, LB_GETTEXT, (WPARAM) index, (LPARAM) m_sFile ) ;
	return TRUE ;
}

BOOL
GetOpenFileName2( OPENFILENAME *pOfn, LPCTSTR pAppName )
{
	FileOpenDialog		*pDialog = new FileOpenDialog( pOfn, FALSE, pAppName ) ;

	if ( !DialogBoxParam( pOfn->hInstance, MAKEINTRESOURCE(IDD_DLG_FOPEN), pOfn->hwndOwner, (DLGPROC) DlgProc, (LPARAM) pDialog ) ) {
		delete pDialog ;
		return FALSE ;
	}
	if ( pOfn->lpstrFile ) {
		_tcscpy( pOfn->lpstrFile, pDialog->GetText() ) ;
	}
	if ( pOfn->lpstrInitialDir ) {
		_tcscpy( (LPTSTR) pOfn->lpstrInitialDir, pDialog->GetDir() ) ;
	}
	delete pDialog ;
	return TRUE ;
}

BOOL
GetSaveFileName2( OPENFILENAME *pOfn )
{
	FileOpenDialog		*pDialog = new FileOpenDialog( pOfn, TRUE, NULL ) ;

	if ( !DialogBoxParam( pOfn->hInstance, MAKEINTRESOURCE(IDD_DLG_FOPEN), pOfn->hwndOwner, (DLGPROC) DlgProc, (LPARAM) pDialog ) ) {
		delete pDialog ;
		return FALSE ;
	}
	if ( pOfn->lpstrFile ) {
		_tcscpy( pOfn->lpstrFile, pDialog->GetText() ) ;
	}
	if ( pOfn->lpstrInitialDir ) {
		_tcscpy( (LPTSTR) pOfn->lpstrInitialDir, pDialog->GetDir() ) ;
	}
	delete pDialog ;
	return TRUE ;
}

void
AddRecent( LPCTSTR pAppName, LPCTSTR pPath )
{
	HKEY	hKey ;
	LONG	lret ;
	LPBYTE	lpData ;
	LPTSTR	pBuffer, name ;
	BOOL	fWrite = TRUE ;
	TCHAR	buf[ MAX_PATH ], data[ MAX_PATH ] ;
	DWORD	index, cchName, dwType, cbData, i, ret ;

	pBuffer = (LPTSTR) LocalAlloc( LPTR, sizeof (TCHAR) * MAX_PATH * MAX_RECENT ) ;
	if ( !pBuffer ) {
		return ;
	}
	/* レジストリから最近使ったファイルの一覧を得る */
	wsprintf( buf, TEXT("%s\\%s\\Recent"), GawaroBaseKey, pAppName ) ;
	lret = RegCreateKeyEx( HKEY_CURRENT_USER, buf, 0, TEXT(""), 0,
#ifdef	_WIN32_WCE
						   0,
#else	/* _WIN32_WCE */
						   KEY_ALL_ACCESS,
#endif	/* _WIN32_WCE */
						   NULL, &hKey, &ret ) ;
	if ( lret != ERROR_SUCCESS ) {
		LocalFree( (HLOCAL) pBuffer ) ;
		return ;
	}
	index = 0 ;
	while ( 1 ) {
		cchName = sizeof buf / sizeof (TCHAR) ;
		cbData  = sizeof data ;
		lret = RegEnumValue( hKey, index ++,
							 buf, &cchName,
							 NULL, &dwType,
							 (LPBYTE) data, &cbData ) ;
		if ( lret != ERROR_SUCCESS ) {
			break ;
		} else if ( dwType != REG_SZ ) {
			continue ;
		}
		i = _ttol( buf ) ;
		if ( i >= 0 && i < MAX_RECENT ) {
			if ( _tcsicmp( data, pPath ) ) {
				_tcscpy( &pBuffer[ i * MAX_PATH ], data ) ;
			} else if ( i == 0 ) {
				fWrite = FALSE ;
			}
		}
	}
	if ( fWrite ) {
		/* 最近使ったファイルの一覧をレジストリに書き込む */
		index = 0 ;
		/* 指定されたファイルを書き込む */
		wsprintf( buf, TEXT("%d"), index ++ ) ;
		name = buf ;
		dwType = REG_SZ ;
		lpData = (LPBYTE) pPath ;
		cbData = (_tcslen(pPath) + 1) * sizeof (*pPath) ;
		RegSetValueEx( hKey, name, 0, dwType, lpData, cbData ) ;
		/* 残りのファイルを書き込む */
		for ( i = 0 ; i < MAX_RECENT ; i ++ ) {
			LPCTSTR	ptr = &pBuffer[ i * MAX_PATH ] ;

			if ( !*ptr ) {
				continue ;
			}
			wsprintf( buf, TEXT("%d"), index ++ ) ;
			name = buf ;
			dwType = REG_SZ ;
			lpData = (LPBYTE) ptr ;
			cbData = (_tcslen(ptr) + 1) * sizeof (*ptr) ;
			RegSetValueEx( hKey, name, 0, dwType, lpData, cbData ) ;
		}
	}
	RegCloseKey( hKey ) ;
	LocalFree( (HLOCAL) pBuffer ) ;
}

void
slash2backslash( LPTSTR str )
{
	LPTSTR	ptr ;

	/* '/'を'\'に変換する */
	while ( ptr = _tcschr( str, TEXT('/') ) ) {
		*ptr = TEXT('\\') ;
	}
}

void
catdir( LPTSTR body, LPCTSTR adddir, LPTSTR pPrevDir )
{
	DWORD	len ;
	LPTSTR	ptr ;

	slash2backslash( body ) ;
	if ( pPrevDir ) {
		_tcscpy( pPrevDir, TEXT("..") ) ;
	}
	while ( *adddir ) {
		if ( _tcschr( TEXT("/\\"), *adddir ) ) {
			/* 先頭が"/"か"\"の場合は読み飛ばす */
			adddir ++ ;
			continue ;
		}
		if ( adddir[0] == TEXT('.') && adddir[1] == TEXT('.') ) {
			ptr = _tcsrchr( body, TEXT('\\') ) ;
			if ( ptr && pPrevDir ) {
				_tcscpy( pPrevDir, ptr + 1 ) ;
			}
			if ( ptr && ptr != body ) {
				*ptr = 0 ;
			} else {
				_tcscpy( body, TEXT("\\") ) ;
			}
			adddir += 2 ;
		} else if ( adddir[0] == TEXT('.') ) {
			adddir ++ ;
		} else {
			break ;
		}
	}
	if ( !*adddir ) {
		return ;
	}
	len = _tcslen( body ) ;
	if ( !len || body[ len - 1 ] != TEXT('\\') ) {
		body[ len ++ ] = TEXT('\\') ;
	}
	_tcscpy( &body[ len ], adddir ) ;
	slash2backslash( body ) ;
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
