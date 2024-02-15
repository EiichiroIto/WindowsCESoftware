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
#include	"resource.h"
#include	"appmain.h"
#include	"lfunc.h"
#include	"base.h"
#include	"graph.h"
#include	"appsub.h"

BOOL	AboutDialogExec( HINSTANCE hInst, HWND hWnd, LPBOOL fDelete ) ;
BOOL	g_bNarrow = FALSE ;

/*-------------------------------------------------------------------------*
 * StationArea
 *-------------------------------------------------------------------------*/
StationArea::StationArea( LINEID *pLineID )
{
	m_pTermLine = pLineID ;
}

/*
 * �E�B���h�E���쐬����
 */
DWORD
StationArea::Create( HINSTANCE hInst, HWND hWndParent, DWORD x, DWORD y, DWORD w, WORD id1, WORD id2 )
{
	DWORD	ch = GetFontH() + 4 ;

	/* �o��/�����w���͗̈���쐬���� */
	m_hWnd = CreateWindowEx( 0,
					CTRL_LINEEDIT,
					TEXT(""),
					WS_CHILD|WS_VISIBLE|WS_BORDER|WS_TABSTOP,
					x, y, w, ch + 4,
					hWndParent,
					(HMENU) id1,
					hInst,
					0 ) ;
	y += ch + 8 ;
	/* �o��/���������p�H���̈���쐬���� */
	m_hWndLine = CreateWindowEx( WS_EX_CLIENTEDGE,
					CTRL_LABEL,
					TEXT(""),
					WS_CHILD|WS_VISIBLE|WS_BORDER,
					x, y, w, ch + 4,
					hWndParent,
					(HMENU) id2,
					hInst,
					0 ) ;
	y += ch + 4 ;
	return y ;
}

/*
 * ���͓��e���m�肷��B
 */
BOOL
StationArea::confirm()
{
	CHAR	roma[ 128 ] ;
	DWORD	sort_index, station_index ;

	/* ���Ɋm��ς݂ŗ̈�ɕύX���������Ă��Ȃ��ꍇ */
	if ( !SendMessage( m_hWnd, RBM_GETMODIFY, 0, 0 ) && m_fValid ) {
		return TRUE ;
	}
	m_fValid = FALSE ;
	/* ���[�}����������擾���� */
	SendMessage( m_hWnd, RBM_GETSJIS, (WPARAM) sizeof roma, (LPARAM) roma ) ;
	/* �w���ɊY�����邩���ׂ� */
	if ( name2stid( (LPBYTE) roma, &station_index ) ) {
		set_station_index( station_index ) ;
		return TRUE ;
	}
	/* ���[�}���Ƃ��ĊY������w�����邩���ׂ� */
	if ( !roma2sortidx( (LPBYTE) roma, &sort_index ) ) {
		return FALSE ;
	}
	/* �\�[�g�C���f�b�N�X����w�����Z�b�g���� */
	set_sort_index( sort_index ) ;
	return TRUE ;
}

/*
 * �w�C���f�b�N�X����w�����Z�b�g����
 */
void
StationArea::set_station_index( DWORD station_index )
{
	const BYTE	*p ;

	/* �w�C���f�b�N�X��ݒ肷�� */
	m_dIndex = station_index ;
	/* �w����\������ */
	p = get_station( m_dIndex )->get_name() ;
	SendMessage( m_hWnd, RBM_SETSJIS, 0, (LPARAM) p ) ;
	/* �ҏW�ς݃t���O�̃N���A */
	SendMessage( m_hWnd, RBM_SETMODIFY, 0, 0 ) ;
	m_fValid = TRUE ;
}

/*
 * �\�[�g�C���f�b�N�X����w�����Z�b�g����
 */
void
StationArea::set_sort_index( DWORD sort_index )
{
	DWORD	station_index ;

	/* �\�[�g�C���f�b�N�X����w�C���f�b�N�X�𓾂� */
	station_index = get_sort_s_id( sort_index ) ;
	/* �w�C���f�b�N�X����w�����Z�b�g���� */
	set_station_index( station_index ) ;
}

void
StationArea::GetSjis( LPSTR sjis, DWORD size ) const
{
	SendMessage( m_hWnd, RBM_GETSJIS, (WPARAM) size, (LPARAM) sjis ) ;
}

