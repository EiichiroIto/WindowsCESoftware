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

class TimeEdit {
protected:
	HWND	m_hWnd ;
	HWND	m_hWndParent ;		/* コントロールの親ウィンドウ */
	WORD	m_idCtrl ;			/* コントロールの識別番号 */
	TCHAR	m_sText[ 10 ] ;
	DWORD	m_cw, m_ch ;		/* 半角文字の幅と高さ */
	BOOL	m_fModify ;

public:
	TimeEdit() ;
	~TimeEdit() ;

	BOOL	Create() { return FALSE ; }

	void	WMCreate( HWND hWnd, LPCREATESTRUCT lpcs ) ;
	void	WMSetFocus() ;
	void	WMKillFocus( HWND hWnd ) ;
	void	WMMouseDown( WPARAM wParam, LPARAM lParam ) ;
	void	WMPaint() ;
	void	WMChar( TCHAR c, LONG keydata ) ;
	void	WMKeyDown( int nVirtKey, LONG lKeyData ) ;
	LRESULT	WMCommand( HWND hWnd, UINT msg, WPARAM wp, LPARAM lp ) ;

	void	KeyDownSub( int nVirtKey, BOOL fShift, BOOL fControl ) ;
	inline	void	KeyBS() ;
	inline	void	KeyESC() ;
	inline	void	KeyEnter() const ;
	inline	void	KeyTAB( BOOL fShift ) const ;
	void	SetTimeVal( DWORD min ) ;
	DWORD	GetTimeVal() const ;
	BOOL	GetModify() const { return m_fModify ; }
	void	SetModify( BOOL fModify ) { m_fModify = TRUE ; }

	friend	LRESULT CALLBACK	TimeEditWndProc( HWND hWnd,  UINT msg, WPARAM wParam, LPARAM lParam ) ;
} ;
typedef	TimeEdit	*PTimeEdit ;

TimeEdit::TimeEdit()
{
	m_sText[0] = 0 ;
	m_cw = GetFontHW() ;
	m_ch = GetFontH() ;
	m_fModify = TRUE ;
}

TimeEdit::~TimeEdit()
{
}

void
TimeEdit::WMCreate( HWND hWnd, LPCREATESTRUCT lpcs )
{
	m_hWnd = hWnd ;
	m_hWndParent = lpcs->hwndParent ;
	m_idCtrl = (WORD) lpcs->hMenu ;
}

void
TimeEdit::WMSetFocus()
{
	if ( ::CreateCaret( m_hWnd, NULL, 2, m_ch ) ) {
		::ShowCaret( m_hWnd ) ;
	}
	::SetCaretPos( _tcslen( m_sText ) * m_cw, 2 ) ;
	::InvalidateRect( m_hWnd, NULL, TRUE ) ;
}

void
TimeEdit::WMKillFocus( HWND hWnd )
{
	::HideCaret( m_hWnd ) ;
	::DestroyCaret() ;
	PostMessage( m_hWndParent, WM_COMMAND, MAKEWPARAM( m_idCtrl, TEN_KILLFOCUS ), (LPARAM) m_hWnd ) ;
}

void
TimeEdit::WMMouseDown( WPARAM wParam, LPARAM lParam )
{
	if ( GetFocus() != m_hWnd ) {
		SetFocus( m_hWnd ) ;
	}
}

void
TimeEdit::WMPaint()
{
	PAINTSTRUCT	ps ;
	HDC			hDC ;
	RECT		rect ;

	hDC = ::BeginPaint( m_hWnd, &ps ) ;
	GetClientRect( m_hWnd, &rect ) ;
	rect.top += 2 ;		rect.left += 2 ;
	rect.bottom -= 2 ;	rect.right -= 2 ;
	KDrawText( hDC, m_sText, -1, &rect, 0 ) ;
	if ( GetFocus() == m_hWnd ) {
		::SetCaretPos( _tcslen( m_sText ) * m_cw, 2 ) ;
	}
	::EndPaint( m_hWnd, &ps ) ;
}

