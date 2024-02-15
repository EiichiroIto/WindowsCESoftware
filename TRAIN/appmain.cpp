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

#define		DAT_EXT		TEXT("DAT")
#define		IMG_EXT		TEXT("TRI")
#define		MAX_RESULT	200

TCHAR		MessageBuf[ 512 ] ;
CHAR		VersionStr[] = "0.01" ;
RESULT		Result[ MAX_RESULT ] ;
DWORD		SizeResult = MAX_RESULT ;

BOOL	FileOpenDlg( HINSTANCE hInst, HWND hWnd, LPTSTR buf, DWORD size ) ;
BOOL	TermLineDialogExec( HINSTANCE hInst, HWND hWnd, STATIONID sid, LINEID *pRid ) ;
BOOL	SelectLineDialogExec( HINSTANCE hInst, HWND hWnd, STATIONID *sid ) ;
BOOL	SetTimeDialogExec( HINSTANCE hInst, HWND hWnd, LPDWORD pDeptTime, DWORD dDiffTime ) ;

/*-------------------------------------------------------------------------*
 * TrainApp
 *-------------------------------------------------------------------------*/
int		TrainApp::m_dDeptTime = 0 ;		/* 出発時刻 */
DWORD	TrainApp::m_dNumDetailed = 0 ;	/* 詳細モードでの項目数 */
DWORD	TrainApp::m_dNumSummary = 0 ;	/* 概要モードでの項目数 */
DWORD	TrainApp::m_dwAddLen = 13 ;		/* 結果表示領域の追加空白数 */

TrainApp::TrainApp()
{
	m_pMainWnd = 0 ;
	m_pDept      = new StationArea( &Sel_deptline ) ;
	m_pAriv      = new StationArea( &Sel_arivline ) ;
	m_pResult    = new ResultArea() ;
	m_pWalk      = new TimeArea( &Walk_time ) ;
	m_pWait      = new TimeArea( &Wait_time ) ;
	m_pBoad      = new TimeArea( &Jyosya_time ) ;
	m_pTotal     = new TimeArea( &Total_time ) ;
	m_pLimited   = new KindArea( &F_nouse, 0x01 ) ;
	m_pSuper     = new KindArea( &F_nouse, 0x02 ) ;
	m_pOther     = new KindArea( &F_nouse, 0x04 ) ;
	m_pMaxWalk   = new MaxArea( &F_walk, 32767 ) ;
	m_pMaxWait   = new MaxArea( &F_wait, 32767 ) ;
	m_pMaxChange = new MaxArea( &F_change, 100 ) ;
	m_path[0] = m_base[0] = 0 ;
	m_fNeedData = TRUE ;
	m_pISearch = NULL ;
	m_fResult = FALSE ;

	m_dDeptTime = 0 ;
	m_fDetail = TRUE ;				/* 詳細表示フラグ */
	m_fRealTime = TRUE ;			/* 実時間フラグ */
	m_dImage = 1 ;					/* イメージ読み込みモード */
}

TrainApp::~TrainApp()
{
	delete m_pDept ;
	delete m_pAriv ;
	delete m_pResult ;
	delete m_pWalk ;
	delete m_pWait ;
	delete m_pBoad ;
	delete m_pTotal ;
	delete m_pLimited ;
	delete m_pSuper ;
	delete m_pOther ;
	delete m_pMaxWalk ;
	delete m_pMaxWait ;
	delete m_pMaxChange ;
}

