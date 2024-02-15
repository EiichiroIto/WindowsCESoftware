#define	STRICT
#include	<windows.h>
#include	<windowsx.h>
#include	<commctrl.h>
#include	<commdlg.h>
#include	<tchar.h>
#include	"kctrl.h"
#include	"controls.h"
#include	"basewin.h"
#include	"fileopen.h"
#include	"resource.h"
#include	"lfunc.h"
#include	"base.h"

#define		MAX_TERM_LIST	(50)

extern	DWORD	AppVersion ;
extern	DWORD	DllVersion ;
extern	BOOL	g_bNarrow ;

/*-------------------------------------------------------------------------*
 * 出発/到着時利用路線選択ダイアログ
 *-------------------------------------------------------------------------*/
class TermLineDialog : public Window {
	static STATIONID	*s_pFirst ;
	static DWORD		s_dNumFirst ;
	HINSTANCE	m_hInst ;
	LPCTSTR		m_pTitle ;
	LINEID		m_dLineID ;
	DWORD		m_dLineID_idx ;

	HWND		m_hWndList ;

public:
	TermLineDialog( HINSTANCE hInst, LINEID LineID ) ;
	~TermLineDialog() ;

	void	SetTitle( LPCTSTR title ) ;
	BOOL	Setup( STATIONID sid ) ;
	void	WMCreate( HWND hDlg ) ;
	LRESULT	WMCommand( HWND hDlg, UINT msg, WPARAM wp, LPARAM lp ) ;

	LINEID	get_lineID() const { return m_dLineID ; }
	static int	term_line_proc( int ln, const BYTE** buf, int* len ) ;
} ;

STATIONID	*TermLineDialog::s_pFirst = 0 ;
DWORD		TermLineDialog::s_dNumFirst = 0 ;

TermLineDialog::TermLineDialog( HINSTANCE hInst, LINEID LineID )
{
	s_pFirst = new STATIONID[ MAX_TERM_LIST ] ;
	s_dNumFirst = 0 ;
	m_hInst = hInst ;
	m_pTitle = 0 ;
	m_dLineID = LineID ;
	m_dLineID_idx = 0 ;
}

TermLineDialog::~TermLineDialog()
{
	delete[] s_pFirst ;
}

void
TermLineDialog::SetTitle( LPCTSTR title )
{
	m_pTitle = title ;
}

BOOL
TermLineDialog::Setup( STATIONID sid )
{
	/* 端駅を通る路線を得る */
	s_dNumFirst = 0 ;
	for ( LINEID lineID = 1 ; lineID < Num_r ; lineID ++ ) {
		if ( get_rosen( lineID )->find( sid ) ) {
			if ( m_dLineID == lineID ) {
				m_dLineID_idx = s_dNumFirst ;
			}
			s_pFirst[ s_dNumFirst ++ ] = lineID ;
			if ( s_dNumFirst >= MAX_TERM_LIST ) {
				break ;
			}
		}
	}
	/* 路線が見つからない場合は選択できない */
	if ( s_dNumFirst < 1 ) {
		return FALSE ;
	}
	return TRUE ;
}

void
TermLineDialog::WMCreate( HWND hDlg )
{
	DWORD	x, y, w, h, dw, dh, w1, h1 ;

	/* ダイアログのタイトルを設定する */
	if ( m_pTitle ) {
		SetWindowText( hDlg, m_pTitle ) ;
	}
	w = GetFontHW() * 30 + 4 ;
	if ( g_bNarrow ) {
		h = GetFontH() * 14 + 4 ;
	} else {
		h = GetFontH() * 8 + 4 ;
	}
	w1 = GetSystemMetrics( SM_CXDLGFRAME ) * 2 ;
	h1 = GetSystemMetrics( SM_CYMENU ) + GetSystemMetrics( SM_CYDLGFRAME ) * 2 ;
	x = GetSystemMetrics( SM_CXSCREEN ) ;
	dw = w + w1 ;
	if ( x >= dw ) {
		x = (x - dw) / 2 ;
	} else {
		dw = x ;
		w = dw - w1 ;
	}
	y = GetSystemMetrics( SM_CYSCREEN ) ;
	dh = h + h1 ;
	if ( y >= dh ) {
		y = (y - dh) / 2 ;
	} else {
		dh = y ;
		h = dh - h1 ;
	}
	MoveWindow( hDlg, x, y, dw, dh, TRUE ) ;
	x = y = 2 ;
	m_hWndList = CreateWindowEx( 0,
					CTRL_BIGLIST,
					TEXT(""),
					WS_CHILD|WS_VISIBLE|WS_BORDER|WS_TABSTOP,
					x, y, w, h,
					hDlg,
					(HMENU) IDC_LINELIST,
					m_hInst,
					0 ) ;
	::SendMessage( m_hWndList, BLM_SETPROC, 0, (LPARAM) term_line_proc ) ;
	::SendMessage( m_hWndList, BLM_SETCURSEL, (WPARAM) m_dLineID_idx, 0 ) ;
	::SetFocus( m_hWndList ) ;
}

