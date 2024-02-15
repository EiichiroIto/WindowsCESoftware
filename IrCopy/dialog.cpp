#define	STRICT
#include	<windows.h>
#include	<windowsx.h>
#include	<commctrl.h>
#include	<commdlg.h>
#include	<tchar.h>
#include	"basewin.h"
#include	"resource.h"
#include	"dialog.h"
#include	"appmain.h"

extern	DWORD	AppVersion ;
extern	DWORD	DllVersion ;

extern	WORD	get_now() ;

BOOL	GetOpenFileName2( OPENFILENAME *pOfn ) ;

/*-------------------------------------------------------------------------*
 * ファイルオープンダイアログ
 *-------------------------------------------------------------------------*/
static	OPENFILENAME	ofn ;
static	TCHAR			buf1[ MAX_PATH + 1 ] ;

BOOL
FileOpenDlg( HINSTANCE hInst, HWND hWnd, LPTSTR buf, DWORD size )
{
	memset( &ofn, 0, sizeof ofn ) ;
	memcpy( buf1, TEXT("*.*\0"), sizeof TEXT("*.*\0") ) ;
	ofn.lStructSize = sizeof ofn ;
	ofn.hwndOwner = hWnd ;
	ofn.hInstance = hInst ;
	ofn.lpstrFilter = TEXT("All Files (*.*)\0*.*\0\0") ;
	ofn.lpstrCustomFilter = NULL ;
	ofn.nMaxCustFilter = 0 ;
	ofn.nFilterIndex = 1 ;
	ofn.lpstrFile = buf1 ;
	ofn.nMaxFile = MAX_PATH + 1 ;
	ofn.lpstrFileTitle = NULL ;
	ofn.nMaxFileTitle = 0 ;
	ofn.lpstrInitialDir = NULL ;
	ofn.lpstrTitle = NULL ;
	ofn.Flags = 0 ;
	ofn.nFileOffset = 0 ;
	ofn.nFileExtension = 0 ;
	ofn.lpstrDefExt = TEXT("EXE") ;
	ofn.lCustData = 0 ;
	ofn.lpfnHook = NULL ;
	ofn.lpTemplateName = NULL ;
	if ( !GetOpenFileName2( &ofn ) ) {
		return FALSE ;
	}
	_tcscpy( buf, buf1 ) ;
	return TRUE ;
}

BOOL
OpenListDlg( HINSTANCE hInst, HWND hWnd, LPTSTR buf, DWORD size )
{
	memset( &ofn, 0, sizeof ofn ) ;
	memcpy( buf1, TEXT("*.irc\0"), sizeof TEXT("*.irc\0") ) ;
	ofn.lStructSize = sizeof ofn ;
	ofn.hwndOwner = hWnd ;
	ofn.hInstance = hInst ;
	ofn.lpstrFilter = TEXT("List Files (*.irc)\0*.irc\0\0") ;
	ofn.lpstrCustomFilter = NULL ;
	ofn.nMaxCustFilter = 0 ;
	ofn.nFilterIndex = 1 ;
	ofn.lpstrFile = buf1 ;
	ofn.nMaxFile = MAX_PATH + 1 ;
	ofn.lpstrFileTitle = NULL ;
	ofn.nMaxFileTitle = 0 ;
	ofn.lpstrInitialDir = NULL ;
	ofn.lpstrTitle = NULL ;
	ofn.Flags = 0 ;
	ofn.nFileOffset = 0 ;
	ofn.nFileExtension = 0 ;
	ofn.lpstrDefExt = TEXT("IRC") ;
	ofn.lCustData = 0 ;
	ofn.lpfnHook = NULL ;
	ofn.lpTemplateName = NULL ;
	if ( !GetOpenFileName2( &ofn ) ) {
		return FALSE ;
	}
	_tcscpy( buf, buf1 ) ;
	return TRUE ;
}

