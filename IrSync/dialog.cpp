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

void	catdir( LPTSTR body, LPCTSTR adddir, LPTSTR pPrevDir ) ;

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

	wsprintf( buf, TEXT("About %s"), szTitleName ) ;
	SetWindowText( hDlg, buf ) ;
	wsprintf( buf, TEXT("%s for WinCE V%d.%02d"),
			  szTitleName, AppVersion / 100, AppVersion % 100 ) ;
	SetDlgItemText( hDlg, IDC_VERSION, buf ) ;
	wsprintf( buf, TEXT("Copyright %c 1998\r\n%s\r\n%s"),
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
		if ( MessageBox( hDlg, TEXT("Delete registry information for IrCopy?"), TEXT("UNINSTALL"), MB_ICONQUESTION|MB_YESNO ) == IDYES ) {
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
	case IDOK:
	case IDCANCEL:
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

/*-------------------------------------------------------------------------*
 * Openダイアログ
 *-------------------------------------------------------------------------*/
class OpenDialog : public Window {
	LPTSTR	m_pPath ;

public:
	OpenDialog( LPTSTR pPath ) : m_pPath(pPath) {}

	void	WMCreate( HWND hDlg ) ;
	LRESULT	WMCommand( HWND hDlg, UINT msg, WPARAM wp, LPARAM lp ) ;
} ;

void
OpenDialog::WMCreate( HWND hDlg )
{
	SetDlgItemText( hDlg, IDC_EDT_OPEN, m_pPath ) ;
	SendDlgItemMessage( hDlg, IDC_CHK_DELETE, BM_SETCHECK, (WPARAM) 1, 0 ) ;
}

LRESULT
OpenDialog::WMCommand( HWND hDlg, UINT msg, WPARAM wp, LPARAM lp )
{
	DWORD	len ;
	TCHAR	buf[ MAX_PATH ], *ptr ;

	switch ( wp ) {
	case IDCANCEL :
		EndDialog( hDlg, FALSE ) ;
		break ;
	case IDOK:
		GetDlgItemText( hDlg, IDC_EDT_OPEN, buf, 2 ) ;
		if ( !*buf ) {
			MessageBeep( MB_ICONHAND ) ;
			return 0 ;
		}
		GetDlgItemText( hDlg, IDC_EDT_OPEN, m_pPath, MAX_PATH ) ;
		EndDialog( hDlg, TRUE ) ;
		break ;
	case IDC_CHK_DELETE:
		GetDlgItemText( hDlg, IDC_EDT_OPEN, buf, MAX_PATH ) ;
		ptr = buf ;
		if ( *ptr == TEXT('"') ) {
			ptr ++ ;
		}
		len = _tcslen( ptr ) ;
		if ( len && ptr[ len - 1 ] == TEXT('"') ) {
			ptr[ len - 1 ] = 0 ;
		}
		SetDlgItemText( hDlg, IDC_EDT_OPEN, ptr ) ;
		SetFocus( GetDlgItem( hDlg, IDC_EDT_OPEN ) ) ;
		EnableWindow( GetDlgItem( hDlg, IDC_CHK_DELETE ), FALSE ) ;
		break ;
	}
	return 0 ;
}

BOOL
OpenDialogExec( HINSTANCE hInst, HWND hWnd, LPTSTR szPath )
{
	OpenDialog		*pDialog = new OpenDialog( szPath ) ;

	if ( !DialogBoxParam( hInst, MAKEINTRESOURCE(IDD_DLG_OPEN), hWnd, (DLGPROC) DlgProc, (LPARAM) pDialog ) ) {
		delete pDialog ;
		return FALSE ;
	}
	delete pDialog ;
	return TRUE ;
}