BOOL
TrainApp::Create( HINSTANCE hInstance )
{
	m_hInst = hInstance ;

	/* ウィンドウクラスを設定する */
	if ( !LineEditRegisterClass( m_hInst ) ) {
		return FALSE ;
	}
	if ( !TimeEditRegisterClass( m_hInst ) ) {
		return FALSE ;
	}
	if ( !BigListRegisterClass( m_hInst ) ) {
		return FALSE ;
	}
	if ( !LabelRegisterClass( m_hInst ) ) {
		return FALSE ;
	}
	if ( !MainWindow::RegisterClass( m_hInst ) ) {
		return FALSE ;
	}

	/* レジストリの設定を読み込む */
	GetReg() ;

#ifdef	_WIN32_WCE
	m_hAccl = LoadAccelerators( m_hInst,
								MAKEINTRESOURCE(IDR_ACCELERATOR1) ) ;
#else	/* _WIN32_WCE */
	m_hAccl = LoadAccelerators( GetModuleHandle(NULL),
								MAKEINTRESOURCE(IDR_ACCELERATOR1) ) ;

#endif	/* _WIN32_WCE */

	m_pMainWnd = new MainWindow( this ) ;
	if ( !m_pMainWnd ) {
		return FALSE ;
	}
	if ( !m_pMainWnd->Create() ) {
		return FALSE ;
	}
	/* コマンドバーボタンの状態を更新する */
	m_pMainWnd->SetCBState( IDC_DETAIL, m_fDetail ) ;
	m_pMainWnd->SetCBState( IDC_FIRST, F_machi ) ;
	m_pMainWnd->SetCBState( IDC_REALTIME, m_fRealTime ) ;
	/* データを初期化する */
	Initialize( TEXT("") ) ;
	if ( m_fNeedData ) {
		/* ファイルの読み込みに失敗したらファイルオープンダイアログを出す */
		PostMessage( m_pMainWnd->get_hwnd(), WM_COMMAND, IDM_OPEN, 0 ) ;
	}
	/* 画面を前面にする */
	SetForegroundWindow( m_pMainWnd->get_hwnd() ) ;
	m_pDept->SetFocus() ;
	return TRUE ;
}

int
TrainApp::Run()
{
	MSG		msg ;

	while ( GetMessage( &msg, NULL, 0, 0 ) ) {
		if ( !TranslateAccelerator( m_pMainWnd->get_hwnd(), m_hAccl, &msg ) ) {
			TranslateMessage( &msg ) ;
		}
		DispatchMessage( &msg ) ;
	}
	return msg.wParam ;
}

BOOL
TrainApp::Reload()
{
	BOOL	ret ;
	TCHAR	fnImage[ MAX_PATH ], *ptr ;

	/* イメージファイルの読み込み */
	_tcscpy( fnImage, m_path ) ;
	ret = _tcslen( m_path ) ;
	_tcscpy( &m_path[ ret - 3 ], DAT_EXT ) ;
	_tcscpy( &fnImage[ ret - 3 ], IMG_EXT ) ;
	if ( (m_dImage & 0x01) && Fopen( fnImage, TEXT("rb") ) ) {
		clear_data() ;
		ret = load_img( (LPBYTE) VersionStr ) ;
		Fclose() ;
		if ( ret ) {
			/* イメージファイル読み込み成功 */
			goto loadok ;
		}
	}
	if ( !Fopen( m_path, TEXT("rb") ) ) {
		m_fNeedData = TRUE ;
		return FALSE ;
	}
	clear_data() ;
	load_data() ;
	init_kaisya() ;
	load_adj() ;
	Fclose() ;

loadok:
	/* 読み込みOK */
	m_fNeedData = FALSE ;
	/* ベース名を取り出す */
	_tcscpy( m_base, m_path ) ;
	ptr = _tcsrchr( m_base, TEXT('\\') ) ;
	if ( ptr ) {
		_tcscpy( m_base, ptr + 1 ) ;
	}
	ptr = _tcsrchr( m_base, TEXT('/') ) ;
	if ( ptr ) {
		_tcscpy( m_base, ptr + 1 ) ;
	}
	ptr = _tcsrchr( m_base, TEXT('.') ) ;
	if ( ptr ) {
		*ptr = 0 ;
	}
	/* ウィンドウタイトルを設定する */
	SetWindowText( m_pMainWnd->get_hwnd(), m_base ) ;
	return TRUE ;
}