BOOL
SaveListDlg( HINSTANCE hInst, HWND hWnd, LPTSTR buf, DWORD size )
{
	memset( &ofn, 0, sizeof ofn ) ;
	*buf1 = 0 ;
	ofn.lStructSize = sizeof ofn ;
	ofn.hwndOwner = hWnd ;
	ofn.hInstance = hInst ;
	ofn.lpstrFilter = TEXT("List Files (*.irc)\0*.irc\0\0") ;
	ofn.lpstrCustomFilter = NULL ;
	ofn.nMaxCustFilter = 0 ;
	ofn.nFilterIndex = 1 ;
	ofn.lpstrFile = buf1 ;
	ofn.nMaxFile = MAX_PATH ;
	ofn.lpstrFileTitle = NULL ;
	ofn.nMaxFileTitle = 0 ;
	ofn.lpstrInitialDir = NULL ;
	ofn.lpstrTitle = NULL ;
	ofn.Flags = 0 ;
	ofn.nFileOffset = 0 ;
	ofn.nFileExtension = 0 ;
	ofn.lpstrDefExt = TEXT("IRC") ;
	ofn.lCustData = 0 ;
	ofn.lpfnHook = NULL ;
	ofn.lpTemplateName = NULL ;
	if ( !GetSaveFileName( &ofn ) ) {
		return FALSE ;
	}
	_tcscpy( buf, buf1 ) ;
	return TRUE ;
}

/*-------------------------------------------------------------------------*
 * Aboutダイアログ
 *-------------------------------------------------------------------------*/
class AboutDialog : public Window {
	BOOL	m_fDelete ;

public:
	AboutDialog() { m_fDelete = FALSE ; }
	~AboutDialog() {}

	void	WMCreate( HWND hDlg ) ;
	LRESULT	WMCommand( HWND hDlg, UINT msg, WPARAM wp, LPARAM lp ) ;

	BOOL	get_delete() const { return m_fDelete ; }
} ;

void
AboutDialog::WMCreate( HWND hDlg )
{
	TCHAR	buf[ 128 ] ;

	wsprintf( buf, TEXT("IrCopy for Windows%c CE Ver %d.%02d"),
			  0xAE, AppVersion / 100, AppVersion % 100 ) ;
	SetDlgItemText( hDlg, IDC_VERSION, buf ) ;
//	wsprintf( buf, TEXT("KCTRL Ver %d.%02d"),
//			  DllVersion / 100, DllVersion % 100 ) ;
//	SetDlgItemText( hDlg, IDC_DLLVERSION, buf ) ;
	wsprintf( buf, TEXT("Copyright %c 1997\r\n%s\r\n%s"),
			  0xA9,
			  TEXT("Eiichiroh Itoh"),
			  TEXT("All Rights Reserved") ) ;
	SetDlgItemText( hDlg, IDC_COPYRIGHT, buf ) ;
}

LRESULT
AboutDialog::WMCommand( HWND hDlg, UINT msg, WPARAM wp, LPARAM lp )
{
	if ( wp == IDCANCEL || wp == IDOK ) {
		EndDialog( hDlg, TRUE ) ;
	} else if ( wp == IDC_UNINSTALL ) {
		if ( MessageBox( hDlg, TEXT("Delete registry information for IrCopy?"), TEXT("UNINSTALL"), MB_YESNO ) == IDYES ) {
			m_fDelete = TRUE ;
			EndDialog( hDlg, TRUE ) ;
		}
	}
	return 0 ;
}

BOOL
AboutDialogExec( HINSTANCE hInst, HWND hWnd, LPBOOL fDelete )
{
	AboutDialog		*pDialog = new AboutDialog() ;

	*fDelete = FALSE ;
	if ( !DialogBoxParam( hInst, MAKEINTRESOURCE(IDD_DLG_ABOUT), hWnd, (DLGPROC) DlgProc, (LPARAM) pDialog ) ) {
		delete pDialog ;
		return FALSE ;
	}
	*fDelete = pDialog->get_delete() ;
	delete pDialog ;
	return TRUE ;
}

/*-------------------------------------------------------------------------*
 * Add ダイアログ
 *-------------------------------------------------------------------------*/
class AddDialog : public Window {
	HINSTANCE	m_hInst ;
	BOOL		m_fEdit ;
	TCHAR		m_sSrcPath[ MAX_PATH ] ;
	TCHAR		m_sDstPath[ MAX_PATH ] ;

public:
	AddDialog( HINSTANCE hInst, BOOL fEdit, TransInfo *pInfo ) {
		m_hInst = hInst ;
		m_fEdit = fEdit ;
		GetFromInfo( pInfo ) ;
	}
	~AddDialog() {}

	void	GetFromInfo( const TransInfo *pInfo ) ;
	void	PutToInfo( TransInfo *pInfo ) const ;

	void	WMCreate( HWND hDlg ) ;
	LRESULT	WMCommand( HWND hDlg, UINT msg, WPARAM wp, LPARAM lp ) ;
} ;

void
AddDialog::GetFromInfo( const TransInfo *pInfo )
{
	_tcscpy( m_sSrcPath, pInfo->srcPath ) ;
	_tcscpy( m_sDstPath, pInfo->dstPath ) ;
}

