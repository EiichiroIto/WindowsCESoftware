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

class BigList {
protected:
	HWND	m_hWnd ;
	HWND	m_hWndParent ;	/* �R���g���[���̐e�E�B���h�E */
	WORD	m_idCtrl ;		/* �R���g���[���̎��ʔԍ� */
	DWORD	m_ch ;			/* ���p�����̍��� */
	DWORD	m_dLine ;		/* �\���ł���s�� */
	BOOL	m_fFocus ;		/* �t�H�[�J�X�������Ă���ꍇ*�^* */
	BOOL	m_fPush ;		/* �{�^�����������̏ꍇ*�^* */

	int		(*m_pProc)( int ln, const BYTE **buf, int *len ) ;
	int		(*m_pSpaceProc)( int ln ) ;
	DWORD	m_dMax ;		/* ���X�g�̌� */
	DWORD	m_dTop ;		/* �\�����̐擪�s */
	DWORD	m_dCur ;		/* �J�[�\���s */
	DWORD	m_dLast ;		/* ���O�̃J�[�\���s */

	DWORD	m_dCurY ;
	DWORD	m_dCurH ;

	BOOL	MouseSub( LPARAM lParam, int *dir ) ;

public:
	BigList() ;
	~BigList() ;

	BOOL	Create() { return FALSE ; }

	void	WMCreate( HWND hWnd, LPCREATESTRUCT lpcs ) ;
	void	WMSetFocus() ;
	void	WMKillFocus( HWND hWnd ) ;
	void	WMMouseDown( WPARAM wParam, LPARAM lParam ) ;
	void	WMMouseMove( WPARAM wParam, LPARAM lParam ) ;
	void	WMMouseUp( WPARAM wParam, LPARAM lParam ) ;
	void	WMPaint() ;
	void	WMChar( TCHAR c, LONG keydata ) ;
	BOOL	WMSysChar( TCHAR c, LONG keydata ) ;
	void	WMKeyDown( int nVirtKey, LONG lKeyData ) ;
	LRESULT	WMCommand( HWND hWnd, UINT msg, WPARAM wp, LPARAM lp ) ;
	void	WMVScroll( WPARAM wParam, LPARAM lParam ) ;

	void	SetScrollPos() const ;
	void	DrawLines( HDC hDC, DWORD from, DWORD to ) const ;
	void	ChangeTop( DWORD oldTop, DWORD newTop ) ;
	void	ShowCursor( HDC hDC ) ;
	void	HideCursor( HDC hDC ) ;
	void	ShowCursor() ;
	void	HideCursor() ;
	void	KeyDownSub( int nVirtKey, BOOL fShift, BOOL fControl ) ;
	inline	void	KeyEnter() const ;
	inline	void	KeyESC() const ;
	inline	void	KeyTAB( BOOL fShift ) const ;
	void	KeyHome() ;
	void	KeyEnd() ;
	void	KeyUp() ;
	void	KeyDown() ;
	void	KeyPrior() ;
	void	KeyNext() ;
	void	EnsureFocusVisible() ;
	void	SetProc( int (*proc)( int ln, const BYTE **buf, int *len ) ) ;
	void	SetSpaceProc( int (*proc)( int ln ) ) ;
	DWORD	GetCurSel() const ;
	void	SetCurSel( DWORD index ) ;

	friend	LRESULT CALLBACK	BigListWndProc( HWND hWnd,  UINT msg, WPARAM wParam, LPARAM lParam ) ;
} ;
typedef	BigList	*PBigList ;

BigList::BigList()
{
	m_ch = GetFontH() ;
	m_pProc = 0 ;
	m_pSpaceProc = 0 ;
	m_dMax = m_dTop = m_dCur = 0 ;
	m_dLast = (DWORD) -1 ;
	m_dLine = 0 ;
	m_fFocus = FALSE ;
	m_fPush = FALSE ;
}

BigList::~BigList()
{
}

void
BigList::WMCreate( HWND hWnd, LPCREATESTRUCT lpcs )
{
	RECT	rect ;

	m_hWnd = hWnd ;
	m_hWndParent = lpcs->hwndParent ;
	m_idCtrl = (WORD) lpcs->hMenu ;
	/* ��x�ɕ\���ł���s�������߂� */
	GetClientRect( m_hWnd, &rect ) ;
	m_dLine = rect.bottom - rect.top + 1 - 2 ;
	m_dLine /= m_ch ;
}