LRESULT
TermLineDialog::WMCommand( HWND hDlg, UINT msg, WPARAM wp, LPARAM lp )
{
	WORD	idCtrl = LOWORD(wp) ;
	WORD	notifyCode = HIWORD(wp) ;

	if ( idCtrl == IDC_LINELIST ) {
		if ( notifyCode == BLN_ENTER ) {
			idCtrl = IDOK ;
		} else if ( notifyCode == BLN_ESC ) {
			idCtrl = IDCANCEL ;
		}
	}
	switch ( idCtrl ) {
	case IDCANCEL:
		EndDialog( hDlg, FALSE ) ;
		return TRUE ;
	case IDOK:
		m_dLineID_idx = ::SendMessage( m_hWndList, BLM_GETCURSEL, 0, 0 ) ;
		m_dLineID = s_pFirst[ m_dLineID_idx ] ;
		EndDialog( hDlg, TRUE ) ;
		return TRUE ;
	}
	return 0 ;
}

/*
 * 出発時利用路線名一覧を表示するための行取得ルーチン
 */
int
TermLineDialog::term_line_proc( int ln, const BYTE** buf, int* len )
{
	if ( ln < 0 ) {
		return s_dNumFirst ;
	} else if ( ln >= (int) s_dNumFirst ) {
		return -1 ;
	}
	*buf = get_rosen( s_pFirst[ ln ] )->get_name() ;
	*len = Strlen( *buf ) ;
	return 0 ;
}

BOOL
TermLineDialogExec( HINSTANCE hInst, HWND hWnd, STATIONID sid, LINEID *pLineID )
{
	TermLineDialog		*pDialog ;

	pDialog = new TermLineDialog( hInst, *pLineID ) ;
	pDialog->SetTitle( TEXT("Select Terminal Line") ) ;
	if ( !pDialog->Setup( sid ) ) {
		delete pDialog ;
		return FALSE ;
	} else if ( !DialogBoxParam( hInst, MAKEINTRESOURCE(IDD_DLG_VARIABLE), hWnd, (DLGPROC) DlgProc, (LPARAM) pDialog ) ) {
		delete pDialog ;
		return FALSE ;
	}
	*pLineID = pDialog->get_lineID() ;
	delete pDialog ;
	return TRUE ;
}

/*-------------------------------------------------------------------------*
 * 路線選択ダイアログ
 *-------------------------------------------------------------------------*/
class SelectLineDialog : public Window {
	static BYTE		*s_UseRosen ;	/* 利用有無のフラグ */
	static LINEID	*s_LineList ;	/* 利用路線のリスト */
	static DWORD	s_NumLine ;		/* 利用路線数 */
	static LINEID	s_CurLine ;		/* 現在選択中の路線 */
	HINSTANCE	m_hInst ;
	LPCTSTR		m_pTitle ;
	STATIONID	m_dStationID ;

	HWND		m_hWndCoop ;
	HWND		m_hWndLine ;
	HWND		m_hWndStation ;

public:
	SelectLineDialog( HINSTANCE hInst ) ;
	~SelectLineDialog() ;

	void	SetTitle( LPCTSTR title ) ;
	void	WMCreate( HWND hDlg ) ;
	LRESULT	WMCommand( HWND hDlg, UINT msg, WPARAM wp, LPARAM lp ) ;
	void	SetupUseRosen() ;
	void	SetupLineList( int idx ) ;
	void	SetupStationList( int idx ) ;
	void	Update() ;
	void	ResetAll() ;
	void	ResetCoop() ;
	void	RefreshTop() ;
	STATIONID	GetStationID() const { return m_dStationID ; }

	static int	sel_coop_proc( int ln, const BYTE** buf, int* len ) ;
	static int	sel_line_proc( int ln, const BYTE** buf, int* len ) ;
	static int	mark_line_proc( int ln ) ;
	static int	sel_station_proc( int ln, const BYTE** buf, int* len ) ;
} ;

BYTE	*SelectLineDialog::s_UseRosen = 0 ;		/* 利用有無のフラグ */
LINEID	*SelectLineDialog::s_LineList = 0 ;		/* 利用路線のリスト */
DWORD	SelectLineDialog::s_NumLine = 0 ;		/* 利用路線数 */
LINEID	SelectLineDialog::s_CurLine = RID_NONE ;/* 現在選択中の路線 */

SelectLineDialog::SelectLineDialog( HINSTANCE hInst )
{
	m_hInst = hInst ;
	m_pTitle = 0 ;
	s_UseRosen = new BYTE[ (Num_r + 7) / 8] ;
	Memset( s_UseRosen, 0, (Num_r + 7) / 8 ) ;
	s_LineList = new LINEID[ Num_r ] ;
	s_NumLine = 0 ;
	m_dStationID = SID_NONE ;
}

SelectLineDialog::~SelectLineDialog()
{
	delete[] s_UseRosen ;
	delete[] s_LineList ;
}

void
SelectLineDialog::SetTitle( LPCTSTR title )
{
	m_pTitle = title ;
}