void
TimeEdit::WMChar( TCHAR chCharCode, LONG lKeyData )
{
	TCHAR	ch = m_sText[0] ;
	DWORD	len = _tcslen( m_sText ) ;
	LPTSTR	dst = &m_sText[ len ] ;

	if ( chCharCode >= TEXT('0') && chCharCode <= TEXT('9') ) {
		switch ( len ) {
		case 1:
			if ( ch < TEXT('3') ) {
				break ;
			}
			dst = m_sText ;
			*dst++ = TEXT('0') ;
			*dst++ = ch ;
			//
		case 2:
			*dst++ = TEXT(':') ;
			//
		case 3:
			if ( TEXT('6') <= chCharCode && chCharCode <= TEXT('9') ) {
				*dst++ = TEXT('0') ;
			}
			break ;
		default:
			if ( len > 4 ) {
				return ;
			}
		}
		*dst++ = chCharCode ;
		*dst = 0 ;
	} else if ( chCharCode == TEXT(':') ) {
		switch ( len ) {
		case 0:
			*dst++ = TEXT('0') ;
			*dst++ = TEXT('0') ;
			break ;
		case 1:
			dst = m_sText ;
			*dst++ = TEXT('0') ;
			*dst++ = ch ;
			break ;
		default:
			if ( len > 2 ) {
				return ;
			}
		}
		*dst++ = chCharCode ;
		*dst = 0 ;
	} else {
		return ;
	}
	m_fModify = TRUE ;
	::InvalidateRect( m_hWnd, NULL, TRUE ) ;
	PostMessage( m_hWndParent, WM_COMMAND, MAKEWPARAM( m_idCtrl, TEN_CHANGE ), (LPARAM) m_hWnd ) ;
}

void
TimeEdit::WMKeyDown( int nVirtKey, LONG lKeyData )
{
	BOOL	fShift   = 0x80 & GetKeyState( VK_SHIFT ) ;
	BOOL	fControl = 0x80 & GetKeyState( VK_CONTROL ) ;

	KeyDownSub( nVirtKey, fShift, fControl ) ;
}

LRESULT
TimeEdit::WMCommand( HWND hWnd, UINT msg, WPARAM wp, LPARAM lp )
{
	return 0 ;
}

void
TimeEdit::KeyDownSub( int nVirtKey, BOOL fShift, BOOL fControl )
{
	switch ( nVirtKey ) {
	case VK_BACK:		KeyBS() ;			break ;
	case VK_ESCAPE:		KeyESC() ;			break ;
	case VK_RETURN:		KeyEnter() ;		break ;
	case VK_TAB:		KeyTAB( fShift ) ;	break ;
	case VK_DELETE:							break ;

	case VK_HOME:
	case VK_END:
	case VK_LEFT:
	case VK_RIGHT:
	case VK_UP:
	case VK_DOWN:
	case VK_PRIOR:
	case VK_NEXT:
		SendMessage( m_hWndParent, WM_COMMAND, MAKEWPARAM( m_idCtrl, TEN_KEYDOWN ), (LPARAM) nVirtKey ) ;
		break ;
	}
}

void
TimeEdit::KeyBS()
{
	DWORD	len = _tcslen( m_sText ) ;

	if ( len ) {
		m_sText[ len - 1 ] = 0 ;
		m_fModify = TRUE ;
		::InvalidateRect( m_hWnd, NULL, TRUE ) ;
		PostMessage( m_hWndParent, WM_COMMAND, MAKEWPARAM( m_idCtrl, TEN_CHANGE ), (LPARAM) m_hWnd ) ;
	}
}

void
TimeEdit::KeyESC()
{
	DWORD	len = _tcslen( m_sText ) ;

	if ( len ) {
		m_sText[0] = 0 ;
		m_fModify = TRUE ;
		::InvalidateRect( m_hWnd, NULL, TRUE ) ;
		PostMessage( m_hWndParent, WM_COMMAND, MAKEWPARAM( m_idCtrl, TEN_CHANGE ), (LPARAM) m_hWnd ) ;
	}
}

void
TimeEdit::KeyEnter() const
{
	SendMessage( m_hWndParent, WM_COMMAND, MAKEWPARAM( m_idCtrl, TEN_ENTER ), (LPARAM) m_hWnd ) ;
}

void
TimeEdit::KeyTAB( BOOL fShift ) const
{
	WORD	cmd = fShift ? TEN_STAB : TEN_TAB ;
	SendMessage( m_hWndParent, WM_COMMAND, MAKEWPARAM( m_idCtrl, cmd ), (LPARAM) m_hWnd ) ;
}