/*
 * �o��/���������p�H����ݒ肷��
 */
void
StationArea::set_term_line( LINEID lineID ) const
{
	TCHAR	unicode[ 128 ] ;

	if ( *m_pTermLine == lineID ) {
		/* �ȑO�Ɠ����Ȃ牽�����Ȃ� */
		return ;
	}
	if ( lineID == RID_NONE ) {
		unicode[0] = 0 ;
	} else {
		sjis2unicode( get_rosen( lineID )->get_name(), unicode, sizeof unicode ) ;
	}
	SetWindowText( m_hWndLine, unicode ) ;
	*m_pTermLine = lineID ;
}

/*
 * �o��/�����w�����N���A����
 */
void
StationArea::Clear()
{
	SendMessage( m_hWnd, RBM_SETSJIS, 0, (LPARAM) "" ) ;
	SetWindowText( m_hWndLine, TEXT("") ) ;
	*m_pTermLine = RID_NONE ;
}

/*
 * ���͕����񂪋󂩒��ׂ�
 */
BOOL
StationArea::is_empty() const
{
	CHAR	sjis[ 10 ] ;

	SendMessage( m_hWnd, RBM_GETSJIS, (WPARAM) sizeof sjis, (LPARAM) sjis ) ;
	return !sjis[0] ;
}

/*
 * �K�v�ȍ��������߂�
 */
DWORD
StationArea::GetHeight() const
{
	return 2 * (GetFontH() + 4) + 12 ;
}

/*-------------------------------------------------------------------------*
 * ResultArea
 *-------------------------------------------------------------------------*/
/*
 * �E�B���h�E���쐬����
 */
HWND
ResultArea::Create( HINSTANCE hInst, HWND hWndParent, DWORD x, DWORD y, DWORD w, DWORD h, WORD id )
{
	m_hWnd = CreateWindowEx( 0,
					CTRL_BIGLIST,
					TEXT(""),
					WS_CHILD|WS_VISIBLE|WS_BORDER|WS_TABSTOP|WS_VSCROLL,
					x, y, w, h,
					hWndParent,
					(HMENU) id,
					hInst,
					0 ) ;
	return m_hWnd ;
}

/*
 * ���X�g�v���V�[�W����ݒ肷��
 */
void
ResultArea::SetProc( int (*proc)( int ln, const BYTE **buf, int *len ) )
{
	SendMessage( m_hWnd, BLM_SETPROC, 0, (LPARAM) proc ) ;
}

/*
 * �J�[�\���s���擾����
 */
DWORD
ResultArea::GetCurSel() const
{
	return SendMessage( m_hWnd, BLM_GETCURSEL, 0, 0 ) ;
}

/*
 * �J�[�\���s��ݒ肷��
 */
void
ResultArea::SetCurSel( DWORD index )
{
	SendMessage( m_hWnd, BLM_SETCURSEL, (WPARAM) index, 0 ) ;
}

/*
 * �L�[����
 */
void
ResultArea::SendKey( DWORD nVirtKey ) const
{
	SendMessage( m_hWnd, WM_KEYDOWN, nVirtKey, 0 ) ;
}

/*
 * ���e�̃N���A
 */
void
ResultArea::Clear()
{
	SendMessage( m_hWnd, BLM_SETPROC, 0, (LPARAM) NULL ) ;
	InvalidateRect( m_hWnd, NULL, TRUE ) ;
}

/*-------------------------------------------------------------------------*
 * TimeArea
 *-------------------------------------------------------------------------*/
TimeArea::TimeArea( int *pTime )
{
	m_pTime = pTime ;
}

/*
 * �E�B���h�E���쐬����
 */