void
BigList::WMSetFocus()
{
	m_fFocus = TRUE ;
	HideCursor() ;
	ShowCursor() ;
}

void
BigList::WMKillFocus( HWND hWnd )
{
	m_fFocus = FALSE ;
	HideCursor() ;
	ShowCursor() ;
}

BOOL
BigList::MouseSub( LPARAM lParam, int *dir )
{
	RECT	rect ;
	DWORD	newCur = 0 ;
	WORD	xPos = LOWORD( lParam ) ;
	short	yPos = (short) HIWORD( lParam ) ;

	if ( dir ) {
		*dir = 0 ;
	}
	if ( GetFocus() != m_hWnd ) {
		SetFocus( m_hWnd ) ;
	}
	GetClientRect( m_hWnd, &rect ) ;
	if ( xPos >= rect.right ) {
		return FALSE ;
	} else if ( yPos < 1 ) {
		if ( dir ) {
			*dir = -1 ;
		}
		return FALSE ;
	}
	newCur = (yPos - 1) / m_ch + m_dTop ;
	if ( newCur < m_dMax && newCur < m_dTop + m_dLine ) {
		m_dCur = newCur ;
		EnsureFocusVisible() ;
		return TRUE ;
	} else {
		if ( dir ) {
			*dir = 1 ;
		}
	}
	return FALSE ;
}

void
BigList::WMMouseDown( WPARAM wParam, LPARAM lParam )
{
	if ( MouseSub( lParam, NULL ) ) {
		m_fPush = TRUE ;
		SetCapture( m_hWnd ) ;
	}
}

void
BigList::WMMouseMove( WPARAM wParam, LPARAM lParam )
{
	if ( m_fPush ) {
		int		dir ;
		if ( !MouseSub( lParam, &dir ) && dir ) {
			if ( dir < 0 ) {
				KeyUp() ;
			} else {
				KeyDown() ;
			}
			EnsureFocusVisible() ;
		}
	}
}

void
BigList::WMMouseUp( WPARAM wParam, LPARAM lParam )
{
	if ( m_fPush ) {
		m_fPush = FALSE ;
		if ( MouseSub( lParam, NULL ) ) {
			KeyEnter() ;
		}
	}
	ReleaseCapture() ;
}

void
BigList::WMPaint()
{
	PAINTSTRUCT	ps ;
	HDC			hDC ;
	int			len ;
	DWORD		i, h ;
	RECT		rect ;
	const BYTE	*ptr ;
	CHAR		sjis[ 128 ] ;
	TCHAR		unicode[ 128 ] ;

	hDC = ::BeginPaint( m_hWnd, &ps ) ;
	GetClientRect( m_hWnd, &rect ) ;
	m_dCurY = m_dCurH = 0 ;
	if ( m_pProc ) {
		rect.top += 1 ;		rect.left += 1 ;	rect.right -= 1 ;
		h = rect.bottom - rect.top + 1 ;
		for ( i = m_dTop ; ; i ++ ) {
			if ( (*m_pProc)( (int) i, &ptr, &len ) == -1 ) {
				/* ���X�g�͈̔͊O�ɂȂ����ꍇ */
				break ;
			} else if ( h < m_ch ) {
				/* ��ʂɓ��肫��Ȃ��ꍇ */
				break ;
			}
			h -= m_ch ;
			/* �K�v�ȕ����������o����Unicode�ϊ����� */
			memcpy( sjis, ptr, len ), sjis[ len ] = 0 ;
			sjis2unicode( (LPBYTE) sjis, unicode, sizeof unicode ) ;
			rect.bottom = rect.top + m_ch - 1 ;
			/* �s�\�����s�� */
			KDrawText( hDC, unicode, -1, &rect, 0 ) ;
			if ( i == m_dCur ) {
				m_dCurH = m_fFocus ? m_ch : 2 ;
				m_dCurY = rect.top + (m_ch - m_dCurH) ;
				/* �J�[�\���ʒu�������ꍇ */
				PatBlt( hDC, 0, m_dCurY, rect.right + 1, m_dCurH, DSTINVERT ) ;
			}
			rect.top += m_ch ;
		}
	}
	::EndPaint( m_hWnd, &ps ) ;
}