void
AddDialog::PutToInfo( TransInfo *pInfo ) const
{
	_tcscpy( pInfo->srcPath, m_sSrcPath ) ;
	_tcscpy( pInfo->dstPath, m_sDstPath ) ;
}

void
AddDialog::WMCreate( HWND hDlg )
{
	if ( m_fEdit ) {
		SetWindowText( hDlg, TEXT("Transfer Property") ) ;
	}
	SetDlgItemText( hDlg, IDC_EDT_SRC, m_sSrcPath ) ;
	SetDlgItemText( hDlg, IDC_EDT_DST, m_sDstPath ) ;
	SendDlgItemMessage( hDlg, IDC_EDT_DST, EM_SETMODIFY, *m_sDstPath ? TRUE : FALSE, 0 ) ;
}

LRESULT
AddDialog::WMCommand( HWND hDlg, UINT msg, WPARAM wp, LPARAM lp )
{
	DWORD	attr ;
	TCHAR	buf[ MAX_PATH ] ;

	switch ( GET_WM_COMMAND_ID( wp, lp ) ) {
	case IDCANCEL:
		EndDialog( hDlg, FALSE ) ;
		break ;
	case IDOK:
		*buf = 0 ;
		GetDlgItemText( hDlg, IDC_EDT_SRC, buf, MAX_PATH ) ;
		if ( !*buf ) {
			/* Source 領域が空の場合 */
			PostMessage( hDlg, WM_COMMAND, IDC_BTN_BROWSE, 0 ) ;
			return FALSE ;
		}
		attr = GetFileAttributes( buf ) ;
		if ( (attr == 0xFFFFFFFF) || (attr & FILE_ATTRIBUTE_DIRECTORY) ) {
			MessageBox( hDlg, TEXT("Could not find the file"), TEXT("Error"), MB_OK ) ;
			SetFocus( GetDlgItem( hDlg, IDC_EDT_SRC ) ) ;
			return FALSE ;
		}
		*buf = 0 ;
		GetDlgItemText( hDlg, IDC_EDT_DST, buf, MAX_PATH ) ;
		if ( !*buf ) {
			/* Destination 領域が空の場合 */
			SetFocus( GetDlgItem( hDlg, IDC_EDT_DST ) ) ;
			return FALSE ;
		}
		GetDlgItemText( hDlg, IDC_EDT_SRC, m_sSrcPath, MAX_PATH ) ;
		GetDlgItemText( hDlg, IDC_EDT_DST, m_sDstPath, MAX_PATH ) ;
		EndDialog( hDlg, TRUE ) ;
		break ;
	case IDC_EDT_SRC:
		if ( GET_WM_COMMAND_CMD( wp, lp ) == EN_CHANGE ) {
			if ( !SendDlgItemMessage( hDlg, IDC_EDT_DST, EM_GETMODIFY, 0, 0 ) ) {
				*buf = 0 ;
				GetDlgItemText( hDlg, IDC_EDT_SRC, buf, MAX_PATH ) ;
				SetDlgItemText( hDlg, IDC_EDT_DST, buf ) ;
			}
		}
		break ;
	case IDC_BTN_BROWSE:
		if ( FileOpenDlg( m_hInst, hDlg, buf, sizeof buf ) ) {
			SetDlgItemText( hDlg, IDC_EDT_SRC, buf ) ;
			SetFocus( GetDlgItem( hDlg, IDC_EDT_SRC ) ) ;
		}
		break ;
	}
	return 0 ;
}

BOOL
AddDialogExec( HINSTANCE hInst, HWND hWnd, BOOL fEdit, TransInfo *pInfo )
{
	AddDialog		*pDialog = new AddDialog( hInst, fEdit, pInfo ) ;

	if ( !DialogBoxParam( hInst, MAKEINTRESOURCE(IDD_DLG_ADD), hWnd, (DLGPROC) DlgProc, (LPARAM) pDialog ) ) {
		delete pDialog ;
		return FALSE ;
	}
	pDialog->PutToInfo( pInfo ) ;
	delete pDialog ;
	return TRUE ;
}

/*-------------------------------------------------------------------------*
 * Transfer ダイアログ
 *-------------------------------------------------------------------------*/
class TransferDialog : public Window {
	HINSTANCE	m_hInst ;
	DWORD		m_dwMode ;
	MainApp		*m_pApp ;

public:
	TransferDialog( HINSTANCE hInst, DWORD mode, MainApp *pApp ) {
		m_hInst = hInst ;
		m_dwMode = mode ;
		m_pApp = pApp ;
	}
	~TransferDialog() {}