void
SelectLineDialog::WMCreate( HWND hDlg )
{
	HWND	hWndCtrl ;
	TCHAR	unicode[ 20 ] ;
	DWORD	scw = GetSystemMetrics( SM_CXVSCROLL ) ;
	DWORD	x, y, h, dw, dh, tw, th, lh ;
	DWORD	x1, x2, x3, y1, y2, y3, w1, w2, w3, h1, h2, h3 ;

	/* ダイアログのタイトルを設定する */
	if ( m_pTitle ) {
		SetWindowText( hDlg, m_pTitle ) ;
	}
#ifdef	_WIN32_WCE
	x = GetSystemMetrics( SM_CXSCREEN ) ;
#else	/* _WIN32_WCE */
	x = 480 ;
#endif	/* _WIN32_WCE */
	y = GetSystemMetrics( SM_CYSCREEN ) - 20 ;
	lh = GetFontH() + 2 ;
	if ( g_bNarrow ) {
		/* 幅調整 */
		w1 = GetFontHW() * 11 + 6 + scw ;
		w2 = x - w1 - 6 ;
		w3 = x - 4 ;
		h1 = GetFontH() * 9 + 4 ;
		h2 = h1 ;
		h3 = h2 ;
		/* コントロールの位置を作成する */
		x1 = 0 ;
		x2 = x1 + w1 + 2 ;
		x3 = 0 ;
		y1 = 2 ;
		y2 = 2 ;
		y3 = y1 + 2 + lh + h1 ;
		/* ダイアログのサイズと位置を調整する */
		MoveWindow( hDlg, 0, 0, x,
					h1 + h3 + lh * 2 + GetSystemMetrics( SM_CYMENU ) + GetSystemMetrics( SM_CYDLGFRAME ) * 2 + 6,
					TRUE ) ;
	} else {
		/* 幅調整 */
		w1 = GetFontHW() * 11 + 6 + scw ;
		w2 = GetFontHW() * 28 + 6 + scw ;
		w3 = GetFontHW() * 23 + 6 + scw ;
		tw = GetSystemMetrics( SM_CXDLGFRAME ) * 2 + 8 ;
		dw = w1 + w2 + w3 + tw ;
		if ( x >= dw ) {
			x = (x - dw) / 2 ;
		} else {
			dw = x ;
			w1 = (dw - tw) * 11 / 62 ;
			w2 = (dw - tw) * 28 / 62 ;
			w3 = (dw - tw) * 23 / 62 ;
		}
		/* 高さ調整 */
		h = GetFontH() * 10 + 4 ;
		th = GetSystemMetrics( SM_CYMENU ) + GetSystemMetrics( SM_CYDLGFRAME ) * 2 + 6 ;
		dh = lh + h + th ;
		if ( y >= dh ) {
			y = (y - dh) / 2 ;
		} else {
			dh = y ;
			h = dh - lh - th ;
		}
		h1 = h2 = h3 = h ;
		/* コントロールの位置を作成する */
		x1 = 2 ;
		x2 = x1 + w1 + 2 ;
		x3 = x2 + w2 + 2 ;
		y1 = y2 = y3 = 2 ;
		/* ダイアログのサイズと位置を調整する */
		MoveWindow( hDlg, x, y, dw, dh, TRUE ) ;
	}
	/* 営業会社のタイトルを作成する */
	sjis2unicode( (LPBYTE) "営業会社", unicode, sizeof unicode ) ;
	hWndCtrl = CreateWindowEx( 0,
					CTRL_LABEL,
					unicode,
					WS_CHILD|WS_VISIBLE|WS_BORDER,
					x1, y1, w1, lh,
					hDlg,
					(HMENU) IDC_COOPCAPT,
					m_hInst,
					0 ) ;
	SendMessage( hWndCtrl, LAM_SETINVERT, (WPARAM) TRUE, 0 ) ;
	/* 路線のタイトルを作成する */
	sjis2unicode( (LPBYTE) "路線", unicode, sizeof unicode ) ;
	hWndCtrl = CreateWindowEx( 0,
					CTRL_LABEL,
					unicode,
					WS_CHILD|WS_VISIBLE|WS_BORDER,
					x2, y2, w2, lh,
					hDlg,
					(HMENU) IDC_LINECAPT,
					m_hInst,
					0 ) ;
	SendMessage( hWndCtrl, LAM_SETINVERT, (WPARAM) TRUE, 0 ) ;
	/* 停車駅のタイトルを作成する */
	sjis2unicode( (LPBYTE) "停車駅", unicode, sizeof unicode ) ;
	hWndCtrl = CreateWindowEx( 0,
					CTRL_LABEL,
					unicode,
					WS_CHILD|WS_VISIBLE|WS_BORDER,
					x3, y3, w3, lh,
					hDlg,
					(HMENU) IDC_STATIONCAPT,
					m_hInst,
					0 ) ;
	SendMessage( hWndCtrl, LAM_SETINVERT, (WPARAM) TRUE, 0 ) ;
	/* 位置の補正を行う */
	y1 += lh ;
	y2 += lh ;
	y3 += lh ;
	/* 営業会社リストを作成する */
	m_hWndCoop = CreateWindowEx( 0,
						CTRL_BIGLIST,
						TEXT(""),
						WS_CHILD|WS_VISIBLE|WS_BORDER|WS_TABSTOP|WS_VSCROLL,
						x1, y1, w1, h1,
						hDlg,
						(HMENU) IDC_COOPLIST,
						m_hInst,
						0 ) ;
	::SendMessage( m_hWndCoop, BLM_SETPROC, 0, (LPARAM) sel_coop_proc ) ;
	/* 路線リストを作成する */
	m_hWndLine = CreateWindowEx( 0,
						CTRL_BIGLIST,
						TEXT(""),
						WS_CHILD|WS_VISIBLE|WS_BORDER|WS_TABSTOP|WS_VSCROLL,
						x2, y2, w2, h2,
						hDlg,
						(HMENU) IDC_LINELIST,
						m_hInst,
						0 ) ;
	/* 駅リストを作成する */
	m_hWndStation = CreateWindowEx( 0,
						CTRL_BIGLIST,
						TEXT(""),
						WS_CHILD|WS_VISIBLE|WS_BORDER|WS_TABSTOP|WS_VSCROLL,
						x3, y3, w3, h3,
						hDlg,
						(HMENU) IDC_STATIONLIST,
						m_hInst,
						0 ) ;
	::SetFocus( m_hWndCoop ) ;
	/* UseRosenを初期化する */
	SetupUseRosen() ;
}

