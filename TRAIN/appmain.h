#define		szTitleName		TEXT("Train")

class MainWindow ;
class StationArea ;
class ResultArea ;
class TimeArea ;
class KindArea ;
class MaxArea ;

class TrainApp {
private:
	HINSTANCE	m_hInst ;
	HACCEL		m_hAccl ;
	MainWindow	*m_pMainWnd ;			/* メイン画面 */
	StationArea	*m_pDept ;				/* 出発駅 */
	StationArea	*m_pAriv ;				/* 到着駅 */
	ResultArea	*m_pResult ;			/* 結果 */
	TimeArea	*m_pWalk ;				/* 徒歩時間 */
	TimeArea	*m_pWait ;				/* 待ち時間 */
	TimeArea	*m_pBoad ;				/* 乗車時間 */
	TimeArea	*m_pTotal ;				/* 所要時間 */
	KindArea	*m_pLimited ;			/* 特急 */
	KindArea	*m_pSuper ;				/* 新幹線 */
	KindArea	*m_pOther ;				/* その他 */
	MaxArea		*m_pMaxWalk ;			/* 最大徒歩時間 */
	MaxArea		*m_pMaxWait ;			/* 最大待ち時間 */
	MaxArea		*m_pMaxChange ;			/* 最大乗換回数 */
	TCHAR		m_path[ MAX_PATH ] ;	/* データファイルパス */
	TCHAR		m_base[ MAX_PATH ] ;	/* データファイルのベース名 */
	BOOL		m_fNeedData ;			/* データ読み込みが必要な場合*真* */
	BOOL		m_fResult ;				/* 結果表示中の場合*真* */
	StationArea	*m_pISearch ;			/* インクリメンタルサーチ中のフィールド */

	BOOL		m_fDetail ;				/* 詳細表示フラグ */
	DWORD		m_dImage ;				/* イメージ読み込みモード */
	BOOL		m_fRealTime ;			/* 実時間フラグ */

	static int		m_dDeptTime ;		/* 出発時刻 */
	static DWORD	m_dNumDetailed ;	/* 詳細モードでの項目数 */
	static DWORD	m_dNumSummary ;		/* 概要モードでの項目数 */
	static DWORD	m_dwAddLen ;		/* 結果表示領域の追加空白数 */

	BOOL		Reload() ;
	void		MakeRoute() ;

public:
	TrainApp() ;
	~TrainApp() ;

	BOOL		Create( HINSTANCE hInst ) ;
	int			Run() ;

	HINSTANCE	get_inst() const {
		return m_hInst ;
	}
	StationArea	*get_dept() { return m_pDept ; }
	StationArea	*get_ariv() { return m_pAriv ; }
	ResultArea	*get_result() { return m_pResult ; }
	TimeArea	*get_walk() { return m_pWalk ; }
	TimeArea	*get_wait() { return m_pWait ; }
	TimeArea	*get_boad() { return m_pBoad ; }
	TimeArea	*get_total() { return m_pTotal ; }
	KindArea	*get_limited() { return m_pLimited ; }
	KindArea	*get_super() { return m_pSuper ; }
	KindArea	*get_other() { return m_pOther ; }
	MaxArea		*get_maxwalk() { return m_pMaxWalk ; }
	MaxArea		*get_maxwait() { return m_pMaxWait ; }
	MaxArea		*get_maxchange() { return m_pMaxChange ; }
	BOOL		is_in_search() const { return m_pISearch != NULL ; }
	BOOL		is_in_result() const { return m_fResult ; }
	void		set_search( StationArea *sa ) { m_pISearch = sa ; }
	StationArea*	get_search() { return m_pISearch ; }
	void		set_result( BOOL f ) { m_fResult = f ; }

	void		Initialize( LPCTSTR path ) ;
	void		Confirm( BOOL fDept, BOOL fEnter ) ;
	void		Change( BOOL fDept ) ;
	void		KeyDown( BOOL fDept, DWORD nVirtKey ) ;
	void		TabKey( WORD idCtrl ) const ;
	void		ShiftTabKey( WORD idCtrl ) const ;
	void		KillFocus( WORD idCtrl ) ;
	BOOL		Search() ;
	void		SelectTermLine( BOOL fDept ) ;
	BOOL		OpenDatafile() ;
	BOOL		SaveImagefile( BOOL fForce ) ;
	BOOL		GetReg() ;
	BOOL		SetReg() const ;
	void		DeleteReg() const ;
	void		SetDetail( BOOL fDetail ) ;
	void		SetRealTime( BOOL fRealTime ) ;
	void		SelectLine( StationArea *ptr ) ;
	void		ChangeTime() ;

	static int	detailed_result_proc( int index, const BYTE **ptr, int *len ) ;
	static int	summary_result_proc( int ln, const BYTE** buf, int* len ) ;
	static int	station_proc( int index, const BYTE **ptr, int *len ) ;
	static void	set_addlen( DWORD w ) { m_dwAddLen = w ; }
} ;

extern "C" {
	extern	TCHAR	MessageBuf[ 512 ] ;
} ;