void
TrainApp::Initialize( LPCTSTR fn )
{
	BYTE	tmpbuf[ 20 ] ;

	/* initialize */
	Num_s = Num_r = 0 ;
	if ( *fn ) {
		/* コマンドラインに指定されたファイル名を使う */
		_tcscpy( m_path, fn ) ;
	}
	m_fNeedData = TRUE ;
	if ( *m_path ) {
		/* 中間ファイルの読み込み */
		SetCursor( (HCURSOR) IDC_WAIT ) ;
		Reload() ;
		SetCursor( NULL ) ;
	}
	/* 探索結果のカウンタをクリア */
	m_dNumDetailed = m_dNumSummary = 0 ;
	/* 現在時刻の取得 */
	get_nowstr( tmpbuf ) ;
	if ( !parse_time( &tmpbuf[9], &m_dDeptTime ) ) {
		/* 現在時刻の取得に失敗した場合 */
		m_dDeptTime = 0 ;
	}
}

/*
 * 駅名の入力を確定し，探索を行う
 */
void
TrainApp::Confirm( BOOL fDept, BOOL fEnter )
{
	/* インクリメンタルフラグをクリアする */
	set_search( fDept ? m_pDept : m_pAriv ) ;
	if ( fDept ) {
		if ( m_pDept->is_empty() ) {
			SelectLine( m_pDept ) ;
		} else if ( m_pDept->confirm() ) {
			/* 到着駅入力領域に移る */
			m_pAriv->SetFocus() ;
		}
	} else {
		if ( m_pAriv->is_empty() ) {
			SelectLine( m_pAriv ) ;
		} else if ( m_pAriv->confirm() ) {
			if ( fEnter ) {
				/* 検索を開始する */
				Search() ;
			} else {
				/* 出発駅入力領域に移る */
				m_pDept->SetFocus() ;
			}
		}
	}
}

/*
 * ローマ字駅名の入力
 */
void
TrainApp::Change( BOOL fDept )
{
	DWORD			sort_index ;
	CHAR			roma[ 128 ] ;
	StationArea		*pSt = fDept ? m_pDept : m_pAriv ;

	set_result( FALSE ) ;
	pSt->set_term_line( RID_NONE ) ;
	pSt->GetSjis( roma, sizeof roma ) ;
	if ( !roma2sortidx( (LPBYTE) roma, &sort_index ) ) {
		/* 先頭が一致する駅が見つからない場合 */
		/* インクリメンタルフラグをクリアする */
		set_search( NULL ) ;
		return ;
	}
	if ( get_search() != pSt ) {
		m_pResult->SetProc( station_proc ) ;
		/* インクリメンタルフラグをセットする */
		set_search( pSt ) ;
	}
	m_pResult->SetCurSel( sort_index ) ;
}

/*
 * 駅名入力領域で特殊キーが押された場合
 */
void
TrainApp::KeyDown( BOOL fDept, DWORD nVirtKey )
{
	DWORD			sort_index ;
	StationArea		*pSt = fDept ? m_pDept : m_pAriv ;

	if ( !is_in_search() ) {
		/* インクリメンタルサーチ中でなければ何もしない */
		return ;
	}
	m_pResult->SendKey( nVirtKey ) ;
	sort_index = m_pResult->GetCurSel() ;
	/* ソートインデックスから駅名をセットする */
	pSt->set_sort_index( sort_index ) ;
}

/*
 * TABキーが押された場合
 */
