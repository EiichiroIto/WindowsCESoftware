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
	MainWindow	*m_pMainWnd ;			/* ���C����� */
	StationArea	*m_pDept ;				/* �o���w */
	StationArea	*m_pAriv ;				/* �����w */
	ResultArea	*m_pResult ;			/* ���� */
	TimeArea	*m_pWalk ;				/* �k������ */
	TimeArea	*m_pWait ;				/* �҂����� */
	TimeArea	*m_pBoad ;				/* ��Ԏ��� */
	TimeArea	*m_pTotal ;				/* ���v���� */
	KindArea	*m_pLimited ;			/* ���} */
	KindArea	*m_pSuper ;				/* �V���� */
	KindArea	*m_pOther ;				/* ���̑� */
	MaxArea		*m_pMaxWalk ;			/* �ő�k������ */
	MaxArea		*m_pMaxWait ;			/* �ő�҂����� */
	MaxArea		*m_pMaxChange ;			/* �ő�抷�� */
	TCHAR		m_path[ MAX_PATH ] ;	/* �f�[�^�t�@�C���p�X */
	TCHAR		m_base[ MAX_PATH ] ;	/* �f�[�^�t�@�C���̃x�[�X�� */
	BOOL		m_fNeedData ;			/* �f�[�^�ǂݍ��݂��K�v�ȏꍇ*�^* */
	BOOL		m_fResult ;				/* ���ʕ\�����̏ꍇ*�^* */
	StationArea	*m_pISearch ;			/* �C���N�������^���T�[�`���̃t�B�[���h */

	BOOL		m_fDetail ;				/* �ڍו\���t���O */
	DWORD		m_dImage ;				/* �C���[�W�ǂݍ��݃��[�h */
	BOOL		m_fRealTime ;			/* �����ԃt���O */

	static int		m_dDeptTime ;		/* �o������ */
	static DWORD	m_dNumDetailed ;	/* �ڍ׃��[�h�ł̍��ڐ� */
	static DWORD	m_dNumSummary ;		/* �T�v���[�h�ł̍��ڐ� */
	static DWORD	m_dwAddLen ;		/* ���ʕ\���̈�̒ǉ��󔒐� */

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