void
BigList::WMChar( TCHAR chCharCode, LONG lKeyData )
{
	if ( chCharCode == TEXT(' ') && m_pSpaceProc ) {
		if ( (*m_pSpaceProc)( m_dCur ) ) {
			::InvalidateRect( m_hWnd, NULL, TRUE ) ;
		}
	}
}

BOOL
BigList::WMSysChar( TCHAR chCharCode, LONG lKeyData )
{
	return FALSE ;
}

void
BigList::KeyDownSub( int nVirtKey, BOOL fShift, BOOL fControl )
{
	switch ( nVirtKey ) {
	case VK_RETURN:		KeyEnter() ;		break ;
	case VK_ESCAPE:		KeyESC() ;			break ;
	case VK_TAB:		KeyTAB( fShift ) ;	break ;
	case VK_HOME:		KeyHome() ;			break ;
	case VK_END:		KeyEnd() ;			break ;
	case VK_UP:			KeyUp() ;			break ;
	case VK_DOWN:		KeyDown() ;			break ;
	case VK_LEFT:
	case VK_PRIOR:		KeyPrior() ;		break ;
	case VK_RIGHT:
	case VK_NEXT:		KeyNext() ;			break ;
	default:								return ;
	}
	EnsureFocusVisible() ;
}

void
BigList::WMKeyDown( int nVirtKey, LONG lKeyData )
{
	BOOL	fShift   = 0x80 & GetKeyState( VK_SHIFT ) ;
	BOOL	fControl = 0x80 & GetKeyState( VK_CONTROL ) ;

	KeyDownSub( nVirtKey, fShift, fControl ) ;
}

LRESULT
BigList::WMCommand( HWND hWnd, UINT msg, WPARAM wp, LPARAM lp )
{
	return 0 ;
}

void
BigList::WMVScroll( WPARAM wParam, LPARAM lParam )
{
	int		nScrollCode = (int) LOWORD(wParam) ;
	int		nPos = (short int) HIWORD(wParam) ;
	DWORD	newTop = m_dTop ;

	switch ( nScrollCode ) {
	case SB_TOP:
		newTop = 0 ;
		break ;
	case SB_BOTTOM:
		newTop = m_dMax >= m_dLine ? m_dMax - m_dLine : 0 ;
		break ;
	case SB_LINEDOWN:
		if ( m_dTop + m_dLine >= m_dMax ) {
			return ;
		} else {
			newTop = m_dTop + 1 ;
		}
		break ;
	case SB_LINEUP:
		if ( m_dTop ) {
			newTop = m_dTop - 1 ;
		} else {
			return ;
		}
		break ;
	case SB_PAGEDOWN:
		newTop = m_dTop + m_dLine * 2 - 1 >= m_dMax ? m_dMax - m_dLine : m_dTop + m_dLine - 1 ;
		break ;
	case SB_PAGEUP:
		newTop = m_dTop < m_dLine - 1 ? 0 : m_dTop - m_dLine + 1 ;
		break ;
	case SB_THUMBPOSITION:
		if ( nPos < (int) m_dMax ) {
			newTop = nPos ;
		} else {
			return ;
		}
		break ;
	default:
		return ;
	}
	ChangeTop( m_dTop, newTop ) ;
}

/*
 * �����X�N���[���o�[����
 */
void
BigList::SetScrollPos() const
{
	SCROLLINFO	si ;

	memset( &si, 0, sizeof si ) ;
	si.cbSize = sizeof si ;
	si.fMask = SIF_POS|SIF_RANGE ;
	si.nMin = 0 ;
	si.nMax = m_dMax >= m_dLine ? m_dMax - m_dLine : 0 ;
	si.nPos = m_dTop ;
	SetScrollInfo( m_hWnd, SB_VERT, &si, TRUE ) ;
}

/*
 * �w�肳�ꂽ�͈͂̍s��`��
 */