void
TrainApp::TabKey( WORD idCtrl ) const
{
	if ( is_in_result() ) {
		switch ( idCtrl ) {
		case IDC_DEPT:		m_pAriv->SetFocus() ;		break ;
		case IDC_ARIV:		m_pResult->SetFocus() ;		break ;
		case IDC_RESULT:	m_pDept->SetFocus() ;		break ;
		case IDC_MAXWALK:	m_pMaxWait->SetFocus() ;	break ;
		case IDC_MAXWAIT:	m_pMaxChange->SetFocus() ;	break ;
		case IDC_MAXCHANGE:	m_pMaxWalk->SetFocus() ;	break ;
		}
	} else {
		switch ( idCtrl ) {
		case IDC_DEPT:		m_pAriv->SetFocus() ;		break ;
		case IDC_RESULT:
		case IDC_ARIV:		m_pDept->SetFocus() ;		break ;
		case IDC_MAXWALK:	m_pMaxWait->SetFocus() ;	break ;
		case IDC_MAXWAIT:	m_pMaxChange->SetFocus() ;	break ;
		case IDC_MAXCHANGE:	m_pMaxWalk->SetFocus() ;	break ;
		}
	}
}

/*
 * Shift+TABキーが押された場合
 */
void
TrainApp::ShiftTabKey( WORD idCtrl ) const
{
	if ( is_in_result() ) {
		switch ( idCtrl ) {
		case IDC_DEPT:		m_pResult->SetFocus() ;		break ;
		case IDC_ARIV:		m_pDept->SetFocus() ;		break ;
		case IDC_RESULT:	m_pAriv->SetFocus() ;		break ;
		case IDC_MAXWALK:	m_pMaxChange->SetFocus() ;	break ;
		case IDC_MAXWAIT:	m_pMaxWalk->SetFocus() ;	break ;
		case IDC_MAXCHANGE:	m_pMaxWait->SetFocus() ;	break ;
		}
	} else {
		switch ( idCtrl ) {
		case IDC_DEPT:		m_pDept->SetFocus() ;		break ;
		case IDC_RESULT:
		case IDC_ARIV:		m_pAriv->SetFocus() ;		break ;
		case IDC_MAXWALK:	m_pMaxChange->SetFocus() ;	break ;
		case IDC_MAXWAIT:	m_pMaxWalk->SetFocus() ;	break ;
		case IDC_MAXCHANGE:	m_pMaxWait->SetFocus() ;	break ;
		}
	}
}

/*
 * 
 */
void
TrainApp::KillFocus( WORD idCtrl )
{
	/* インクリメンタルフラグをクリアする */
	set_search( NULL ) ;
	if ( idCtrl == IDC_DEPT ) {
		m_pDept->confirm() ;
	} else if ( idCtrl == IDC_ARIV ) {
		m_pAriv->confirm() ;
	}
}

/*
 * 探索を行う
 */
BOOL
TrainApp::Search()
{
	DWORD	dept_index, ariv_index ;

	/* インクリメンタルフラグをクリアする */
	set_search( NULL ) ;
	/* 最大値設定を反映する */
	m_pMaxWalk->Update() ;
	m_pMaxWait->Update() ;
	m_pMaxChange->Update() ;
	/* 入力領域の内容を確定する */
	m_pDept->confirm() ;
	m_pAriv->confirm() ;
	/* 正しく入力されているかチェックする */
	if ( !m_pDept->is_valid() ) {
		m_pDept->SetFocus() ;
		return FALSE ;
	} else if ( !m_pAriv->is_valid() ) {
		m_pAriv->SetFocus() ;
		return FALSE ;
	}
	dept_index = m_pDept->get_index() ;
	ariv_index = m_pAriv->get_index() ;
	if ( dept_index == ariv_index ) {
		m_pAriv->SetFocus() ;
		return FALSE ;
	}
	/* 時刻表モードでなければ出発時刻をクリアする */
	if ( !m_fRealTime ) {
		m_dDeptTime = 0 ;
	}
	/* 探索結果のカウンタをクリア */
	m_dNumDetailed = m_dNumSummary = 0 ;
	/* 探索開始 */
	other_job( 0 ) ;
	load_adj() ;
	other_job( 1 ) ;
	make_adj( ariv_index, dept_index ) ;
	other_job( 3 ) ;
	find( 0, 1 ) ;
	other_job( 4 ) ;
	/* 探索結果を作成する */
	MakeRoute() ;
	/* 徒歩時間 */
	m_pWalk->Refresh() ;
	/* 待ち時間 */
	m_pWait->Refresh() ;
	/* 乗車時間 */
	m_pBoad->Refresh() ;
	/* 所要時間 */
	m_pTotal->Refresh() ;

	other_job( 99 ) ;
	/* 結果表示領域に探索結果をセットする */
	m_pResult->SetProc( m_fDetail ? detailed_result_proc : summary_result_proc ) ;
	/* 結果表示中 */
	set_result( TRUE ) ;
	m_pResult->SetFocus() ;
	return TRUE ;
}