LRESULT
SelectLineDialog::WMCommand( HWND hDlg, UINT msg, WPARAM wp, LPARAM lp )
{
	int		idx ;
	HWND	hWndCtrl = (HWND) lp ;
	WORD	idCtrl = LOWORD(wp) ;
	WORD	notifyCode = HIWORD(wp) ;

	if ( idCtrl == IDC_COOPLIST ) {
		if ( notifyCode == BLN_ENTER || notifyCode == BLN_TAB ) {
			idCtrl = IDOK ;
			hWndCtrl = m_hWndCoop ;
		} else if ( notifyCode == BLN_ESC ) {
			idCtrl = IDCANCEL ;
		}
	} else if ( idCtrl == IDC_LINELIST ) {
		if ( notifyCode == BLN_ENTER || notifyCode == BLN_TAB ) {
			idCtrl = IDOK ;
			hWndCtrl = m_hWndLine ;
		} else if ( notifyCode == BLN_ESC ) {
			idCtrl = IDCANCEL ;
		} else if ( notifyCode == BLN_STAB ) {
			RefreshTop() ;
		}
	} else if ( idCtrl == IDC_STATIONLIST ) {
		if ( notifyCode == BLN_ENTER ) {
			idCtrl = IDOK ;
			hWndCtrl = m_hWndStation ;
		} else if ( notifyCode == BLN_ESC ) {
			idCtrl = IDCANCEL ;
		} else if ( notifyCode == BLN_STAB ) {
			::SendMessage( m_hWndStation, BLM_SETPROC, 0, (LPARAM) 0 ) ;
			::SetFocus( m_hWndLine ) ;
		}
	}
	switch ( idCtrl ) {
	case IDCANCEL:
		EndDialog( hDlg, FALSE ) ;
		return TRUE ;
	case IDOK:
		if ( hWndCtrl == m_hWndCoop ) {
			idx = SendMessage( m_hWndCoop, BLM_GETCURSEL, 0, 0 ) ;
			SetupLineList( idx ) ;
			::SendMessage( m_hWndLine, BLM_SETPROC, 0, (LPARAM) sel_line_proc ) ;
			::SendMessage( m_hWndLine, BLM_SETSPACEPROC, 0, (LPARAM) mark_line_proc ) ;
			::SendMessage( m_hWndStation, BLM_SETPROC, 0, (LPARAM) 0 ) ;
			::SetFocus( m_hWndLine ) ;
			break ;
		} else if ( hWndCtrl == m_hWndLine ) {
			idx = SendMessage( m_hWndLine, BLM_GETCURSEL, 0, 0 ) ;
			SetupStationList( idx ) ;
			::SendMessage( m_hWndStation, BLM_SETPROC, 0, (LPARAM) sel_station_proc ) ;
			::SetFocus( m_hWndStation ) ;
			break ;
		}
		if ( hWndCtrl == m_hWndStation && s_CurLine != RID_NONE ) {
			idx = SendMessage( m_hWndStation, BLM_GETCURSEL, 0, 0 ) ;
			m_dStationID = get_rosen( s_CurLine )->get_s_id( idx ) ;
		}
		Update() ;
		EndDialog( hDlg, TRUE ) ;
		return TRUE ;
	case IDC_COOPCAPT:
		ResetAll() ;
		::InvalidateRect( m_hWndCoop, NULL, TRUE ) ;
		break ;
	case IDC_LINECAPT:
		ResetCoop() ;
		::InvalidateRect( m_hWndLine, NULL, TRUE ) ;
		break ;
	case IDC_STATIONCAPT:
		break ;
	}
	return 0 ;
}