void
BigList::DrawLines( HDC hDC, DWORD from, DWORD to ) const
{
	DWORD		i ;
	int			len ;
	RECT		rect ;
	const BYTE	*ptr ;
	CHAR		sjis[ 128 ] ;
	TCHAR		unicode[ 128 ] ;

	GetClientRect( m_hWnd, &rect ) ;
	rect.top = from * m_ch + 1 ;
	for ( i = from ; i <= to ; i ++ ) {
		if ( (*m_pProc)( (int) (i + m_dTop), &ptr, &len ) == -1 ) {
			/* ���X�g�͈̔͊O�ɂȂ����ꍇ */
			break ;
		}
		/* �K�v�ȕ����������o����Unicode�ϊ����� */
		memcpy( sjis, ptr, len ), sjis[ len ] = 0 ;
		sjis2unicode( (LPBYTE) sjis, unicode, sizeof unicode ) ;
		rect.bottom = rect.top + m_ch ;
		FillRect( hDC, &rect, (HBRUSH) GetStockObject( WHITE_BRUSH ) ) ;
		rect.bottom = rect.top + m_ch - 1 ;
		/* �s�\�����s�� */
		KDrawText( hDC, unicode, -1, &rect, 0 ) ;
		rect.top += m_ch ;
	}
	for ( ; i <= to ; i ++ ) {
		rect.bottom = rect.top + m_ch ;
		FillRect( hDC, &rect, (HBRUSH) GetStockObject( WHITE_BRUSH ) ) ;
	}
}

/*
 * �擪�s��ύX���鏈�����s��
 */
void
BigList::ChangeTop( DWORD oldTop, DWORD newTop )
{
	HDC		hDC ;
	RECT	clip, scroll ;

	m_dTop = newTop ;
	::GetClientRect( m_hWnd, &clip ) ;
	clip.top ++ ;
	clip.bottom = clip.top + m_ch * m_dLine ; //- 1 ;
	scroll = clip ;
	if ( oldTop == newTop ) {
		return ;
	} else if ( oldTop + 1 == newTop ) {
		/* ��X�N���[���̏ꍇ */
		hDC = ::GetDC( m_hWnd ) ;
		HideCursor( hDC ) ;
		scroll.top += m_ch ;
		::ScrollWindowEx( m_hWnd, 0, - (int) m_ch, &scroll, &clip, NULL, NULL, 0 ) ;
		DrawLines( hDC, m_dLine - 1, m_dLine - 1 ) ;
		ShowCursor( hDC ) ;
		::ReleaseDC( m_hWnd, hDC ) ;
	} else if ( oldTop == newTop + 1 ) {
		/* ���X�N���[���̏ꍇ */
		hDC = ::GetDC( m_hWnd ) ;
		HideCursor( hDC ) ;
		scroll.bottom -= m_ch ;
		::ScrollWindowEx( m_hWnd, 0, m_ch, &scroll, &clip, NULL, NULL, 0 ) ;
		DrawLines( hDC, 0, 0 ) ;
		ShowCursor( hDC ) ;
		::ReleaseDC( m_hWnd, hDC ) ;
	} else {
		/* �S�̂�`�悵�Ȃ����ꍇ */
		::InvalidateRect( m_hWnd, NULL, TRUE ) ;
	}
	SetScrollPos() ;
}

/*
 * �J�[�\���s��`��
 */
void
BigList::ShowCursor( HDC hDC )
{
	RECT	rect ;

	if ( m_dCur >= m_dTop && m_dCur <= m_dTop + m_dLine - 1 ) {
		GetClientRect( m_hWnd, &rect ) ;
		m_dCurH = m_fFocus ? m_ch : 2 ;
		m_dCurY = (m_dCur - m_dTop + 1) * m_ch - m_dCurH + 1 ;
		PatBlt( hDC, 0, m_dCurY, rect.right, m_dCurH, DSTINVERT ) ;
	} else {
		m_dCurY = m_dCurH = 0 ;
	}
}

/*
 * �J�[�\���s������
 */
void
BigList::HideCursor( HDC hDC )
{
	RECT	rect ;

	if ( !m_dCurH ) {
		return ;
	}
	GetClientRect( m_hWnd, &rect ) ;
	PatBlt( hDC, 0, m_dCurY, rect.right, m_dCurH, DSTINVERT ) ;
	m_dCurY = m_dCurH = 0 ;
}

/*
 * �J�[�\���s��`��
 */
void
BigList::ShowCursor()
{
	HDC		hDC = GetDC( m_hWnd ) ;
	ShowCursor( hDC ) ;
	ReleaseDC( m_hWnd, hDC ) ;
}