/*
 * 探索を行う
 */
void
TrainApp::MakeRoute()
{
	int		count ;
	RESULT	*res = Result ;

	m_dNumDetailed = make_route( Result, SizeResult ) ;
	m_dNumSummary = 0 ;
	count = m_dNumDetailed ;
	if ( !count ) {
		return ;
	}
	do {
		if ( res->type != RESULT_PASS ) {
			m_dNumSummary ++ ;
		}
		res ++ ;
	} while ( -- count ) ;
}

/*
 * 詳細モードの探索結果を表示するための行取得ルーチン
 */
int
TrainApp::detailed_result_proc( int ln, const BYTE** buf, int* len )
{
	if ( ln < 0 ) {
		return m_dNumDetailed ;
	} else if ( ln >= (int) m_dNumDetailed ) {
		return -1 ;
	}
	*buf = get_route( &Result[ ln ], m_dDeptTime, m_dwAddLen ) ;
	*len = Strlen( *buf ) ;
	return 0 ;
}

/*
 * 概要モードの探索結果を表示するための行取得ルーチン
 */
int
TrainApp::summary_result_proc( int ln, const BYTE** buf, int* len )
{
	int		ln2 ;
	RESULT	*res = Result ;
	DWORD	count = m_dNumDetailed ;

	if ( ln < 0 ) {
		return m_dNumSummary ;
	} else if ( ln >= (int) m_dNumSummary ) {
		return -1 ;
	} else if ( !count ) {
		return -1 ;
	}
	ln2 = 0 ;
	do {
		if ( res->type != RESULT_PASS ) {
			if ( !ln ) {
				break ;
			}
			ln -- ;
		}
		ln2 ++ ;
		res ++ ;
	} while ( -- count ) ;
	*buf = get_route( &Result[ ln2 ], m_dDeptTime, m_dwAddLen ) ;
	*len = Strlen( *buf ) ;
	return 0 ;
}

/*
 * 駅名一覧を表示するための行取得ルーチン
 */
int
TrainApp::station_proc( int ln, const BYTE** buf, int* len )
{
	int			station_index ;
	LPBYTE		p ;
	static BYTE	tmpbuf[ 128 ] ;

	if ( ln < 0 ) {
		return Num_s - 1 ;
	} else if ( ln >= Num_s - 1 ) {
		return -1 ;
	}
	station_index = get_sort_s_id( ln ) ;
	p = tmpbuf ;
	StrAppend( &p, get_station( station_index )->get_name() ) ;
	if ( !F_inter ) {
		*p++ = '(' ;
		StrAppend( &p, get_station_kana( station_index ) ) ;
		*p++ = ')' ;
	}
	*p   = 0 ;
	*buf = tmpbuf ;
	*len = Strlen( *buf ) ;
	return 0 ;
}

/*
 * 出発/到着時利用路線の選択を行う
 */
void
TrainApp::SelectTermLine( BOOL fDept )
{
	STATIONID		sid ;
	LINEID			rid = fDept ? Sel_deptline : Sel_arivline ;
	StationArea		*pSt = fDept ? m_pDept : m_pAriv ;

	if ( !pSt->confirm() ) {
		return ;
	}
	sid = pSt->get_index() ;
	if ( !TermLineDialogExec( m_hInst, m_pMainWnd->get_hwnd(), sid, &rid ) ) {
		rid = RID_NONE ;
	}
	pSt->set_term_line( rid ) ;
	pSt->SetFocus() ;
}

