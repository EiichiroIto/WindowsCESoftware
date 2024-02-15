#define	STRICT
#include	<windows.h>
#include	<windowsx.h>
#include	<commctrl.h>
#include	<commdlg.h>
#include	<tchar.h>
#include	"basewin.h"
#include	"resource.h"
#include	"dialog.h"

extern	DWORD	AppVersion ;
extern	DWORD	DllVersion ;

static HFONT	CreateListboxFont() ;

extern	TCHAR	g_szFolder[ MAX_PATH ] ;
extern	TCHAR	g_szFile[ MAX_PATH ] ;
extern	BOOL	g_fToFile ;
extern	HWND	g_hWndDialog ;
extern	DWORD	g_dwSleep ;

BOOL	SelDirDialogExec( HINSTANCE hInst, HWND hWnd, LPTSTR path ) ;

/*
 * 設定ダイアログ
 */
class SetupDialog : public Window {
	HINSTANCE	m_hInst ;

public:
	SetupDialog( HINSTANCE hInst ) { m_hInst = hInst ; }
	~SetupDialog() {}

	void	WMCreate( HWND hDlg ) ;
	LRESULT	WMCommand( HWND hDlg, UINT msg, WPARAM wp, LPARAM lp ) ;
} ;

void
SetupDialog::WMCreate( HWND hDlg )
{
	TCHAR	buf[ 128 ] ;

	/* バージョン情報を設定する */
	wsprintf( buf, TEXT("CBSave for Windows%c CE Ver %d.%02d"),
			  0xAE, AppVersion / 100, AppVersion % 100 ) ;
	SetWindowText( hDlg, buf ) ;
	wsprintf( buf, TEXT("KCTRL Ver %d.%02d\r\nCopyright %c 1997 %s\r\n%s"),
			  DllVersion / 100, DllVersion % 100,
			  0xA9,
			  TEXT("Eiichiroh Itoh"),
			  TEXT("All Rights Reserved") ) ;
	SetDlgItemText( hDlg, IDC_COPYRIGHT, buf ) ;
	/* 項目の値を設定する */
	SetDlgItemText( hDlg, IDC_EDT_FILE, g_szFile ) ;
	SetDlgItemText( hDlg, IDC_EDT_FOLDER, g_szFolder ) ;
	SendDlgItemMessage( hDlg, g_fToFile ? IDC_RB_FILE : IDC_RB_FOLDER,
						BM_SETCHECK, (WPARAM) 1, 0 ) ;
	SetForegroundWindow( hDlg ) ;
}

