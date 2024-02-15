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

class LineEdit {
protected:
	HWND	m_hWnd ;
	HWND	m_hWndParent ;		/* コントロールの親ウィンドウ */
	WORD	m_idCtrl ;			/* コントロールの識別番号 */
	TCHAR	m_sText[ 128 ] ;
	DWORD	m_cw, m_ch ;		/* 半角文字の幅と高さ */
	BOOL	m_fModify ;			/* 変更が加えられた場合*真* */
	BOOL	m_fSelect ;			/* 選択中の場合*真* */
	BOOL	m_fOnFocus ;		/* フォーカスがある場合 */
	DWORD	m_dMaxWidth ;		/* 入力可能な領域の幅 */

public:
	LineEdit() ;
	~LineEdit() ;

	BOOL	Create() { return FALSE ; }

	void	WMCreate( HWND hWnd, LPCREATESTRUCT lpcs ) ;
	void	WMSetFocus() ;
	void	WMKillFocus( HWND hWnd ) ;
	void	WMMouseDown( WPARAM wParam, LPARAM lParam ) ;
	void	WMPaint() ;
	void	WMChar( TCHAR c, LONG keydata ) ;
	void	WMKeyDown( int nVirtKey, LONG lKeyData ) ;
	LRESULT	WMCommand( HWND hWnd, UINT msg, WPARAM wp, LPARAM lp ) ;

	BOOL	ClearSelect() ;
	void	KeyDownSub( int nVirtKey, BOOL fShift, BOOL fControl ) ;
	inline	void	KeyBS() ;
	inline	void	KeyESC() ;
	inline	void	KeyEnter() const ;
	inline	void	KeyTAB( BOOL fShift ) const ;
	inline	void	KeyRight() ;
	DWORD	GetTextWidth() const ;
	void	GetText( LPTSTR buf, DWORD size ) const ;
	void	SetText( LPCTSTR buf ) ;
	void	GetSjisText( LPSTR buf, DWORD size ) const ;
	void	SetSjisText( LPCSTR buf ) ;
	BOOL	GetModify() const { return m_fModify ; }
	void	SetModify( BOOL fModify ) { m_fModify = TRUE ; }

	friend	LRESULT CALLBACK	LineEditWndProc( HWND hWnd,  UINT msg, WPARAM wParam, LPARAM lParam ) ;
} ;
typedef	LineEdit	*PLineEdit ;

LineEdit::LineEdit()
{
	m_sText[0] = 0 ;
	m_cw = GetFontHW() ;
	m_ch = GetFontH() ;
	m_fModify = TRUE ;
	m_fSelect = FALSE ;
	m_fOnFocus = FALSE ;
	m_dMaxWidth = 0 ;
}

LineEdit::~LineEdit()
{
}

void
LineEdit::WMCreate( HWND hWnd, LPCREATESTRUCT lpcs )
{
	RECT	rect ;

	m_hWnd = hWnd ;
	m_hWndParent = lpcs->hwndParent ;
	m_idCtrl = (WORD) lpcs->hMenu ;
	GetClientRect( m_hWnd, &rect ) ;
	m_dMaxWidth = rect.right - rect.left - 2 ;
}

void
LineEdit::WMSetFocus()
{
	m_fOnFocus = TRUE ;
	::CreateCaret( m_hWnd, NULL, 2, m_ch ) ;
	m_fSelect = m_sText[0] != 0 ;
	if ( !m_fSelect ) {
		::ShowCaret( m_hWnd ) ;
		::SetCaretPos( GetTextWidth(), 1 ) ;
	}
	::InvalidateRect( m_hWnd, NULL, TRUE ) ;
}

void
LineEdit::WMKillFocus( HWND hWnd )
{
	m_fOnFocus = FALSE ;
	if ( m_fSelect ) {
		::InvalidateRect( m_hWnd, NULL, TRUE ) ;
	} else {
		::HideCaret( m_hWnd ) ;
	}
	::DestroyCaret() ;
	PostMessage( m_hWndParent, WM_COMMAND, MAKEWPARAM( m_idCtrl, RBN_KILLFOCUS ), (LPARAM) m_hWnd ) ;
}

void
LineEdit::WMMouseDown( WPARAM wParam, LPARAM lParam )
{
	if ( GetFocus() != m_hWnd ) {
		SetFocus( m_hWnd ) ;
	}
}