/*
 * データファイルオープン
 */
BOOL
TrainApp::OpenDatafile()
{
	while ( 1 ) {
		if ( FileOpenDlg( m_hInst, m_pMainWnd->get_hwnd(), m_path, sizeof m_path ) ) {
			/* 中間ファイルの読み込み */
			SetCursor( (HCURSOR) IDC_WAIT ) ;
			Reload() ;
			SetCursor( NULL ) ;
			if ( m_fNeedData ) {
				/* 中間ファイルを読み込めなかった場合 */
				MessageBeep( MB_ICONHAND ) ;
				continue ;
			}
			/* 画面の項目をクリアする */
			m_pDept->Clear() ;
			m_pAriv->Clear() ;
			m_pResult->Clear() ;
			m_pWalk->Clear() ;
			m_pWait->Clear() ;
			m_pBoad->Clear() ;
			m_pTotal->Clear() ;
			/* 初期フォーカスの設定 */
			m_pDept->SetFocus() ;
			break ;
		} else if ( m_fNeedData ) {
			return FALSE ;
		} else {
			break ;
		}
	}
	return TRUE ;
}

/*
 * イメージファイルセーブ
 */
BOOL
TrainApp::SaveImagefile( BOOL fForce )
{
	DWORD	len ;
	TCHAR	fnImage[ MAX_PATH ] ;

	if ( !(m_dImage & 0x02) && !fForce ) {
		return TRUE ;
	}
	_tcscpy( fnImage, m_path ) ;
	len = _tcslen( fnImage ) ;
	_tcscpy( &fnImage[ len - 3 ], IMG_EXT ) ;
	if ( !Fopen( fnImage, TEXT("wb") ) ) {
		MessageBox( m_pMainWnd->get_hwnd(), TEXT("Can't Save."), TEXT("Save Image File"), MB_OK|MB_ICONHAND ) ;
		return FALSE ;
	}
	save_img( (LPBYTE) VersionStr ) ;
	Fclose() ;
	return TRUE ;
}

#define		REG_SUBKEY		TEXT("Software\\Gawaro\\TRAIN")
#define		VAR_FILENAME	TEXT("Filename")
#define		VAR_DETAIL		TEXT("Detail")
#define		VAR_FIRST		TEXT("FirstWait")
#define		VAR_REALTIME	TEXT("RealTime")
#define		VAR_WALK		TEXT("MaxWalk")
#define		VAR_WAIT		TEXT("MaxWait")
#define		VAR_CHANGE		TEXT("MaxChange")
#define		VAR_NOUSE		TEXT("NoUse")

/*
 * レジストリからの情報取得
 */