DWORD
TimeArea::Create( HINSTANCE hInst, HWND hWndParent, DWORD x, DWORD y, WORD id, WORD bmpID )
{
	HBITMAP	hBmp ;
	HWND	hWndCtrl ;
	DWORD	ch = GetFontH() + 4, w ;

	/* �Ή�����r�b�g�}�b�v��\������ */
	hWndCtrl = CreateWindowEx( 0,
					TEXT("STATIC"),
					TEXT(""),
					WS_CHILD|WS_VISIBLE|SS_BITMAP,
					x, y, 16, 16,
					hWndParent,
					0,
					hInst,
					0 ) ;
	hBmp = LoadBitmap( hInst, MAKEINTRESOURCE(bmpID) ) ;
	SendMessage( hWndCtrl, STM_SETIMAGE,
				(WPARAM)IMAGE_BITMAP,
				(LPARAM)hBmp ) ;
	/* �o��/�����w���͗̈���쐬���� */
	x += 18 ;
	w = GetFontHW() * 5 + 4 ;
	m_hWnd = CreateWindowEx( 0,
					CTRL_LABEL,
					TEXT(""),
					WS_CHILD|WS_VISIBLE|WS_BORDER,
					x, y, w, ch,
					hWndParent,
					(HMENU) id,
					hInst,
					0 ) ;
	y += ch + 2 ;
	return y ;
}

/*
 * �\�����e���X�V����
 */
void
TimeArea::Refresh() const
{
	CHAR	sjis[ 10 ] ;
	TCHAR	unicode[ 10 ] ;

	make_timestr96( *m_pTime, (LPBYTE) &sjis ) ;
	sjis2unicode( (LPBYTE) sjis, unicode, sizeof unicode ) ;
	SetWindowText( m_hWnd, unicode ) ;
}

/*
 * ���e�̃N���A
 */
void
TimeArea::Clear()
{
	SetWindowText( m_hWnd, TEXT("") ) ;
}

/*
 * �K�v�ȍ��������߂�
 */
DWORD
TimeArea::GetHeight() const
{
	return GetFontH() + 4 + 2 ;
}

/*-------------------------------------------------------------------------*
 * KindArea
 *-------------------------------------------------------------------------*/
KindArea::KindArea( int *pFlag, int dMask )
{
	m_pFlag = pFlag ;
	m_dMask = dMask ;
}

/*
 * �E�B���h�E���쐬����
 */
DWORD
KindArea::Create( HINSTANCE hInst, HWND hWndParent, DWORD x, DWORD y, WORD id, LPCSTR text )
{
	TCHAR	unicode[ 20 ] ;
	DWORD	ch = GetFontH() + 4, w ;

	/* ��ʗ̈���쐬���� */
	w = GetFontHW() * 6 + 4 ;
	m_hWnd = CreateWindowEx( 0,
					CTRL_LABEL,
					TEXT(""),
					WS_CHILD|WS_VISIBLE|WS_BORDER,
					x, y, w, ch,
					hWndParent,
					(HMENU) id,
					hInst,
					0 ) ;
	sjis2unicode( (LPBYTE) text, unicode, sizeof unicode ) ;
	SetWindowText( m_hWnd, unicode ) ;
	y += ch + 2 ;
	Refresh() ;
	return y ;
}

/*
 * �\�����e���X�V����
 */
void
KindArea::Refresh() const
{
	BOOL	fOn = (*m_pFlag) & m_dMask ;

	SendMessage( m_hWnd, LAM_SETCROSS, (WPARAM) fOn, 0 ) ;
}

/*
 * ���e�𔽓]����
 */
void
KindArea::Toggle()
{
	*m_pFlag = (*m_pFlag) ^ m_dMask ;
	Refresh() ;
}

/*
 * �K�v�ȍ��������߂�
 */
DWORD
KindArea::GetHeight() const
{
	return GetFontH() + 4 + 2 ;
}

/*-------------------------------------------------------------------------*
 * MaxArea
 *-------------------------------------------------------------------------*/
MaxArea::MaxArea( int *pData, int dMaxValue )
{
	m_pData = pData ;
	m_dMaxValue = dMaxValue ;
}

/*
 * �E�B���h�E���쐬����
 */
DWORD
MaxArea::Create( HINSTANCE hInst, HWND hWndParent, DWORD x, DWORD y, WORD id, WORD BmpID )
{
	HBITMAP	hBmp ;
	HWND	hWndCtrl ;
	DWORD	ch = GetFontH() + 4, w ;

	/* �Ή�����r�b�g�}�b�v��\������ */
	hWndCtrl = CreateWindowEx( 0,
					TEXT("STATIC"),
					TEXT(""),
					WS_CHILD|WS_VISIBLE|SS_BITMAP,
					x, y, 16, 16,
					hWndParent,
					0,
					hInst,
					0 ) ;
	hBmp = LoadBitmap( hInst, MAKEINTRESOURCE(BmpID) ) ;
	SendMessage( hWndCtrl, STM_SETIMAGE,
				(WPARAM)IMAGE_BITMAP,
				(LPARAM)hBmp ) ;
	/* ��ʗ̈���쐬���� */
	x += 18 ;
	w = GetFontHW() * 2 + 4 ;
	m_hWnd = CreateWindowEx( 0,
					CTRL_LINEEDIT,
					TEXT(""),
					WS_CHILD|WS_VISIBLE|WS_BORDER,
					x, y, w, ch,
					hWndParent,
					(HMENU) id,
					hInst,
					0 ) ;
	Refresh() ;
	return y + ch ;
}

