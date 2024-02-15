#ifndef	UNICODE
#define	UNICODE
#endif	/* UNICODE */
#ifndef	_UNICODE
#define	_UNICODE
#endif	/* _UNICODE */
#define	STRICT
#include	<windows.h>
#include	<windowsx.h>
#include	<commctrl.h>
#include	<tchar.h>
#include	"basewin.h"

BOOL APIENTRY
DlgProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	PWindow		pWindow ;

	switch ( message ) {
	case WM_INITDIALOG:
		SetWindowLong( hDlg, DWL_USER, lParam ) ;
		pWindow = (PWindow) lParam ;
		pWindow->WMCreate( hDlg ) ;
		return TRUE ;
	case WM_COMMAND:
		pWindow = (PWindow) GetWindowLong( hDlg, DWL_USER ) ;
		if ( pWindow == NULL ) {
			if ( (wParam == IDOK) || (wParam == IDCANCEL) ) {
				EndDialog( hDlg, TRUE ) ;
				return TRUE ;
			}
		} else {
			return pWindow->WMCommand( hDlg, message, wParam, lParam ) ;
		}
		break ;
	case WM_DRAWITEM:
		pWindow = (PWindow) GetWindowLong( hDlg, DWL_USER ) ;
		if ( pWindow ) {
			pWindow->WMDrawItem( (HWND) wParam, (LPDRAWITEMSTRUCT) lParam ) ;
		}
		break ;
	case WM_GETDLGCODE:
		pWindow = (PWindow) GetWindowLong( hDlg, DWL_USER ) ;
		if ( pWindow ) {
			return pWindow->WMGetDlgCode() ;
		}
		break ;
	}
	return FALSE ;
}

LRESULT CALLBACK
WndProc( HWND hWnd,  UINT msg, WPARAM wParam, LPARAM lParam )
{
	PWindow		pWindow ;

	if ( msg == WM_CREATE ) {
		LPCREATESTRUCT	lpStruct = (LPCREATESTRUCT) lParam ;
		pWindow = (PWindow) lpStruct->lpCreateParams ;
		SetWindowLong( hWnd, 0, (LONG) pWindow ) ;
		pWindow->WMCreate( hWnd ) ;
		return 0 ;
	}
	pWindow = (PWindow) GetWindowLong( hWnd, 0 ) ;
	if ( !pWindow ) {
		return DefWindowProc( hWnd, msg, wParam, lParam ) ;
	}
	switch ( msg ) {
	case WM_COMMAND:
		return pWindow->WMCommand( hWnd, msg, wParam, lParam ) ;
	case WM_NOTIFY:
		pWindow->WMNotify( hWnd, (int) wParam, (LPNMHDR) lParam ) ;
		break ;
	case WM_SETFOCUS:
		pWindow->WMSetFocus() ;
		return DefWindowProc( hWnd, msg, wParam, lParam ) ;
	case WM_KILLFOCUS:
		pWindow->WMKillFocus( (HWND) wParam ) ;
		return DefWindowProc( hWnd, msg, wParam, lParam ) ;
	case WM_PAINT:
		pWindow->WMPaint() ;
		return DefWindowProc( hWnd, msg, wParam, lParam ) ;
	case WM_CHAR:			pWindow->WMChar( (TCHAR) wParam, lParam ) ;		break ;
	case WM_KEYDOWN:		pWindow->WMKeyDown( (int) wParam, lParam ) ;	break ;
	case WM_ACTIVATE:
		pWindow->WMActivate( LOWORD(wParam), (BOOL) HIWORD(wParam), (HWND) lParam ) ;
		return DefWindowProc( hWnd, msg, wParam, lParam ) ;
	case WM_DESTROY:
		pWindow->WMDestroy() ;
		delete pWindow ;
		SetWindowLong( hWnd, 0, (LONG) 0 ) ;
		break ;
	case WM_CLOSE:
		if ( !pWindow->WMClose() ) {
			DefWindowProc( hWnd, msg, wParam, lParam ) ;
		}
		break ;
	case WM_COPYDATA:
		return pWindow->WMCopydata( (PCOPYDATASTRUCT) lParam ) ;
	default:
		return DefWindowProc( hWnd, msg, wParam, lParam ) ;
	}
	return 0 ;
}