/*
 * 路線使用状況のセットアップ
 */
void
SelectLineDialog::SetupUseRosen()
{
	int		kid ;
	LINEID	r_id ;

	for ( kid = 0 ; kid < Num_kaisya ; kid ++ ) {
		Kaisya_use[ kid ] = 1 ;
	}
	for ( r_id = 1 ; r_id < Num_r ; r_id ++ ) {
		ROSEN	*r = get_rosen( r_id ) ;
		int		use = r->get_use() ;

		if ( use ) {
			s_UseRosen[ r_id / 8 ] |= 1 << (r_id % 8) ;
		}
		kid = r->get_kid() ;
		Assert( kid != KID_NONE ) ;
		Kaisya_use[ kid ] = Kaisya_use[ kid ] && use ;
	}
}

/*
 * 利用路線選択のための初期化
 */
void
SelectLineDialog::SetupLineList( int idx )
{
	LINEID	r_id ;
	int		coopLen ;
	BYTE	coopName[ 100 ] ;

	Strcpy( coopName, StringGet( Kaisya_idx[ idx ] ) ) ;
	coopLen = Kaisya_len[ idx ] ;
	s_NumLine = 0 ;
	for ( r_id = 1 ; r_id < Num_r ; r_id ++ ) {
		const ROSEN		*r = get_rosen( r_id ) ;
		const BYTE		*name = StringGet( r->get_name_idx() ) ;
		int				len = r->get_len_kaisya() ;
		if ( coopLen == len && !Memcmp( coopName, name, len ) ) {
			s_LineList[ s_NumLine ++ ] = r_id ;
		}
	}
	s_CurLine = RID_NONE ;
}

/*
 * 駅名一覧のための初期化
 */
void
SelectLineDialog::SetupStationList( int idx )
{
	s_CurLine = s_LineList[ idx ] ;
}

/*
 * 修正内容を路線データに反映する
 */
void
SelectLineDialog::Update()
{
	LINEID		r_id ;

	for ( r_id = 1 ; r_id < Num_r ; r_id ++ ) {
		ROSEN	*r = get_rosen( r_id ) ;
		r->set_use( s_UseRosen[ r_id / 8 ] & (1 << (r_id % 8)) ? 1 : 0 ) ;
	}
}

/*
 * 全てのマークを解除する
 */
void
SelectLineDialog::ResetAll()
{
	for ( LINEID r_id = 1 ; r_id < Num_r ; r_id ++ ) {
		s_UseRosen[ r_id / 8 ] |= 1 << (r_id % 8) ;
	}
}

/*
 * 該当会社の路線だけ解除する
 */
void
SelectLineDialog::ResetCoop()
{
	for ( int i = 0 ; i < (int) s_NumLine ; i ++ ) {
		LINEID	r_id = s_LineList[ i ] ;
		int		mask = 1 << (r_id % 8) ;
		BYTE	*p_use = &s_UseRosen[ r_id / 8 ] ;

		(*p_use) |= mask ;
	}
}

/*
 * 営業会社のリストに移り再描画する
 */
void
SelectLineDialog::RefreshTop()
{
	s_NumLine = 0 ;
	::SendMessage( m_hWndLine, BLM_SETPROC, 0, (LPARAM) 0 ) ;
	::SendMessage( m_hWndStation, BLM_SETPROC, 0, (LPARAM) 0 ) ;
	::InvalidateRect( m_hWndCoop, NULL, TRUE ) ;
	::SetFocus( m_hWndCoop ) ;
}

/*
 * 利用運行会社名一覧を表示するための行取得ルーチン
 */
int
SelectLineDialog::sel_coop_proc( int ln, const BYTE** buf, int* len )
{
	static BYTE	tmpbuf[ 20 ] ;

    if ( ln < 0 ) {
		return Num_kaisya ;
    } else if ( ln >= Num_kaisya ) {
		return -1 ;
    }
	Memset( tmpbuf, ' ', 19 ) ;
	tmpbuf[ 0 ] = Kaisya_use[ ln ] ? ' ' : 'x' ;
	Memcpy( &tmpbuf[1], StringGet( Kaisya_idx[ ln ] ), 18 ) ;
	tmpbuf[ 19 ] = 0 ;
    *buf = tmpbuf ;
    if ( Kaisya_len[ ln ] == 0 ) {
		*len = Strlen( *buf ) + 1 ;
    } else {
		*len = Kaisya_len[ ln ] + 1 ;
    }
    return 0 ;
}

/*
 * 利用路線名一覧を表示するための行取得ルーチン
 */