/*
 * �\�����e���X�V����
 */
void
MaxArea::Refresh() const
{
	TCHAR	buf[ 10 ] ;
	int		data = *m_pData ;

	if ( data == m_dMaxValue ) {
		buf[0] = 0 ;
	} else {
		wsprintf( buf, TEXT("%d"), data ) ;
	}
	SetWindowText( m_hWnd, buf ) ;
}

/*
 * �f�[�^���X�V����
 */
void
MaxArea::Update() const
{
	TCHAR	buf[ 10 ] ;

	GetWindowText( m_hWnd, buf, 10 ) ;
	if ( buf[0] ) {
		*m_pData = _ttol( buf ) ;
	} else {
		*m_pData = m_dMaxValue ;
	}
}

/*
 * �K�v�ȍ��������߂�
 */
DWORD
MaxArea::GetHeight() const
{
	return GetFontH() + 4 ;
}

/*-------------------------------------------------------------------------*
 * MainWindow
 *-------------------------------------------------------------------------*/
MainWindow::MainWindow( TrainApp *pApp )
{
	m_pApp = pApp ;
	m_hIcon = 0 ;
	m_fDeleteReg = FALSE ;
}

MainWindow::~MainWindow()
{
	if ( m_hIcon ) {
		::DeleteObject( m_hIcon ) ;
	}
}