/*
 * �J�[�\���s������
 */
void
BigList::HideCursor()
{
	HDC		hDC = GetDC( m_hWnd ) ;
	HideCursor( hDC ) ;
	ReleaseDC( m_hWnd, hDC ) ;
}

/*
 * Enter�L�[�������ꂽ�ꍇ
 */
void
BigList::KeyEnter() const
{
	SendMessage( m_hWndParent, WM_COMMAND, MAKEWPARAM( m_idCtrl, BLN_ENTER ), (LPARAM) m_hWnd ) ;
}

/*
 * ESC�L�[�������ꂽ�ꍇ
 */
void
BigList::KeyESC() const
{
	SendMessage( m_hWndParent, WM_COMMAND, MAKEWPARAM( m_idCtrl, BLN_ESC ), (LPARAM) m_hWnd ) ;
}

/*
 * TAB�L�[�������ꂽ�ꍇ
 */
void
BigList::KeyTAB( BOOL fShift ) const
{
	WORD	cmd = fShift ? BLN_STAB : BLN_TAB ;
	SendMessage( m_hWndParent, WM_COMMAND, MAKEWPARAM( m_idCtrl, cmd ), (LPARAM) m_hWnd ) ;
}

/*
 * ���X�g�̐擪�Ɉړ�����
 */
void
BigList::KeyHome()
{
	m_dCur = 0 ;
}

/*
 * ���X�g�̍Ō�Ɉړ�����
 */
void
BigList::KeyEnd()
{
	if ( !m_pProc ) {
		return ;
	}
	m_dCur = m_dMax ? m_dMax - 1 : 0 ;
}

/*
 * ��s��Ɉړ�����
 */
void
BigList::KeyUp()
{
	if ( m_dCur ) {
		m_dCur -- ;
	}
}

/*
 * ��s���Ɉړ�����
 */
void
BigList::KeyDown()
{
	if ( m_dCur + 1 < m_dMax ) {
		m_dCur ++ ;
	}
}

/*
 * ��y�[�W�O�Ɉړ�����
 */
void
BigList::KeyPrior()
{
	if ( m_dCur >= m_dLine - 1 ) {
		m_dCur -= m_dLine - 1 ;
	} else {
		m_dCur = 0 ;
	}
}

/*
 * ��y�[�W��Ɉړ�����
 */
void
BigList::KeyNext()
{
	if ( m_dCur + m_dLine - 1 < m_dMax ) {
		m_dCur += m_dLine - 1 ;
	} else {
		m_dCur = m_dMax - 1 ;
	}
}

/*
 * �J�[�\���s����ʂɌ����悤�ɂ���
 */
void
BigList::EnsureFocusVisible()
{
	BOOL	fShow = FALSE ;
	BOOL	fCursor = FALSE ;
	DWORD	line, oldTop = m_dTop ;

	if ( m_dLast != m_dCur ) {
		HideCursor() ;
		fCursor = TRUE ;
		PostMessage( m_hWndParent, WM_COMMAND, MAKEWPARAM( m_idCtrl, BLN_CHANGE ), (LPARAM) m_hWnd ) ;
	}
	if ( m_dCur < m_dTop || m_dCur > m_dTop + m_dLine - 1 ) {
		/* �\���͈͂𒴂��Ă���ꍇ */
		if ( m_dLast < m_dTop || m_dLast > m_dTop + m_dLine - 1 ) {
			/* �O��̈ʒu���\���͈͂𒴂��Ă����ꍇ */
			line = m_dLine / 2 ;
		} else {
			/* ��ʏ�̍s�ʒu�����߂� */
			line = m_dLast - m_dTop ;
		}
		if ( m_dCur < line ) {
			/* ���X�g�̐擪�ɂ���ꍇ */
			m_dTop = 0 ;
		} else {
			m_dTop = m_dCur - line ;
		}
		if ( m_dTop + m_dLine >= m_dMax ) {
			m_dTop = m_dMax >= m_dLine ? m_dMax - m_dLine : 0 ;
		}
		fShow = TRUE ;
	}
	if ( fShow ) {
		ChangeTop( oldTop, m_dTop ) ;
		fCursor = FALSE ;
	}
	if ( fCursor ) {
		ShowCursor() ;
	}
	m_dLast = m_dCur ;
}