int
SelectLineDialog::sel_line_proc( int ln, const BYTE** buf, int* len )
{
	static	BYTE	tmpbuf[ 40 ] ;

	if ( ln < 0 ) {
		return s_NumLine ;
	} else if ( ln >= (int) s_NumLine ) {
		return -1 ;
	}
	LINEID		r_id = s_LineList[ ln ] ;
	const ROSEN	*r = get_rosen( r_id ) ;
	const BYTE	*r_name = StringGet( r->get_name_idx() ) ;
	int			k_len = r->get_len_kaisya() ;
	int			r_len = Strlen( r_name ) ;

	Memset( tmpbuf, ' ', 29 ) ;	tmpbuf[ 29 ] = 0 ;
	tmpbuf[ 0 ] = (s_UseRosen[ r_id / 8 ] & (1 << (r_id % 8))) ? ' ' : 'x' ;
	if ( r_len > k_len ) {
		r_len -= k_len ;
		r_name += k_len ;
	}
	if ( r_len > 24 ) {
		r_len = 24 ;
	}
	Memcpy( &tmpbuf[ 1 ], r_name, r_len ) ;
	ItoaN( r->get_wait(), &tmpbuf[ 25 ], 3 ) ;
	*buf = tmpbuf ;
	*len = Strlen( *buf ) ;
	return r->get_use() ? 0 : 1 ;
}

/*
 * 利用路線名一覧の選択ルーチン
 */
int
SelectLineDialog::mark_line_proc( int ln )
{
	LINEID	r_id = s_LineList[ ln ] ;
	int		mask = 1 << (r_id % 8) ;
	BYTE	*p_use = &s_UseRosen[ r_id / 8 ] ;

	if ( (*p_use) & mask ) {
		(*p_use) &= ~mask ;
	} else {
		(*p_use) |= mask ;
	}
	return 1 ;
}

/*
 * （指定路線の）駅一覧を表示するための行取得ルーチン
 */
int
SelectLineDialog::sel_station_proc( int ln, const BYTE** buf, int* len )
{
	static	BYTE	tmpbuf[ 100 ] ;
	BYTE			tmpbuf2[ 20 ] ;
	int				tmpd ;
	LPBYTE			ptr ;

	if ( s_CurLine == RID_NONE ) {
		return ln < 0 ? 0 : -1 ;
	}
	const ROSEN*	r = get_rosen( s_CurLine ) ;
	if ( ln < 0 ) {
		return r->get_num() ;
	} else if ( ln >= r->get_num() ) {
		return -1 ;
	}
	const int		s_id = r->get_s_id( ln ) ;

	ptr = tmpbuf ;
	/* 停車駅の名称 */
	StrAppend( &ptr, get_station( s_id )->get_name() ) ;
	tmpd = MAX_ST_NAME - Strlen( tmpbuf ) ;
	if ( tmpd > 0 ) {
		Memset( ptr, ' ', tmpd ),    ptr += tmpd ;
	}
	/* 直前の駅からの所要時間 */
	tmpd = (ln > 0) ? (r->get_time( ln ) - r->get_time( ln - 1 )) : 0 ;
	ItoaN( tmpd, tmpbuf2, 3 ) ;
	StrAppend( &ptr, tmpbuf2 ) ;
	*ptr = 0 ;
	/* 返却データ */
	*buf = tmpbuf ;
	*len = Strlen( *buf ) ;
	return 0 ;
}

BOOL
SelectLineDialogExec( HINSTANCE hInst, HWND hWnd, STATIONID *sid )
{
	SelectLineDialog		*pDialog ;

	pDialog = new SelectLineDialog( hInst ) ;
	pDialog->SetTitle( TEXT("Select Line") ) ;
	if ( !DialogBoxParam( hInst, MAKEINTRESOURCE(IDD_DLG_VARIABLE), hWnd, (DLGPROC) DlgProc, (LPARAM) pDialog ) ) {
		delete pDialog ;
		return FALSE ;
	}
	*sid = pDialog->GetStationID() ;
	delete pDialog ;
	return TRUE ;
}

/*-------------------------------------------------------------------------*
 * 時刻入力ダイアログ
 *-------------------------------------------------------------------------*/
class SetTimeDialog : public Window {
	HINSTANCE	m_hInst ;
	LPCTSTR		m_pTitle ;
	HWND		m_hWndDept ;
	HWND		m_hWndAriv ;
	DWORD		m_dDeptTime ;	/* 出発時刻 */
	DWORD		m_dDiffTime ;	/* 所要時間 */

public:
	SetTimeDialog( HINSTANCE hInst, DWORD dDept, DWORD dDiff ) ;
	~SetTimeDialog() ;

	void	SetTitle( LPCTSTR title ) ;
	void	WMCreate( HWND hDlg ) ;
	LRESULT	WMCommand( HWND hDlg, UINT msg, WPARAM wp, LPARAM lp ) ;

	DWORD	GetDeptTime() const { return m_dDeptTime ; }
} ;

SetTimeDialog::SetTimeDialog( HINSTANCE hInst, DWORD dDept, DWORD dDiff )
{
	m_hInst = hInst ;
	m_dDeptTime = dDept ;
	m_dDiffTime = dDiff ;
	m_pTitle = 0 ;
}

SetTimeDialog::~SetTimeDialog()
{
}

void
SetTimeDialog::SetTitle( LPCTSTR title )
{
	m_pTitle = title ;
}