BOOL
TrainApp::GetReg()
{
	HKEY	hk ;
	LONG	lret ;
	LPTSTR	name ;
	LPBYTE	lpData ;
	DWORD	dwType, cbData, data ;

	/* レジストリキーをオープンする */
	lret = RegOpenKeyEx( HKEY_CURRENT_USER,
						 REG_SUBKEY,
						 0,
						 KEY_QUERY_VALUE,
						 &hk ) ;
	if ( lret != ERROR_SUCCESS ) {
		return FALSE ;
	}
	/* データファイル名 */
	name = VAR_FILENAME ;
	dwType = REG_SZ ;
	lpData = (LPBYTE) m_path ;
	cbData = sizeof m_path ;
	lret = RegQueryValueEx( hk, name, NULL, &dwType, lpData, &cbData ) ;
	/* 詳細表示 */
	name = VAR_DETAIL ;
	dwType = REG_DWORD ;
	lpData = (LPBYTE) &data ;
	cbData = sizeof (DWORD) ;
	lret = RegQueryValueEx( hk, name, NULL, &dwType, lpData, &cbData ) ;
	if ( lret == ERROR_SUCCESS && cbData != ERROR_MORE_DATA ) {
		m_fDetail = data ;
	}
	/* 初回待ち合わせ有無 */
	name = VAR_FIRST ;
	dwType = REG_DWORD ;
	lpData = (LPBYTE) &data ;
	cbData = sizeof (DWORD) ;
	lret = RegQueryValueEx( hk, name, NULL, &dwType, lpData, &cbData ) ;
	if ( lret == ERROR_SUCCESS && cbData != ERROR_MORE_DATA ) {
		F_machi = data ;
	}
	/* 時間/時刻表示 */
	name = VAR_REALTIME ;
	dwType = REG_DWORD ;
	lpData = (LPBYTE) &data ;
	cbData = sizeof (DWORD) ;
	lret = RegQueryValueEx( hk, name, NULL, &dwType, lpData, &cbData ) ;
	if ( lret == ERROR_SUCCESS && cbData != ERROR_MORE_DATA ) {
		m_fRealTime = data ;
	}
	/* 最大徒歩時間 */
	name = VAR_WALK ;
	dwType = REG_DWORD ;
	lpData = (LPBYTE) &data ;
	cbData = sizeof (DWORD) ;
	lret = RegQueryValueEx( hk, name, NULL, &dwType, lpData, &cbData ) ;
	if ( lret == ERROR_SUCCESS && cbData != ERROR_MORE_DATA ) {
		F_walk = data ;
	}
	/* 最大待ち時間 */
	name = VAR_WAIT ;
	dwType = REG_DWORD ;
	lpData = (LPBYTE) &data ;
	cbData = sizeof (DWORD) ;
	lret = RegQueryValueEx( hk, name, NULL, &dwType, lpData, &cbData ) ;
	if ( lret == ERROR_SUCCESS && cbData != ERROR_MORE_DATA ) {
		F_wait = data ;
	}
	/* 最大乗り換え回数 */
	name = VAR_CHANGE ;
	dwType = REG_DWORD ;
	lpData = (LPBYTE) &data ;
	cbData = sizeof (DWORD) ;
	lret = RegQueryValueEx( hk, name, NULL, &dwType, lpData, &cbData ) ;
	if ( lret == ERROR_SUCCESS && cbData != ERROR_MORE_DATA ) {
		F_change = data ;
	}
	/* 非利用路線種別 */
	name = VAR_NOUSE ;
	dwType = REG_DWORD ;
	lpData = (LPBYTE) &data ;
	cbData = sizeof (DWORD) ;
	lret = RegQueryValueEx( hk, name, NULL, &dwType, lpData, &cbData ) ;
	if ( lret == ERROR_SUCCESS && cbData != ERROR_MORE_DATA ) {
		F_nouse = data ;
	}
	RegCloseKey( hk ) ;
	return TRUE ;
}

/*
 * レジストリへの情報設定
 */