DWORD
TimeEdit::GetTimeVal() const
{
	DWORD	hour = 0 ;
	BOOL	fMin = FALSE ;
	TCHAR	c, tmpbuf[ 10 ] ;
	LPTSTR	dst = tmpbuf ;
	LPCTSTR	str = m_sText ;

	while ( ( c = *str++ ) != 0 ) {
		if ( c == TEXT(':') ) {
			fMin = TRUE ;
			*dst = 0 ;
			hour = _ttol( tmpbuf ) ;
			dst = tmpbuf ;
		} else {
			*dst++ = c ;
		}
	}
	*dst = 0 ;
	if ( !fMin ) {
		return _ttol( tmpbuf ) * 60 ;
	}
	return hour * 60 + _ttol( tmpbuf ) ;
}

void
TimeEdit::SetTimeVal( DWORD min )
{
	min %= 60 * 24 ;
	wsprintf( m_sText, TEXT("%02d:%02d"), min / 60, min % 60 ) ;
	::InvalidateRect( m_hWnd, NULL, TRUE ) ;
}

BOOL
TimeEditRegisterClass( HINSTANCE hInst )
{
	WNDCLASS	wc ;

	wc.style         = NULL ;
	wc.lpfnWndProc   = (WNDPROC) TimeEditWndProc ;
	wc.cbClsExtra    = 0 ;
	wc.cbWndExtra    = 4 ;
	wc.hInstance     = hInst ;
	wc.hIcon         = NULL ;
	wc.hCursor       = NULL ;
	wc.hbrBackground = (HBRUSH) GetStockObject( WHITE_BRUSH ) ;
	wc.lpszMenuName  = 0 ;
	wc.lpszClassName = CTRL_TIMEEDIT ;
	return ::RegisterClass( &wc ) ;
}

LRESULT CALLBACK
TimeEditWndProc( HWND hWnd,  UINT msg, WPARAM wParam, LPARAM lParam )
{
	PTimeEdit	pWindow ;

	if ( msg == WM_CREATE ) {
		LPCREATESTRUCT	lpStruct = (LPCREATESTRUCT) lParam ;
		pWindow = new TimeEdit() ;
		SetWindowLong( hWnd, 0, (LONG) pWindow ) ;
		pWindow->WMCreate( hWnd, lpStruct ) ;
		return 0 ;
	}
	pWindow = (PTimeEdit) GetWindowLong( hWnd, 0 ) ;
	if ( !pWindow ) {
		return DefWindowProc( hWnd, msg, wParam, lParam ) ;
	}
	switch ( msg ) {
	case WM_DESTROY:
		delete pWindow ;
		SetWindowLong( hWnd, 0, (LONG) 0 ) ;
		break ;
	case WM_SETFOCUS:
		pWindow->WMSetFocus() ;
		return DefWindowProc( hWnd, msg, wParam, lParam ) ;
	case WM_KILLFOCUS:
		pWindow->WMKillFocus( (HWND) wParam ) ;
		return DefWindowProc( hWnd, msg, wParam, lParam ) ;
	case WM_COMMAND:
		return pWindow->WMCommand( hWnd, msg, wParam, lParam ) ;
	case WM_PAINT:			pWindow->WMPaint() ;							break ;
	case WM_CHAR:			pWindow->WMChar( (TCHAR) wParam, lParam ) ;		break ;
	case WM_KEYDOWN:		pWindow->WMKeyDown( (int) wParam, lParam ) ;	break ;
	case WM_LBUTTONDOWN:	pWindow->WMMouseDown( wParam, lParam ) ;		break ;
	case WM_GETDLGCODE:		return DLGC_WANTARROWS|DLGC_WANTCHARS ;

	case TEM_GETTIMEVAL:	return pWindow->GetTimeVal() ;
	case TEM_SETTIMEVAL:	pWindow->SetTimeVal( (DWORD) wParam ) ;			break ;
	case TEM_GETMODIFY:		return pWindow->GetModify() ;
	case TEM_SETMODIFY:		pWindow->SetModify( (BOOL) wParam ) ;			break ;
	default:
		return DefWindowProc( hWnd, msg, wParam, lParam ) ;
	}
	return 0 ;
}