LRESULT
SetupDialog::WMCommand( HWND hDlg, UINT msg, WPARAM wp, LPARAM lp )
{
	DWORD	attr, ret = 0 ;
	TCHAR	buf[ MAX_PATH ] ;

	if ( wp == IDCANCEL ) {
		EndDialog( hDlg, 0 ) ;
		return TRUE ;
	} else if ( wp == IDC_BTN_DIR ) {
		/* Browseボタンが押された場合 */
		SendDlgItemMessage( hDlg, IDC_RB_FILE,
							BM_SETCHECK, (WPARAM) 0, 0 ) ;
		SendDlgItemMessage( hDlg, IDC_RB_FOLDER,
							BM_SETCHECK, (WPARAM) 1, 0 ) ;
		GetDlgItemText( hDlg, IDC_EDT_FOLDER, buf, MAX_PATH ) ;
		if ( SelDirDialogExec( m_hInst, hDlg, buf ) ) {
			SetDlgItemText( hDlg, IDC_EDT_FOLDER, buf ) ;
		}
		return TRUE ;
	} else if ( wp == IDC_BTN_EXIT ) {
		EndDialog( hDlg, DIALOG_RET_EXIT ) ;
		return TRUE ;
	} else if ( wp == IDC_BTN_UNINST ) {
		if ( MessageBox( hDlg, TEXT("Uninstall this application?"), TEXT("Uninstall"), MB_YESNO ) == IDYES ) {
			EndDialog( hDlg, DIALOG_RET_UNINSTALL ) ;
		}
		return TRUE ;
	} else if ( wp != IDOK ) {
		return FALSE ;
	}
	/* OKが押された場合 */
	/* Fileへコピーする場合のフラグ */
	g_fToFile = SendDlgItemMessage( hDlg, IDC_RB_FILE, BM_GETCHECK, 0, 0 ) ? 1 : 0 ;
	if ( g_fToFile ) {
		/* ファイルのパス */
		GetDlgItemText( hDlg, IDC_EDT_FILE, buf, MAX_PATH ) ;
		attr = GetFileAttributes( buf ) ;
		if ( !*buf || ((attr != 0xFFFFFFFF) && (attr & FILE_ATTRIBUTE_DIRECTORY)) ) {
			/* 空，ディレクトリの場合は戻る */
			return TRUE ;
		}
		_tcscpy( g_szFile, buf ) ;
	} else {
		/* フォルダーのパス */
		GetDlgItemText( hDlg, IDC_EDT_FOLDER, buf, MAX_PATH ) ;
		attr = GetFileAttributes( buf ) ;
		if ( !*buf || (attr == 0xFFFFFFFF) || !(attr & FILE_ATTRIBUTE_DIRECTORY) ) {
			/* 空，ファイルが存在しない，ディレクトリ以外の場合は戻る */
			return TRUE ;
		}
		_tcscpy( g_szFolder, buf ) ;
	}
	/* 変更フラグ */
	ret |= DIALOG_RET_CHANGE ;
	EndDialog( hDlg, ret ) ;
	return TRUE ;
}

BOOL
SetupDialogExec( HINSTANCE hInst, HWND hWnd )
{
	BOOL	ret ;
	SetupDialog		*pDialog = new SetupDialog( hInst ) ;

	ret = DialogBoxParam( hInst, MAKEINTRESOURCE(IDD_DLG_SETUP), hWnd, (DLGPROC) DlgProc, (LPARAM) pDialog ) ;
	delete pDialog ;
	return ret ;
}

/*
 * ディレクトリ選択ダイアログ
 */
class SelDirDialog : public Window {
	TCHAR	m_path[ MAX_PATH ] ;
	HFONT	m_hFont ;

public:
	SelDirDialog( LPCTSTR path ) ;
	~SelDirDialog() ;

	void	WMCreate( HWND hDlg ) ;
	LRESULT	WMCommand( HWND hDlg, UINT msg, WPARAM wp, LPARAM lp ) ;

	void	ListBoxReset( HWND hDlg, HWND hWnd, LPCTSTR path ) ;
	BOOL	ListBoxNotify( HWND hDlg, HWND hWnd, DWORD code, LPTSTR path ) ;
	LPCTSTR	get_path() const { return m_path ; }
} ;

SelDirDialog::SelDirDialog( LPCTSTR path )
{
	_tcscpy( m_path, path ) ;
	/* フォントを取得する */
	m_hFont = CreateListboxFont() ;
}

SelDirDialog::~SelDirDialog()
{
	DeleteObject( m_hFont ) ;
}

void
SelDirDialog::WMCreate( HWND hDlg )
{
	SetWindowText( hDlg, TEXT("Setup OpenPost Directory") ) ;
	SetDlgItemText( hDlg, IDC_DIRECTORY, m_path ) ;
	HWND	hWnd = GetDlgItem( hDlg, IDC_LST_DIRECTORY ) ;
	/* リストボックスのフォントを設定する */
	SendMessage( hWnd, WM_SETFONT, (WPARAM) m_hFont, (LPARAM) TRUE ) ;
	ListBoxReset( hDlg, hWnd, m_path ) ;
}