void
MainWindow::WMCreate( HWND hWnd )
{
	RECT		rect ;
	DWORD		height = 2, tw, th, rw, h, x ;
	DWORD		x1, x2, x3, h1, h2, h3, mw, w ;
	HINSTANCE	hInst = m_pApp->get_inst() ;

	m_hWnd = hWnd ;
	LONG	lStyle = GetWindowLong( hWnd, GWL_STYLE ) ;
#ifdef	_WIN32_WCE
	lStyle &= ~WS_CAPTION ;
#endif	/* _WIN32_WCE */
	SetWindowLong( hWnd, GWL_STYLE, lStyle ) ;
	/* �A�C�R�����쐬���^�X�N�o�[�ɓo�^���� */
	m_hIcon = (HICON) LoadImage( m_pApp->get_inst(), MAKEINTRESOURCE(IDI_APPICON),
								 IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR ) ;
	if ( m_hIcon ) {
		SendMessage( hWnd, WM_SETICON, FALSE, (LPARAM)m_hIcon ) ;
	}
	/* �\���̈�����߂� */
	GetClientRect( m_hWnd, &rect ) ;
	tw = rect.right ;
	th = rect.bottom - rect.top ;
	g_bNarrow = tw < 480 ;
	/* �R�}���h�o�[��ǉ����� */
	height += CreateCommandBar() ;
	if ( tw < 480 ) {
		/* �o���w���͗̈���쐬���� */
		h = height ;
		h = m_pApp->get_dept()->Create( hInst, m_hWnd, 2, h, tw - 4,
										IDC_DEPT, IDC_DEPTLINE ) ;
		/* �����w���͗̈���쐬���� */
		h += 4 ;
		h = m_pApp->get_ariv()->Create( hInst, m_hWnd, 2, h, tw - 4,
										IDC_ARIV, IDC_ARIVLINE ) ;
		/* �ő�l���͗̈�̍����Ɨ��p��ʂ̍��������߂� */
		h1 = m_pApp->get_walk()->GetHeight() + 4 ;
		h1 += m_pApp->get_limited()->GetHeight() + 4 ;
		/* ���ʕ\���̈���쐬���� */
		h += 4 ;
		h2 = th - h - h1 - 2 ;
		m_pApp->get_result()->Create( hInst, m_hWnd, 2, h, tw - 4, h2,
									  IDC_RESULT ) ;
		h += h2 ;
		/* �ő�l���͗̈�̕������߂� */
		w = (tw - 8) / 4 ;
		/* �k�����ԕ\���̈���쐬���� */
		h += 4 ;
		x1 = 2 ;
		m_pApp->get_walk()->Create( hInst, m_hWnd, x1, h,
									IDC_WALKTIME, IDB_WALK_TIME ) ;
		/* ��Ԏ��ԕ\���̈���쐬���� */
		x1 += w ;
		m_pApp->get_boad()->Create( hInst, m_hWnd, x1, h,
									IDC_BOADTIME, IDB_BOAD_TIME ) ;
		/* �҂����ԕ\���̈���쐬���� */
		x1 += w ;
		m_pApp->get_wait()->Create( hInst, m_hWnd, x1, h,
									IDC_WAITTIME, IDB_WAIT_TIME ) ;
		/* ���v���ԕ\���̈���쐬���� */
		x1 += w ;
		h = m_pApp->get_total()->Create( hInst, m_hWnd, x1, h,
										 IDC_TOTALTIME, IDB_TOTAL_TIME ) ;
		/* ���p��ʂ��쐬���� */
		w = tw / 6 ;
		x1 = 0 ;
		/* �L�����} */
		h += 4 ;
		m_pApp->get_limited()->Create( hInst, m_hWnd, x1, h,
									   IDC_LIMITED, " ���} " ) ;
		/* �V���� */
		x1 += w + 2 ;
		m_pApp->get_super()->Create( hInst, m_hWnd, x1, h,
									   IDC_SUPER, "�V����" ) ;
		/* ���̑� */
		x1 += w + 2 ;
		m_pApp->get_other()->Create( hInst, m_hWnd, x1, h,
									   IDC_OTHER, "���̑�" ) ;
		/* �ő�k������ */
		x1 += w + 2 ;
		m_pApp->get_maxwalk()->Create( hInst, m_hWnd, x1, h,
									   IDC_MAXWALK, IDB_WALK_TIME ) ;
		/* �ő�҂����� */
		x1 += w - 2 ;
		m_pApp->get_maxwait()->Create( hInst, m_hWnd, x1, h,
									   IDC_MAXWAIT, IDB_WAIT_TIME ) ;
		/* �ő�抷�� */
		x1 += w - 2 ;
		h = m_pApp->get_maxchange()->Create( hInst, m_hWnd, x1, h,
											 IDC_MAXCHANGE, IDB_CHANGE ) ;
		/* ���ʕ\���̈�̒ǉ��󔒐����[���ɂ��� */
		m_pApp->set_addlen( 0 ) ;
	} else {
		rw = 45 * GetFontHW() + GetSystemMetrics( SM_CXHSCROLL ) + 4 ;
		/* �o���w���͗̈���쐬���� */
		h = m_pApp->get_dept()->Create( hInst, m_hWnd, 2, height, tw - rw - 10,
										IDC_DEPT, IDC_DEPTLINE ) ;
		/* �����w���͗̈���쐬���� */
		h += 6 ;
		h = m_pApp->get_ariv()->Create( hInst, m_hWnd, 2, h, tw - rw - 10,
										IDC_ARIV, IDC_ARIVLINE ) ;
		/* ���ʕ\���̈���쐬���� */
		m_pApp->get_result()->Create( hInst, m_hWnd, tw - rw - 6, height, rw, 180,
									  IDC_RESULT ) ;

		/* �ő�l���͗̈�̕������߂� */
		mw = MaxArea::GetWidth() ;
		w = (tw - rw - mw - 8) / 2 ;
		x1 = 2 ;
		x2 = x1 + w ;
		/* �k�����ԕ\���̈���쐬���� */
		h += 6 ;
		h1 = h ;
		m_pApp->get_walk()->Create( hInst, m_hWnd, x1, h,
									IDC_WALKTIME, IDB_WALK_TIME ) ;
		/* ��Ԏ��ԕ\���̈���쐬���� */
		h = m_pApp->get_boad()->Create( hInst, m_hWnd, x2, h,
										IDC_BOADTIME, IDB_BOAD_TIME ) ;
		/* �҂����ԕ\���̈���쐬���� */
		h += 4 ;
		h2 = h ;
		m_pApp->get_wait()->Create( hInst, m_hWnd, x1, h,
									IDC_WAITTIME, IDB_WAIT_TIME ) ;
		/* ���v���ԕ\���̈���쐬���� */
		h = m_pApp->get_total()->Create( hInst, m_hWnd, x2, h,
										 IDC_TOTALTIME, IDB_TOTAL_TIME ) ;
		/* ���p��ʂ��쐬���� */
		w = (tw - rw - mw - 8) / 3 ;
		x1 = 2 ;
		x2 = x1 + w ;
		x3 = x2 + w ;
		h += 4 ;
		h3 = h ;
		/* �L�����} */
		m_pApp->get_limited()->Create( hInst, m_hWnd, x1, h,
									   IDC_LIMITED, " ���} " ) ;
		/* �V���� */
		m_pApp->get_super()->Create( hInst, m_hWnd, x2, h,
									   IDC_SUPER, "�V����" ) ;
		/* ���̑� */
		m_pApp->get_other()->Create( hInst, m_hWnd, x3, h,
									   IDC_OTHER, "���̑�" ) ;
		/* �ő�l�̈���쐬���� */
		x = tw - rw - mw - 8 ;
		/* �ő�k������ */
		m_pApp->get_maxwalk()->Create( hInst, m_hWnd, x, h1,
									   IDC_MAXWALK, IDB_WALK_TIME ) ;
		/* �ő�҂����� */
		m_pApp->get_maxwait()->Create( hInst, m_hWnd, x, h2,
									   IDC_MAXWAIT, IDB_WAIT_TIME ) ;
		/* �ő�抷�� */
		m_pApp->get_maxchange()->Create( hInst, m_hWnd, x, h3,
										 IDC_MAXCHANGE, IDB_CHANGE ) ;
	}
}