void
SetTimeDialog::WMCreate( HWND hDlg )
{
	TCHAR	unicode[ 20 ] ;
	DWORD	dw, dh, tw, th ;
	DWORD	x, y, x1, x2, x3, y1, y2, w1, w2, w3, h1, h2 ;

	/* ダイアログのタイトルを設定する */
	if ( m_pTitle ) {
		SetWindowText( hDlg, m_pTitle ) ;
	}
	/* 幅調整 */
	x = GetSystemMetrics( SM_CXSCREEN ) ;
	w1 = GetFontHW() * 8 + 6 ;
	w2 = GetFontHW() * 5 + 6 ;
	w3 = GetFontHW() * 6 + 6 ;
	tw = GetSystemMetrics( SM_CXDLGFRAME ) * 2 + 4 * 4 ;
	dw = w1 + w2 + w3 + tw ;
	if ( x >= dw ) {
		x = (x - dw) / 2 ;
	} else {
		dw = x ;
		w1 = (dw - tw) * 8 / 19 ;
		w2 = (dw - tw) * 5 / 19 ;
		w3 = (dw - tw) * 6 / 19 ;
	}
	/* 高さ調整 */
	y = GetSystemMetrics( SM_CYSCREEN ) ;
	h1 = GetFontH() + 4 ;
	h2 = GetFontH() + 4 ;
	th = GetSystemMetrics( SM_CYMENU ) + GetSystemMetrics( SM_CYDLGFRAME ) * 2 + 12 ;
	dh = h1 + h2 + th ;
	if ( y >= dh ) {
		y = (y - dh) / 2 ;
	} else {
		dh = y ;
		h1 = h2 = (dh - th) / 2 ;
	}
	MoveWindow( hDlg, x, y, dw, dh, TRUE ) ;
	x1 = 4 ;
	x2 = x1 + w1 + 4 ;
	x3 = x2 + w2 + 4 ;
	y1 = 4 ;
	y2 = y1 + h1 + 4 ;
	/* 出発時刻のタイトルを作成する */
	sjis2unicode( (LPBYTE) "出発時刻", unicode, sizeof unicode ) ;
	CreateWindowEx( 0,
					CTRL_LABEL,
					unicode,
					WS_CHILD|WS_VISIBLE,
					x1, y1, w1, h1,
					hDlg,
					0,
					m_hInst,
					0 ) ;
	/* 到着時刻のタイトルを作成する */
	sjis2unicode( (LPBYTE) "到着時刻", unicode, sizeof unicode ) ;
	CreateWindowEx( 0,
					CTRL_LABEL,
					unicode,
					WS_CHILD|WS_VISIBLE,
					x1, y2, w1, h2,
					hDlg,
					0,
					m_hInst,
					0 ) ;
	/* 出発時刻の入力領域を作成する */
	m_hWndDept = CreateWindowEx( 0,
					CTRL_TIMEEDIT,
					TEXT(""),
					WS_CHILD|WS_VISIBLE|WS_BORDER|WS_TABSTOP,
					x2, y1, w2, h1,
					hDlg,
					(HMENU) IDC_DEPTTIME,
					m_hInst,
					0 ) ;
	::SendMessage( m_hWndDept, TEM_SETTIMEVAL, (WPARAM) m_dDeptTime, 0 ) ;
	/* 到着時刻の入力領域を作成する */
	m_hWndAriv = CreateWindowEx( 0,
					CTRL_TIMEEDIT,
					TEXT(""),
					WS_CHILD|WS_VISIBLE|WS_BORDER|WS_TABSTOP,
					x2, y2, w2, h2,
					hDlg,
					(HMENU) IDC_ARIVTIME,
					m_hInst,
					0 ) ;
	/* 出発時刻の現在時刻ボタンを作成する */
	CreateWindowEx( 0,
					TEXT("BUTTON"),
					TEXT("Now"),
					WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,
					x3, y1, w3, h1,
					hDlg,
					(HMENU) IDC_DEPTNOW,
					m_hInst,
					0 ) ;
	/* 到着時刻の現在時刻ボタンを作成する */
	CreateWindowEx( 0,
					TEXT("BUTTON"),
					TEXT("Now"),
					WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,
					x3, y2, w3, h2,
					hDlg,
					(HMENU) IDC_ARIVNOW,
					m_hInst,
					0 ) ;
	::SendMessage( m_hWndAriv, TEM_SETTIMEVAL, (WPARAM) (m_dDeptTime + m_dDiffTime), 0 ) ;
	::SetFocus( m_hWndDept ) ;
}