void
LineEdit::WMPaint()
{
	PAINTSTRUCT	ps ;
	HDC			hDC ;
	RECT		rect ;

	hDC = ::BeginPaint( m_hWnd, &ps ) ;
	GetClientRect( m_hWnd, &rect ) ;
	rect.top += 1 ;		rect.left += 1 ;
	rect.bottom -= 1 ;	rect.right -= 1 ;
	KDrawText( hDC, m_sText, -1, &rect, 0 ) ;
	if ( m_fOnFocus ) {
		DWORD	width = GetTextWidth() ;
		if ( m_fSelect ) {
			PatBlt( hDC, rect.left, rect.top, width, m_ch, DSTINVERT ) ;
		} else {
			::SetCaretPos( width, 1 ) ;
		}
	}
	::EndPaint( m_hWnd, &ps ) ;
}

void
LineEdit::WMChar( TCHAR chCharCode, LONG lKeyData )
{
	DWORD	len ;

	if ( (chCharCode >= TEXT(' ')) ) {
		if ( m_fSelect ) {
			ClearSelect() ;
			len = 0 ;
		} else {
			len = _tcslen( m_sText ) ;
		}
		m_sText[ len ] = chCharCode ;
		m_sText[ len + 1 ] = 0 ;
		if ( GetTextWidth() > m_dMaxWidth ) {
			m_sText[ len ] = 0 ;
			return ;
		}
		m_fModify = TRUE ;
		::InvalidateRect( m_hWnd, NULL, TRUE ) ;
		PostMessage( m_hWndParent, WM_COMMAND, MAKEWPARAM( m_idCtrl, RBN_CHANGE ), (LPARAM) m_hWnd ) ;
	}
}

void
LineEdit::WMKeyDown( int nVirtKey, LONG lKeyData )
{
	BOOL	fShift   = 0x80 & GetKeyState( VK_SHIFT ) ;
	BOOL	fControl = 0x80 & GetKeyState( VK_CONTROL ) ;

	KeyDownSub( nVirtKey, fShift, fControl ) ;
}

LRESULT
LineEdit::WMCommand( HWND hWnd, UINT msg, WPARAM wp, LPARAM lp )
{
	return 0 ;
}

BOOL
LineEdit::ClearSelect()
{
	if ( m_fSelect ) {
		m_fSelect = FALSE ;
		::ShowCaret( m_hWnd ) ;
		::SetCaretPos( GetTextWidth(), 1 ) ;
		return TRUE ;
	}
	return FALSE ;
}

void
LineEdit::KeyDownSub( int nVirtKey, BOOL fShift, BOOL fControl )
{
	switch ( nVirtKey ) {
	case VK_BACK:		KeyBS() ;			break ;
	case VK_ESCAPE:		KeyESC() ;			break ;
	case VK_RETURN:		KeyEnter() ;		break ;
	case VK_TAB:		KeyTAB( fShift ) ;	break ;
	case VK_DELETE:							break ;
	case VK_RIGHT:
		KeyRight() ;
		//
	case VK_HOME:
	case VK_END:
	case VK_LEFT:
	case VK_UP:
	case VK_DOWN:
	case VK_PRIOR:
	case VK_NEXT:
		SendMessage( m_hWndParent, WM_COMMAND, MAKEWPARAM( m_idCtrl, RBN_KEYDOWN ), (LPARAM) nVirtKey ) ;
		break ;
	}
}

void
LineEdit::KeyBS()
{
	DWORD	len ;

	len = _tcslen( m_sText ) ;
	if ( !len ) {
		return ;
	}
	if ( m_fSelect ) {
		ClearSelect() ;
		m_sText[ 0 ] = 0 ;
	} else {
		m_sText[ len - 1 ] = 0 ;
	}
	m_fModify = TRUE ;
	::InvalidateRect( m_hWnd, NULL, TRUE ) ;
	PostMessage( m_hWndParent, WM_COMMAND, MAKEWPARAM( m_idCtrl, RBN_CHANGE ), (LPARAM) m_hWnd ) ;
}

void
LineEdit::KeyESC()
{
	if ( m_fSelect ) {
		ClearSelect() ;
	} else {
		m_sText[0] = 0 ;
	}
	m_fModify = TRUE ;
	::InvalidateRect( m_hWnd, NULL, TRUE ) ;
	PostMessage( m_hWndParent, WM_COMMAND, MAKEWPARAM( m_idCtrl, RBN_CHANGE ), (LPARAM) m_hWnd ) ;
}