void
MainWindow::WMDestroy()
{
	PostQuitMessage( 0 ) ;
}

LRESULT
MainWindow::WMCommand( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	DWORD	tmp ;
	HWND	hWndFocus = GetFocus() ;
	WORD	idCtrl = LOWORD(wParam) ;
	WORD	notifyCode = HIWORD(wParam) ;
	BOOL	fDept = FALSE, fDelete = FALSE ;
	StationArea	*st = NULL ;
	StationArea	*pDept = m_pApp->get_dept() ;
	StationArea	*pAriv = m_pApp->get_ariv() ;
	ResultArea	*pResult = m_pApp->get_result() ;

	switch ( idCtrl ) {
	case IDM_ABOUT:
		if ( AboutDialogExec( m_pApp->get_inst(), hWnd, &fDelete ) ) {
			if ( fDelete ) {
				m_fDeleteReg = TRUE ;
				SendMessage( hWnd, WM_CLOSE, 0, 0 ) ;
			}
		}
		break ;
	case IDM_EXIT:
		SendMessage( hWnd, WM_CLOSE, 0, 0 ) ;
		break ;
	case IDM_SEARCH:
		m_pApp->Search() ;
		break ;
	case IDM_OPEN:
		if ( !m_pApp->OpenDatafile() ) {
			SendMessage( hWnd, WM_CLOSE, 0, 0 ) ;
		}
		break ;
	case IDM_SAVE_IMG:
		if ( MessageBox( hWnd, TEXT("Save image file?"), TEXT("Save Image"), MB_YESNO|MB_ICONQUESTION ) == IDYES ) {
			SetCursor( (HCURSOR) IDC_WAIT ) ;
			m_pApp->SaveImagefile( TRUE ) ;
			SetCursor( NULL ) ;
		}
		break ;
	case IDM_TERM_LINE:
		if ( hWndFocus == pDept->get_hwnd() ) {
			fDept = TRUE ;
		} else if ( hWndFocus != pAriv->get_hwnd() ) {
			break ;
		}
		m_pApp->SelectTermLine( fDept ) ;
		break ;
	case IDC_DEPTLINE:
	case IDC_ARIVLINE:
		m_pApp->SelectTermLine( idCtrl == IDC_DEPTLINE ) ;
		break ;
	case IDM_SELECT_LINE:
		if ( hWndFocus == pDept->get_hwnd() ) {
			st = pDept ;
		} else if ( hWndFocus == pAriv->get_hwnd() ) {
			st = pAriv ;
		}
		m_pApp->SelectLine( st ) ;
		break ;
	case IDC_DEPT:
	case IDC_ARIV:
		switch ( notifyCode ) {
		case RBN_ENTER:
			/* ���͂������e���m�肷�� */
			m_pApp->Confirm( idCtrl == IDC_DEPT, TRUE ) ;
			break ;
		case RBN_TAB:
			m_pApp->TabKey( idCtrl ) ;
			break ;
		case RBN_STAB:
			m_pApp->ShiftTabKey( idCtrl ) ;
			break ;
		case RBN_CHANGE:
			m_pApp->Change( idCtrl == IDC_DEPT ) ;
			break ;
		case RBN_KEYDOWN:
			m_pApp->KeyDown( idCtrl == IDC_DEPT, (DWORD) lParam ) ;
			break ;
		case RBN_KILLFOCUS:
			if ( idCtrl == IDC_DEPT ) {
				st = pDept ;
			} else if ( idCtrl == IDC_ARIV ) {
				st = pAriv ;
			}
			if ( st && m_pApp->get_search() == st && hWndFocus == pResult->get_hwnd() ) {
				break ;
			}
			m_pApp->KillFocus( idCtrl ) ;
			break ;
		}
		break ;
	case IDC_RESULT:
		if ( notifyCode == BLN_TAB ) {
			m_pApp->TabKey( idCtrl ) ;
		} else if ( notifyCode == BLN_STAB ) {
			m_pApp->ShiftTabKey( idCtrl ) ;
		} else if ( notifyCode == BLN_CHANGE && m_pApp->is_in_search() && hWndFocus == pResult->get_hwnd() ) {
			DWORD		idx = pResult->GetCurSel() ;
			StationArea	*pSt = m_pApp->get_search() ;
			if ( pSt ) {
				pSt->set_sort_index( idx ) ;
			}
		}
		break ;
	case IDC_LIMITED:
		if ( notifyCode == LAN_CLICK ) {
			m_pApp->get_limited()->Toggle() ;
		}
		break ;
	case IDC_SUPER:
		if ( notifyCode == LAN_CLICK ) {
			m_pApp->get_super()->Toggle() ;
		}
		break ;
	case IDC_OTHER:
		if ( notifyCode == LAN_CLICK ) {
			m_pApp->get_other()->Toggle() ;
		}
		break ;
	case IDC_DETAIL:
		/* �ڍו\���؂�ւ� */
		tmp = SendMessage( m_hWndCB, TB_GETSTATE, (WPARAM) IDC_DETAIL, (LPARAM) 0 ) ;
		if ( tmp != -1 ) {
			m_pApp->SetDetail( tmp & TBSTATE_CHECKED ) ;
		}
		break ;
	case IDC_FIRST:
		/* ����҂����킹�؂�ւ� */
		tmp = SendMessage( m_hWndCB, TB_GETSTATE, (WPARAM) IDC_FIRST, (LPARAM) 0 ) ;
		if ( tmp != -1 ) {
			F_machi = tmp & TBSTATE_CHECKED ;
		}
		break ;
	case IDC_REALTIME:
		/* ����/�����\���؂�ւ� */
		tmp = SendMessage( m_hWndCB, TB_GETSTATE, (WPARAM) IDC_REALTIME, (LPARAM) 0 ) ;
		if ( tmp != -1 ) {
			m_pApp->SetRealTime( tmp & TBSTATE_CHECKED ) ;
		}
		break ;
	case IDM_CHANGETIME:
		m_pApp->ChangeTime() ;
		break ;
	case IDC_MAXWALK:
	case IDC_MAXWAIT:
	case IDC_MAXCHANGE:
		if ( notifyCode == RBN_TAB ) {
			m_pApp->TabKey( idCtrl ) ;
		} else if ( notifyCode == RBN_STAB ) {
			m_pApp->ShiftTabKey( idCtrl ) ;
		}
		break ;
	}
	return 0 ;
}