	void	WMCreate( HWND hDlg ) ;
	LRESULT	WMCommand( HWND hDlg, UINT msg, WPARAM wp, LPARAM lp ) ;
} ;

void
TransferDialog::WMCreate( HWND hDlg )
{
	SendDlgItemMessage( hDlg, IDC_LST_PROGRESS, LB_RESETCONTENT, 0, 0 ) ;
	PostMessage( hDlg, WM_COMMAND, IDM_START, 0 ) ;
}

LRESULT
TransferDialog::WMCommand( HWND hDlg, UINT msg, WPARAM wp, LPARAM lp )
{
	switch ( GET_WM_COMMAND_ID( wp, lp ) ) {
	case IDC_BTN_ABORT:
		m_pApp->AbortTransfer() ;
		EndDialog( hDlg, FALSE ) ;
		break ;
	case IDM_START:
		m_pApp->StartTransfer( m_dwMode,
					hDlg,
					GetDlgItem( hDlg, IDC_LST_PROGRESS ),
					GetDlgItem( hDlg, IDC_CAP_MESSAGE ) ) ;
		break ;
	case IDM_END:
		EndDialog( hDlg, TRUE ) ;
		break ;
	case IDM_ERROR:
		SetDlgItemText( hDlg, IDC_BTN_ABORT, TEXT("&Close") ) ;
		break ;
	}
	return 0 ;
}

BOOL
TransferDialogExec( HINSTANCE hInst, HWND hWnd, DWORD dwMode, MainApp *pApp )
{
	TransferDialog		*pDialog = new TransferDialog( hInst, dwMode, pApp ) ;

	if ( !DialogBoxParam( hInst, MAKEINTRESOURCE(IDD_DLG_TRANS), hWnd, (DLGPROC) DlgProc, (LPARAM) pDialog ) ) {
		delete pDialog ;
		return FALSE ;
	}
	delete pDialog ;
	return TRUE ;
}

void
catdir( LPTSTR buf, LPCTSTR dir, LPTSTR pPrevDir )
{
	DWORD	len ;
	LPTSTR	ptr ;

	if ( !_tcscmp( dir, TEXT(".") ) ) {
		return ;
	} else if ( !_tcscmp( dir, TEXT("..") ) ) {
		ptr = _tcsrchr( buf, TEXT('\\') ) ;
		if ( ptr && pPrevDir ) {
			_tcscpy( pPrevDir, ptr + 1 ) ;
		}
		if ( ptr && ptr != buf ) {
			*ptr = 0 ;
		} else {
			_tcscpy( buf, TEXT("\\") ) ;
		}
		return ;
	}
	if ( pPrevDir ) {
		_tcscpy( pPrevDir, TEXT("..") ) ;
	}
	len = _tcslen( buf ) ;
	if ( !len || buf[ len - 1 ] != TEXT('\\') ) {
		_tcscat( buf, TEXT("\\") ) ;
	}
	_tcscat( buf, dir ) ;
}

/*-------------------------------------------------------------------------*
 * FileOpen ダイアログ
 *-------------------------------------------------------------------------*/
class FileOpenDialog : public Window {
	TCHAR		m_sWildCard[ MAX_PATH ] ;
	TCHAR		m_sFile[ MAX_PATH ] ;
	TCHAR		m_sDir[ MAX_PATH ] ;
	HFONT		m_hFont ;

public:
	FileOpenDialog( OPENFILENAME *pOfn ) ;
	~FileOpenDialog() ;

	LPCTSTR	GetText() const { return m_sFile ; }

	void	WMCreate( HWND hDlg ) ;
	LRESULT	WMCommand( HWND hDlg, UINT msg, WPARAM wp, LPARAM lp ) ;

	BOOL	EnumDir( HWND hWnd, LPDWORD pCount, LPDWORD pMem ) ;
	BOOL	ListDir( HWND hWnd, LPCTSTR pPrevDir ) ;
	BOOL	ChangeDir( HWND hDlg, LPBOOL pDone ) ;
	static LPCTSTR	c_sDir ;
	static LPCTSTR	c_sPatDir ;
	static LPCTSTR	c_sPatFile ;
} ;

LPCTSTR	FileOpenDialog::c_sDir = TEXT("   <DIR>") ;
LPCTSTR	FileOpenDialog::c_sPatDir = TEXT(" %s\t%s") ;
LPCTSTR	FileOpenDialog::c_sPatFile = TEXT("%s\t%8d") ;