LRESULT
SelDirDialog::WMCommand( HWND hDlg, UINT msg, WPARAM wp, LPARAM lp )
{
	DWORD	ret ;
	HWND	hWnd ;

	switch ( GET_WM_COMMAND_ID( wp, lp ) ) {
	case IDCANCEL:
		EndDialog( hDlg, FALSE ) ;
		return 0 ;
	case IDOK:
		hWnd = GetDlgItem( hDlg, IDC_LST_DIRECTORY ) ;
		if ( GetFocus() == hWnd ) {
			if ( ListBoxNotify( hDlg, hWnd, LBN_DBLCLK, m_path ) ) {
				SetDlgItemText( hDlg, IDC_DIRECTORY, m_path ) ;
				break ;
			}
		}
		/* ベースディレクトリを取得する */
		GetDlgItemText( hDlg, IDC_DIRECTORY, m_path, MAX_PATH ) ;
		if ( !*m_path ) {
			break ;
		}
		ret = GetFileAttributes( m_path ) ;
		if ( ret == 0xFFFFFFFF ) {
			if ( MessageBox( hDlg, TEXT("Directory doesn't exist, create it?"), TEXT("Setup"), MB_YESNO|MB_ICONQUESTION ) == IDYES ) {
				CreateDirectory( m_path, NULL ) ;
				if ( ListBoxNotify( hDlg, hWnd, LBN_DBLCLK, m_path ) ) {
					SetDlgItemText( hDlg, IDC_DIRECTORY, m_path ) ;
					break ;
				}
			}
			break ;
		} else if ( !(ret & FILE_ATTRIBUTE_DIRECTORY) ) {
			/* 既存のディレクトリではない場合 */
			MessageBox( hDlg, TEXT("Please specify existing directory."), TEXT("Setup"), MB_OK|MB_ICONSTOP ) ;
			break ;
		}
		EndDialog( hDlg, TRUE ) ;
		break ;
	case IDC_LST_DIRECTORY:
		if ( ListBoxNotify( hDlg, (HWND) lp, HIWORD(wp), m_path ) ) {
			SetDlgItemText( hDlg, IDC_DIRECTORY, m_path ) ;
		}
		break ;
	}
	return 0 ;
}