void
MainWindow::WMNotify( HWND hWnd, int idCtrl, LPNMHDR pnmh )
{
}

void
MainWindow::WMSetFocus()
{
}

BOOL
MainWindow::WMClose()
{
	if ( m_fDeleteReg ) {
		m_pApp->DeleteReg() ;
	} else {
		m_pApp->SetReg() ;
	}
	return FALSE ;
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
						0, 0, 240, 320, //480, 240,
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
	static TBBUTTON	tbButtonW[] = {
		{0,              0, TBSTATE_ENABLED, TBSTYLE_SEP,    0, 0, 0, -1},
		{0,       IDM_OPEN, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, -1},
		{3,   IDM_SAVE_IMG, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, -1},
		{0,              0, TBSTATE_ENABLED, TBSTYLE_SEP,    0, 0, 0, -1},
		{5,     IDC_DETAIL, TBSTATE_ENABLED, TBSTYLE_CHECK,  0, 0, 0, -1},
		{6,      IDC_FIRST, TBSTATE_ENABLED, TBSTYLE_CHECK,  0, 0, 0, -1},
		{7,   IDC_REALTIME, TBSTATE_ENABLED, TBSTYLE_CHECK,  0, 0, 0, -1},
		{0,              0, TBSTATE_ENABLED, TBSTYLE_SEP,    0, 0, 0, -1},
		{2, IDM_CHANGETIME, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, -1},
		{4,IDM_SELECT_LINE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, -1},
		{0,              0, TBSTATE_ENABLED, TBSTYLE_SEP,    0, 0, 0, -1},
		{1,     IDM_SEARCH, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, -1},
		{0,              0, TBSTATE_ENABLED, TBSTYLE_SEP,    0, 0, 0,  0},
	} ;
	static TBBUTTON	tbButtonN[] = {
		{0,              0, TBSTATE_ENABLED, TBSTYLE_SEP,    0, 0, 0, -1},
		{5,     IDC_DETAIL, TBSTATE_ENABLED, TBSTYLE_CHECK,  0, 0, 0, -1},
		{6,      IDC_FIRST, TBSTATE_ENABLED, TBSTYLE_CHECK,  0, 0, 0, -1},
		{7,   IDC_REALTIME, TBSTATE_ENABLED, TBSTYLE_CHECK,  0, 0, 0, -1},
		{0,              0, TBSTATE_ENABLED, TBSTYLE_SEP,    0, 0, 0,  0},
		{4,IDM_SELECT_LINE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, -1},
		{1,     IDM_SEARCH, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, -1},
	} ;

	m_hWndCB = CommandBar_Create( hInst, m_hWnd, 0 ) ;
	if ( g_bNarrow ) {
		CommandBar_InsertMenubar( m_hWndCB, hInst, IDR_NARROW_MENU, 0 ) ;
		CommandBar_AddBitmap( m_hWndCB, hInst, IDR_TOOLBAR1, 8, 16, 16 ) ;
	    CommandBar_AddButtons( m_hWndCB, sizeof(tbButtonN) / sizeof(*tbButtonN), tbButtonN ) ;
	} else {
		CommandBar_InsertMenubar( m_hWndCB, hInst, IDR_MAIN_MENU, 0 ) ;
		CommandBar_AddBitmap( m_hWndCB, hInst, IDR_TOOLBAR1, 8, 16, 16 ) ;
	    CommandBar_AddButtons( m_hWndCB, sizeof(tbButtonW) / sizeof(*tbButtonW), tbButtonW ) ;
	}
	CommandBar_AddAdornments( m_hWndCB, 0, 0 ) ;
	return CommandBar_Height( m_hWndCB ) ;
#else	/* _WIN32_WCE */
	return 0 ;
#endif	/* _WIN32_WCE */
}

/*
 * �R�}���h�o�[�{�^���̏�Ԃ�ݒ肷��
 */
void
MainWindow::SetCBState( DWORD idCtrl, BOOL fState )
{
#ifdef	_WIN32_WCE
	DWORD	oldState ;

	oldState = SendMessage( m_hWndCB, TB_GETSTATE, (WPARAM) idCtrl, (LPARAM) 0 ) ;
	if ( oldState == -1 ) {
		return ;
	}
	if ( !(oldState & TBSTATE_CHECKED) != !fState ) {
		oldState &= ~TBSTATE_CHECKED ;
		oldState |= fState ? TBSTATE_CHECKED : 0 ;
		SendMessage( m_hWndCB, TB_SETSTATE, (WPARAM) idCtrl, MAKELONG(oldState,0) ) ;
	}
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