BOOL
TrainApp::SetReg() const
{
	HKEY	hk ;
	LONG	lret ;
	LPTSTR	name ;
	LPBYTE	lpData ;
	DWORD	ret, dwType, cbData, data ;

	/* レジストリキーを作成する */
	lret = RegCreateKeyEx( HKEY_CURRENT_USER, REG_SUBKEY, 0, TEXT(""), 0,
#ifdef	_WIN32_WCE
						   0,
#else	/* _WIN32_WCE */
						   KEY_ALL_ACCESS,
#endif	/* _WIN32_WCE */
						   NULL, &hk, &ret ) ;
	if ( lret != ERROR_SUCCESS ) {
		return FALSE ;
	}
	/* データファイル名 */
	name = VAR_FILENAME ;
	dwType = REG_SZ ;
	lpData = (LPBYTE) m_path ;
	cbData = (_tcslen(m_path) + 1) * sizeof (*m_path) ;
	lret = RegSetValueEx( hk, name, 0, dwType, lpData, cbData ) ;
	/* 詳細表示 */
	data = m_fDetail ;
	name = VAR_DETAIL ;
	dwType = REG_DWORD ;
	lpData = (LPBYTE) &data ;
	cbData = sizeof (DWORD) ;
	lret = RegSetValueEx( hk, name, 0, dwType, lpData, cbData ) ;
	/* 初回待ち合わせ有無 */
	data = F_machi ;
	name = VAR_FIRST ;
	dwType = REG_DWORD ;
	lpData = (LPBYTE) &data ;
	cbData = sizeof (DWORD) ;
	lret = RegSetValueEx( hk, name, 0, dwType, lpData, cbData ) ;
	/* 時間/時刻表示 */
	data = m_fRealTime ;
	name = VAR_REALTIME ;
	dwType = REG_DWORD ;
	lpData = (LPBYTE) &data ;
	cbData = sizeof (DWORD) ;
	lret = RegSetValueEx( hk, name, 0, dwType, lpData, cbData ) ;
	/* 最大徒歩時間 */
	data = F_walk ;
	name = VAR_WALK ;
	dwType = REG_DWORD ;
	lpData = (LPBYTE) &data ;
	cbData = sizeof (DWORD) ;
	lret = RegSetValueEx( hk, name, 0, dwType, lpData, cbData ) ;
	/* 最大待ち時間 */
	data = F_wait ;
	name = VAR_WAIT ;
	dwType = REG_DWORD ;
	lpData = (LPBYTE) &data ;
	cbData = sizeof (DWORD) ;
	lret = RegSetValueEx( hk, name, 0, dwType, lpData, cbData ) ;
	/* 最大乗り換え回数 */
	data = F_change ;
	name = VAR_CHANGE ;
	dwType = REG_DWORD ;
	lpData = (LPBYTE) &data ;
	cbData = sizeof (DWORD) ;
	lret = RegSetValueEx( hk, name, 0, dwType, lpData, cbData ) ;
	/* 非利用路線種別 */
	data = F_nouse ;
	name = VAR_NOUSE ;
	dwType = REG_DWORD ;
	lpData = (LPBYTE) &data ;
	cbData = sizeof (DWORD) ;
	lret = RegSetValueEx( hk, name, 0, dwType, lpData, cbData ) ;
	RegCloseKey( hk ) ;
	return TRUE ;
}

/*
 * レジストリ情報削除
 */
void
TrainApp::DeleteReg() const
{
	RegDeleteKey( HKEY_CURRENT_USER, REG_SUBKEY ) ;
}

/*
 * 詳細表示フラグ設定
 */
void
TrainApp::SetDetail( BOOL fDetail )
{
	m_fDetail = fDetail ;
	if ( is_in_result() ) {
		/* 結果表示領域に探索結果をセットする */
		m_pResult->SetProc( m_fDetail ? detailed_result_proc : summary_result_proc ) ;
		/* 結果表示中 */
		set_result( TRUE ) ;
		m_pResult->SetFocus() ;
	}
}

/*
 * 時刻表表示フラグ設定
 */
void
TrainApp::SetRealTime( BOOL fRealTime )
{
	m_fRealTime = fRealTime ;
}

/*
 * 路線選択
 */
void
TrainApp::SelectLine( StationArea *ptr )
{
	STATIONID	sid ;

	if ( SelectLineDialogExec( m_hInst, m_pMainWnd->get_hwnd(), &sid ) ) {
		if ( sid == SID_NONE ) {
			return ;
		}
		if ( ptr ) {
			ptr->set_station_index( sid ) ;
			ptr->set_term_line( RID_NONE ) ;
			ptr->SetFocus() ;
		}
	}
}

/*
 * 路線選択
 */
void
TrainApp::ChangeTime()
{
	DWORD	dDeptTime, dDiffTime ;

	dDeptTime = m_dDeptTime ;
	dDiffTime = Total_time ;
	if ( !SetTimeDialogExec( m_hInst, m_pMainWnd->get_hwnd(), &dDeptTime, dDiffTime ) ) {
		return ;
	}
	m_dDeptTime = dDeptTime ;
	::InvalidateRect( m_pResult->get_hwnd(), NULL, TRUE ) ;
	return ;
}
