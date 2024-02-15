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
#include	"kctrl.h"
#include	"controls.h"

class Label {
protected:
	HWND	m_hWnd ;
	HWND	m_hWndParent ;	/* コントロールの親ウィンドウ */
	WORD	m_idCtrl ;		/* コントロールの識別番号 */
	LPTSTR	m_pText ;		/* 表示する文字列 */
	BOOL	m_fInvert ;		/* 反転フラグ */
	BOOL	m_fCross ;		/* クロスフラグ */

public:
	Label() ;
	~Label() ;

	void	WMCreate( HWND hWnd, LPCREATESTRUCT lpcs ) ;
	void	WMMouseDown( WPARAM wParam, LPARAM lParam ) ;
	void	WMPaint() const ;

	void	SetText( LPCTSTR buf ) ;
	void	SetInvert( BOOL fInver ) ;
	void	SetCross( BOOL fCross ) ;

	friend	LRESULT CALLBACK	LabelWndProc( HWND hWnd,  UINT msg, WPARAM wParam, LPARAM lParam ) ;
} ;
typedef	Label	*PLabel ;

Label::Label()
{
	m_pText = 0 ;
	m_fInvert = m_fCross = FALSE ;
}

Label::~Label()
{
	if ( m_pText ) {
		delete[] m_pText ;
	}
}

void
Label::WMCreate( HWND hWnd, LPCREATESTRUCT lpcs )
{
	m_hWnd = hWnd ;
	m_hWndParent = lpcs->hwndParent ;
	m_idCtrl = (WORD) lpcs->hMenu ;
	SetText( lpcs->lpszName ) ;
}

void
Label::WMMouseDown( WPARAM wParam, LPARAM lParam )
{
	if ( m_hWndParent ) {
		PostMessage( m_hWndParent, WM_COMMAND, MAKEWPARAM( m_idCtrl, LAN_CLICK ), (LPARAM) m_hWnd ) ;
	}
}

void
Label::WMPaint() const
{
	PAINTSTRUCT	ps ;
	HDC			hDC ;
	RECT		rect ;

	hDC = ::BeginPaint( m_hWnd, &ps ) ;
	if ( m_pText ) {
		::GetClientRect( m_hWnd, &rect ) ;
		::KDrawText( hDC, m_pText, -1, &rect, 0 ) ;
		if ( m_fCross ) {
			POINT	points[2] ;
			points[0].x = rect.left ;
			points[0].y = rect.top ;
			points[1].x = rect.right - 1 ;
			points[1].y = rect.bottom - 1 ;
			Polyline( hDC, points, 2 ) ;
			points[0].x = rect.left ;
			points[0].y = rect.bottom - 1 ;
			points[1].x = rect.right - 1 ;
			points[1].y = rect.top ;
			Polyline( hDC, points, 2 ) ;
		}
		if ( m_fInvert ) {
			::PatBlt( hDC, rect.left, rect.top, rect.right - rect.left + 1, rect.bottom - rect.top + 1, DSTINVERT ) ;
		}
	}
	::EndPaint( m_hWnd, &ps ) ;
}

/*
 * 文字列を設定する
 */
void
Label::SetText( LPCTSTR buf )
{
	DWORD	len = _tcslen( buf ) ;

	if ( m_pText ) {
		delete[] m_pText ;
	}
	m_pText = new TCHAR[ (len + 1) * sizeof (TCHAR) ] ;
	_tcscpy( m_pText, buf ) ;
	::InvalidateRect( m_hWnd, NULL, TRUE ) ;
}

/*
 * 反転表示を設定する
 */
void
Label::SetInvert( BOOL fInvert )
{
	m_fInvert = fInvert ;
	::InvalidateRect( m_hWnd, NULL, TRUE ) ;
}

/*
 * ×印表示を設定する
 */
void
Label::SetCross( BOOL fCross )
{
	m_fCross = fCross ;
	::InvalidateRect( m_hWnd, NULL, TRUE ) ;
}

BOOL
LabelRegisterClass( HINSTANCE hInst )
{
	WNDCLASS	wc ;

	wc.style         = NULL ;
	wc.lpfnWndProc   = (WNDPROC) LabelWndProc ;
	wc.cbClsExtra    = 0 ;
	wc.cbWndExtra    = 4 ;
	wc.hInstance     = hInst ;
	wc.hIcon         = NULL ;
	wc.hCursor       = NULL ;
	wc.hbrBackground = (HBRUSH) GetStockObject( WHITE_BRUSH ) ;
	wc.lpszMenuName  = 0 ;
	wc.lpszClassName = CTRL_LABEL ;
	return ::RegisterClass( &wc ) ;
}

LRESULT CALLBACK
LabelWndProc( HWND hWnd,  UINT msg, WPARAM wParam, LPARAM lParam )
{
	PLabel	pWindow ;

	if ( msg == WM_CREATE ) {
		LPCREATESTRUCT	lpStruct = (LPCREATESTRUCT) lParam ;
		pWindow = new Label() ;
		SetWindowLong( hWnd, 0, (LONG) pWindow ) ;
		pWindow->WMCreate( hWnd, lpStruct ) ;
		return 0 ;
	}
	pWindow = (PLabel) GetWindowLong( hWnd, 0 ) ;
	if ( !pWindow ) {
		return DefWindowProc( hWnd, msg, wParam, lParam ) ;
	}
	switch ( msg ) {
	case WM_DESTROY:
		delete pWindow ;
		SetWindowLong( hWnd, 0, (LONG) 0 ) ;
		break ;
	case WM_PAINT:			pWindow->WMPaint() ;						break ;
	case WM_LBUTTONDOWN:	pWindow->WMMouseDown( wParam, lParam ) ;	break ;
	case WM_SETTEXT:		pWindow->SetText( (LPCTSTR) lParam ) ;		break ;

	case LAM_SETINVERT:		pWindow->SetInvert( (DWORD) wParam ) ;		break ;
	case LAM_SETCROSS:		pWindow->SetCross( (DWORD) wParam ) ;		break ;
	default:
		return DefWindowProc( hWnd, msg, wParam, lParam ) ;
	}
	return 0 ;
}