void
LineEdit::KeyEnter() const
{
	SendMessage( m_hWndParent, WM_COMMAND, MAKEWPARAM( m_idCtrl, RBN_ENTER ), (LPARAM) m_hWnd ) ;
}

void
LineEdit::KeyTAB( BOOL fShift ) const
{
	WORD	cmd = fShift ? RBN_STAB : RBN_TAB ;
	SendMessage( m_hWndParent, WM_COMMAND, MAKEWPARAM( m_idCtrl, cmd ), (LPARAM) m_hWnd ) ;
}

void
LineEdit::KeyRight()
{
	if ( ClearSelect() ) {
		::InvalidateRect( m_hWnd, NULL, TRUE ) ;
	}
}

/*
 * 入力されている文字列のビット単位での長さを求める
 */
DWORD
LineEdit::GetTextWidth() const
{
	TCHAR	c ;
	DWORD	result = 0 ;
	LPCTSTR	ptr = m_sText ;

	while ( c = *ptr++ ) {
		if ( is_hankaku( c ) ) {
			result ++ ;
		} else {
			result += 2 ;
		}
	}
	return result * m_cw ;
}

/*
 * 文字列を取得する
 */
void
LineEdit::GetText( LPTSTR buf, DWORD size ) const
{
	memcpy( buf, m_sText, size * sizeof (TCHAR) ) ;
}

/*
 * 文字列を設定する
 */
void
LineEdit::SetText( LPCTSTR buf )
{
	_tcscpy( m_sText, buf ) ;
	::InvalidateRect( m_hWnd, NULL, TRUE ) ;
}

/*
 * Shift-JIS文字列を取得する
 */
void
LineEdit::GetSjisText( LPSTR buf, DWORD size ) const
{
	unicode2sjis( m_sText, (LPBYTE) buf, size ) ;
}

/*
 * Shift-JIS文字列を設定する
 */
void
LineEdit::SetSjisText( LPCSTR buf )
{
	sjis2unicode( (LPBYTE) buf, m_sText, sizeof m_sText ) ;
	::InvalidateRect( m_hWnd, NULL, TRUE ) ;
}

BOOL
LineEditRegisterClass( HINSTANCE hInst )
{
	WNDCLASS	wc ;

	wc.style         = NULL ;
	wc.lpfnWndProc   = (WNDPROC) LineEditWndProc ;
	wc.cbClsExtra    = 0 ;
	wc.cbWndExtra    = 4 ;
	wc.hInstance     = hInst ;
	wc.hIcon         = NULL ;
	wc.hCursor       = NULL ;
	wc.hbrBackground = (HBRUSH) GetStockObject( WHITE_BRUSH ) ;
	wc.lpszMenuName  = 0 ;
	wc.lpszClassName = CTRL_LINEEDIT ;
	return ::RegisterClass( &wc ) ;
}

LRESULT CALLBACK
LineEditWndProc( HWND hWnd,  UINT msg, WPARAM wParam, LPARAM lParam )
{
	PLineEdit	pWindow ;

	if ( msg == WM_CREATE ) {
		LPCREATESTRUCT	lpStruct = (LPCREATESTRUCT) lParam ;
		pWindow = new LineEdit() ;
		SetWindowLong( hWnd, 0, (LONG) pWindow ) ;
		pWindow->WMCreate( hWnd, lpStruct ) ;
		return 0 ;
	}
	pWindow = (PLineEdit) GetWindowLong( hWnd, 0 ) ;
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

	case WM_GETTEXT:		pWindow->GetText( (LPTSTR) lParam, (DWORD) wParam ) ;		break ;
	case WM_SETTEXT:		pWindow->SetText( (LPCTSTR) lParam ) ;						break ;
	case RBM_GETSJIS:		pWindow->GetSjisText( (LPSTR) lParam, (DWORD) wParam ) ;	break ;
	case RBM_SETSJIS:		pWindow->SetSjisText( (LPCSTR) lParam ) ;					break ;
	case RBM_GETMODIFY:		return pWindow->GetModify() ;
	case RBM_SETMODIFY:		pWindow->SetModify( (BOOL) wParam ) ;						break ;
	default:
		return DefWindowProc( hWnd, msg, wParam, lParam ) ;
	}
	return 0 ;
}