void
SelDirDialog::ListBoxReset( HWND hDlg, HWND hWnd, LPCTSTR path )
{
	WIN32_FIND_DATA	find ;
	HANDLE			hFind ;
	TCHAR			findPath[ MAX_PATH ] ;

	/* リストボックスをクリアする */
	SendMessage( hWnd, LB_RESETCONTENT, 0, 0 ) ;
	SendMessage( hWnd, LB_ADDSTRING, 0, (LPARAM) TEXT("..") ) ;
	/* 検索用のワイルドカードを作成する */
	_tcscpy( findPath, path ) ;
	_tcscat( findPath, TEXT("\\*.*") ) ;
	hFind = FindFirstFile( findPath, &find ) ;
	if ( hFind == INVALID_HANDLE_VALUE ) {
		return ;
	}
	do {
		if ( !(find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) {
			continue ;
		} else if ( !_tcscmp( find.cFileName, TEXT(".") ) ) {
			continue ;
		} else if ( !_tcscmp( find.cFileName, TEXT("..") ) ) {
			continue ;
		}
		/* リストボックスに項目を追加する */
		SendMessage( hWnd, LB_ADDSTRING, 0, (LPARAM) find.cFileName ) ;
	} while ( FindNextFile( hFind, &find ) ) ;
	FindClose( hFind ) ;
}

BOOL
SelDirDialog::ListBoxNotify( HWND hDlg, HWND hWnd, DWORD code, LPTSTR path )
{
	DWORD	index, len ;
	TCHAR	buf[ MAX_PATH ], *ptr ;

	if ( code == LBN_DBLCLK ) {
		index = SendMessage( hWnd, LB_GETCURSEL, 0, 0 ) ;
		if ( index == LB_ERR ) {
			return FALSE ;
		}
		SendMessage( hWnd, LB_GETTEXT, (WPARAM) index, (LPARAM) buf ) ;
		if ( !_tcscmp( buf, TEXT("..") ) ) {
			ptr = _tcsrchr( path, TEXT('\\') ) ;
			if ( ptr ) {
				*ptr = 0 ;
			}
		} else {
			len = _tcslen( path ) ;
			if ( !len || path[ len - 1 ] != TEXT('\\') ) {
				path[ len ++ ] = TEXT('\\') ;
			}
			_tcscpy( &path[ len ], buf ) ;
		}
		ListBoxReset( hDlg, hWnd, path ) ;
		return TRUE ;
	}
	return FALSE ;
}

BOOL
SelDirDialogExec( HINSTANCE hInst, HWND hWnd, LPTSTR path )
{
	BOOL			ret ;
	LPCTSTR			ptr ;
	SelDirDialog		*pDialog = new SelDirDialog( path ) ;

	if ( !DialogBoxParam( hInst, MAKEINTRESOURCE(IDD_DLG_SELDIR), hWnd, (DLGPROC) DlgProc, (LPARAM) pDialog ) ) {
		delete pDialog ;
		return FALSE ;
	}
	ret = FALSE ;
	ptr = pDialog->get_path() ;
	if ( _tcscmp( path, ptr ) ) {
		_tcscpy( path, ptr ) ;
		ret = TRUE ;
	}
	delete pDialog ;
	return ret ;
}

/*
 * YES/NO選択ダイアログ
 */
class YesNoDialog : public Window {
	HINSTANCE	m_hInst ;
	LPCTSTR		m_pMessage ;
	LPCTSTR		m_pTitle ;

public:
	YesNoDialog( HINSTANCE hInst, LPCTSTR pMessage, LPCTSTR pTitle ) {
		m_hInst = hInst ;
		m_pMessage = pMessage ;
		m_pTitle = pTitle ;
	}
	~YesNoDialog() {}

	void	WMCreate( HWND hDlg ) ;
	LRESULT	WMCommand( HWND hDlg, UINT msg, WPARAM wp, LPARAM lp ) ;
} ;

void
YesNoDialog::WMCreate( HWND hDlg )
{
	g_hWndDialog = hDlg ;
	/* 項目の値を設定する */
	SetWindowText( hDlg, m_pTitle ) ;
	SetDlgItemText( hDlg, IDC_MESSAGE, m_pMessage ) ;
	SetDlgItemInt( hDlg, IDC_EDT_SLEEP, g_dwSleep, 0 ) ;
	SetForegroundWindow( hDlg ) ;
}

LRESULT
YesNoDialog::WMCommand( HWND hDlg, UINT msg, WPARAM wp, LPARAM lp )
{
	switch ( wp ) {
	case IDOK:
		return TRUE ;
	case IDCANCEL:
	case IDC_BTN_NO:
		/* NOボタンが押された場合 */
		EndDialog( hDlg, 0 ) ;
		return TRUE ;
	case IDC_BTN_YES:
		/* YESボタンが押された場合 */
		g_dwSleep = GetDlgItemInt( hDlg, IDC_EDT_SLEEP, NULL, FALSE ) ;
		EndDialog( hDlg, 1 ) ;
		return TRUE ;
	}
	return TRUE ;
}

BOOL
YesNoDialogExec( HINSTANCE hInst, HWND hWnd, LPCTSTR pMessage, LPCTSTR pTitle )
{
	BOOL			ret ;
	YesNoDialog		*pDialog = new YesNoDialog( hInst, pMessage, pTitle ) ;

	ret = DialogBoxParam( hInst, MAKEINTRESOURCE(IDD_DLG_YESNO), hWnd, (DLGPROC) DlgProc, (LPARAM) pDialog ) ;
	g_hWndDialog = NULL ;
	delete pDialog ;
	return ret ;
}

/*
 * リストボックス用の固定ピッチフォントを作成する
 */
static HFONT
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