/*
 * ���ڎ擾�p�v���V�[�W����ݒ肷��
 */
void
BigList::SetProc( int (*proc)( int ln, const BYTE **buf, int *len ) )
{
	m_pProc = proc ;
	m_dMax = m_pProc ? (DWORD) (*m_pProc)( -1, 0, 0 ) : 0 ;
	m_dTop = m_dCur = 0 ;
	m_dLast = (DWORD) -1 ;
	::InvalidateRect( m_hWnd, NULL, TRUE ) ;
	SetScrollPos() ;
}

/*
 * �X�y�[�X�����p�v���V�[�W����ݒ肷��
 */
void
BigList::SetSpaceProc( int (*proc)( int ln ) )
{
	m_pSpaceProc = proc ;
}

/*
 * �J�[�\���s�𓾂�
 */
DWORD
BigList::GetCurSel() const
{
	return m_dCur ;
}

/*
 * �J�[�\���s��ݒ肷��
 */
void
BigList::SetCurSel( DWORD index )
{
	if ( index >= m_dMax ) {
		return ;
	}
	m_dCur = index ;
	EnsureFocusVisible() ;
}

BOOL
BigListRegisterClass( HINSTANCE hInst )
{
	WNDCLASS	wc ;

	wc.style         = NULL ;
	wc.lpfnWndProc   = (WNDPROC) BigListWndProc ;
	wc.cbClsExtra    = 0 ;
	wc.cbWndExtra    = 4 ;
	wc.hInstance     = hInst ;
	wc.hIcon         = NULL ;
	wc.hCursor       = NULL ;
	wc.hbrBackground = (HBRUSH) GetStockObject( WHITE_BRUSH ) ;
	wc.lpszMenuName  = 0 ;
	wc.lpszClassName = CTRL_BIGLIST ;
	return ::RegisterClass( &wc ) ;
}

LRESULT CALLBACK
BigListWndProc( HWND hWnd,  UINT msg, WPARAM wParam, LPARAM lParam )
{
	PBigList	pWindow ;

	if ( msg == WM_CREATE ) {
		LPCREATESTRUCT	lpStruct = (LPCREATESTRUCT) lParam ;
		pWindow = new BigList() ;
		SetWindowLong( hWnd, 0, (LONG) pWindow ) ;
		pWindow->WMCreate( hWnd, lpStruct ) ;
		return 0 ;
	}
	pWindow = (PBigList) GetWindowLong( hWnd, 0 ) ;
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
	case WM_PAINT:			pWindow->WMPaint() ;		break ;
	case WM_CHAR:			pWindow->WMChar( (TCHAR) wParam, lParam ) ;		break ;
	case WM_SYSCHAR:
		if ( !pWindow->WMSysChar( (TCHAR) wParam, lParam ) ) {
			DefWindowProc( hWnd, msg, wParam, lParam ) ;
		}
		break ;
	case WM_KEYDOWN:
		pWindow->WMKeyDown( (int) wParam, lParam ) ;
		break ;
	case WM_LBUTTONDOWN:	pWindow->WMMouseDown( wParam, lParam ) ;		break ;
	case WM_LBUTTONUP:		pWindow->WMMouseUp( wParam, lParam ) ;			break ;
	case WM_MOUSEMOVE:		pWindow->WMMouseMove( wParam, lParam ) ;		break ;
	case WM_VSCROLL:		pWindow->WMVScroll( wParam, lParam ) ;			break ;

//	case WM_GETDLGCODE:		return DLGC_WANTARROWS|DLGC_WANTTAB ;
	case WM_GETDLGCODE:		return DLGC_WANTALLKEYS ;
	case BLM_SETPROC:		pWindow->SetProc( (int(*)(int,const BYTE**, int*)) lParam ) ;	break ;
	case BLM_GETCURSEL:		return pWindow->GetCurSel() ;
	case BLM_SETCURSEL:		pWindow->SetCurSel( (DWORD) wParam ) ;			break ;
	case BLM_SETSPACEPROC:	pWindow->SetSpaceProc( (int(*)(int)) lParam ) ;	break ;
	default:
		return DefWindowProc( hWnd, msg, wParam, lParam ) ;
	}
	return 0 ;
}