FileOpenDialog::FileOpenDialog( OPENFILENAME *pOfn )
{
	LOGFONT         lf ;
	LPCTSTR			ptr ;

	memset( &lf, 0, sizeof lf ) ;
	lf.lfPitchAndFamily = FIXED_PITCH | FF_MODERN ;
	lf.lfCharSet = 128 ;
	lf.lfHeight = 13 ;
	m_hFont = CreateFontIndirect( &lf ) ;
	_tcscpy( m_sWildCard, TEXT("*.*") ) ;
	ptr = pOfn->lpstrFilter ;
	if ( ptr ) {
		if ( *ptr ) {
			ptr += _tcslen( ptr ) + 1 ;
			if ( *ptr ) {
				_tcscpy( m_sWildCard, ptr ) ;
			}
		}
	}
	_tcscpy( m_sFile, pOfn->lpstrFile ? pOfn->lpstrFile : TEXT("") ) ;
	_tcscpy( m_sDir, pOfn->lpstrInitialDir ? pOfn->lpstrInitialDir : TEXT("\\") ) ;
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

	SetDlgItemText( hDlg, IDC_CAP_DIR, m_sDir ) ;
	/* ディレクトリリストのフォントを設定する */
	SendMessage( hWnd, WM_SETFONT, (WPARAM) m_hFont, (LPARAM) TRUE ) ;
	/* TABの位置を設定する */
	GetClientRect( hDlg, &rect ) ;
	w = rect.right - 8 * 10 ;
	tabs[0] = w / ((LOWORD(GetDialogBaseUnits()) + 3) / 4) ;
	SendMessage( hWnd, LB_SETTABSTOPS, 1, (LPARAM) tabs ) ;
	/* 初期ディレクトリのリストを作成する */
	if ( !ListDir( hWnd, NULL ) ) {
		_tcscpy( m_sDir, TEXT("\\") ) ;
		ListDir( hWnd, NULL ) ;
	}
}

LRESULT
FileOpenDialog::WMCommand( HWND hDlg, UINT msg, WPARAM wp, LPARAM lp )
{
	LPTSTR	ptr ;
	BOOL	fDone ;
	DWORD	index ;
	TCHAR	buf[ MAX_PATH ] ;

	switch ( GET_WM_COMMAND_ID( wp, lp ) ) {
	case IDCANCEL:
		EndDialog( hDlg, FALSE ) ;
		return TRUE ;
	case IDC_LST_FILE:
		switch ( GET_WM_COMMAND_CMD( wp, lp ) ) {
		case LBN_SELCHANGE:
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
			if ( ChangeDir( hDlg, &fDone ) ) {
				if ( fDone ) {
					EndDialog( hDlg, TRUE ) ;
				}
				return TRUE ;
			}
			break ;
		}
		break ;
	case IDOK:
		if ( GetFocus() == GetDlgItem( hDlg, IDC_LST_FILE ) ) {
			SendMessage( hDlg, WM_COMMAND, MAKELONG(IDC_LST_FILE, LBN_DBLCLK), 0 ) ;
			return TRUE ;
		}
		GetDlgItemText( hDlg, IDC_EDT_FILE, buf, MAX_PATH ) ;
		if ( *buf ) {
			_tcscpy( m_sFile, m_sDir ) ;
			catdir( m_sFile, buf, NULL ) ;
			EndDialog( hDlg, TRUE ) ;
		}
		return TRUE ;
	}
	return 0 ;
}

BOOL
FileOpenDialog::EnumDir( HWND hWnd, LPDWORD pCount, LPDWORD pMem )
{
	WIN32_FIND_DATA	find ;
	HANDLE			hFind ;
	TCHAR			findPath[ MAX_PATH ] ;
	BOOL			fAdd = !pCount && !pMem ;
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
	catdir( findPath, m_sWildCard, NULL ) ;
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
GetOpenFileName2( OPENFILENAME *pOfn )
{
	FileOpenDialog		*pDialog = new FileOpenDialog( pOfn ) ;

	if ( !DialogBoxParam( pOfn->hInstance, MAKEINTRESOURCE(IDD_DLG_FOPEN), pOfn->hwndOwner, (DLGPROC) DlgProc, (LPARAM) pDialog ) ) {
		delete pDialog ;
		return FALSE ;
	}
	if ( pOfn->lpstrFile ) {
		_tcscpy( pOfn->lpstrFile, pDialog->GetText() ) ;
	}
	delete pDialog ;
	return TRUE ;
}