LRESULT
SetTimeDialog::WMCommand( HWND hDlg, UINT msg, WPARAM wp, LPARAM lp )
{
	int		tmp ;
	DWORD	dArivTime ;
	BYTE	tmpbuf[ 20 ] ;
	WORD	idCtrl = LOWORD(wp) ;
	WORD	notifyCode = HIWORD(wp) ;

	switch ( idCtrl ) {
	case IDCANCEL:
		EndDialog( hDlg, FALSE ) ;
		return TRUE ;
	case IDOK:
		m_dDeptTime = SendMessage( m_hWndDept, TEM_GETTIMEVAL, 0, 0 ) ;
		EndDialog( hDlg, TRUE ) ;
		return TRUE ;
	case IDC_DEPTTIME:
		if ( notifyCode == TEN_CHANGE ) {
			m_dDeptTime = SendMessage( m_hWndDept, TEM_GETTIMEVAL, 0, 0 ) ;
			dArivTime = m_dDeptTime + m_dDiffTime ;
			SendMessage( m_hWndAriv, TEM_SETTIMEVAL, (WPARAM) dArivTime, 0 ) ;
		}
		break ;
	case IDC_ARIVTIME:
		if ( notifyCode == TEN_CHANGE ) {
			dArivTime = SendMessage( m_hWndAriv, TEM_GETTIMEVAL, 0, 0 ) ;
			m_dDeptTime = dArivTime + 24 * 60 - m_dDiffTime ;
			SendMessage( m_hWndDept, TEM_SETTIMEVAL, (WPARAM) m_dDeptTime, 0 ) ;
		}
		break ;
	case IDC_DEPTNOW:
		/* 現在時刻の取得 */
		tmp = 0 ;
		get_nowstr( tmpbuf ) ;
		parse_time( &tmpbuf[9], &tmp ) ;
		m_dDeptTime = tmp ;
		SendMessage( m_hWndDept, TEM_SETTIMEVAL, (WPARAM) m_dDeptTime, 0 ) ;
		dArivTime = m_dDeptTime + m_dDiffTime ;
		SendMessage( m_hWndAriv, TEM_SETTIMEVAL, (WPARAM) dArivTime, 0 ) ;
		::SetFocus( m_hWndDept ) ;
		break ;
	case IDC_ARIVNOW:
		/* 現在時刻の取得 */
		tmp = 0 ;
		get_nowstr( tmpbuf ) ;
		parse_time( &tmpbuf[9], &tmp ) ;
		dArivTime = tmp ;
		SendMessage( m_hWndAriv, TEM_SETTIMEVAL, (WPARAM) dArivTime, 0 ) ;
		m_dDeptTime = dArivTime + 24 * 60 - m_dDiffTime ;
		SendMessage( m_hWndDept, TEM_SETTIMEVAL, (WPARAM) m_dDeptTime, 0 ) ;
		::SetFocus( m_hWndAriv ) ;
		break ;
	}
	return 0 ;
}

BOOL
SetTimeDialogExec( HINSTANCE hInst, HWND hWnd, LPDWORD pDeptTime, DWORD dDiffTime )
{
	SetTimeDialog		*pDialog ;

	pDialog = new SetTimeDialog( hInst, *pDeptTime, dDiffTime ) ;
	pDialog->SetTitle( TEXT("DEPT/ARIV Time") ) ;
	if ( !DialogBoxParam( hInst, MAKEINTRESOURCE(IDD_DLG_VARIABLE), hWnd, (DLGPROC) DlgProc, (LPARAM) pDialog ) ) {
		delete pDialog ;
		return FALSE ;
	}
	*pDeptTime = pDialog->GetDeptTime() ;
	delete pDialog ;
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

	wsprintf( buf, TEXT("Train for Windows%c CE V%d.%02d"),
			  0xAE, AppVersion / 100, AppVersion % 100 ) ;
	SetDlgItemText( hDlg, IDC_VERSION, buf ) ;
	wsprintf( buf, TEXT("KCTRL Ver %d.%02d"),
			  DllVersion / 100, DllVersion % 100 ) ;
	SetDlgItemText( hDlg, IDC_DLLVERSION, buf ) ;
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
		if ( MessageBox( hDlg, TEXT("Delete registry information for TRAIN?"), TEXT("UNINSTALL"), MB_YESNO|MB_ICONQUESTION ) == IDYES ) {
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
 * ファイルオープンダイアログ
 *-------------------------------------------------------------------------*/
static	OPENFILENAME	ofn ;
static	TCHAR			buf1[ MAX_PATH + 1 ] ;

BOOL
FileOpenDlg( HINSTANCE hInst, HWND hWnd, LPTSTR buf, DWORD size )
{
	memset( &ofn, 0, sizeof ofn ) ;
	memcpy( buf1, TEXT("*.dat\0"), sizeof TEXT("*.dat\0") ) ;
	ofn.lStructSize = sizeof ofn ;
    ofn.hwndOwner = hWnd ;
    ofn.hInstance = hInst ;
    ofn.lpstrFilter = TEXT("Data Files (*.dat)\0*.dat\0Image Files (*.tri)\0*.tri\0All Files (*.*)\0*.*\0\0") ;
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
    ofn.lpstrDefExt = TEXT("DAT") ;
    ofn.lCustData = 0 ;
    ofn.lpfnHook = NULL ;
    ofn.lpTemplateName = NULL ;
	if ( !GetOpenFileName2( &ofn, TEXT("TRAIN") ) ) {
		return FALSE ;
	}
	_tcscpy( buf, buf1 ) ;
	return TRUE ;
}
