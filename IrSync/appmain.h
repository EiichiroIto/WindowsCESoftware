#ifdef	_WIN32_WCE
#include	<winsock.h>
#endif	/* _WIN32_WCE */
#include	"list.h"

#define		szTitleName		TEXT("IrSync")
#define		szClassName		TEXT("IRSYNC")

#define		PacketSize		(240)

extern "C" {
	extern	TCHAR	MessageBuf[ 512 ] ;
} ;

class MainWindow ;
class TransPair ;

class MainApp {
private:
	HINSTANCE	m_hInst ;
	HACCEL		m_hAccl ;
	MainWindow	*m_pMainWnd ;				/* メイン画面 */
	TCHAR		m_szSrcDir[ MAX_PATH ] ;

public:
	MainApp() ;
	~MainApp() ;

	BOOL		Create( HINSTANCE hInst ) ;
	int			Run() ;
	BOOL		GetReg() ;
	BOOL		SetReg() const ;
	void		DeleteReg() const ;

	HINSTANCE	get_inst() const { return m_hInst ; }
	LPTSTR		GetSrcDir() { return m_szSrcDir ; }
	void		SetSrcDir( LPTSTR buf ) { _tcscpy( m_szSrcDir, buf ) ; }

private:
	/* 転送ファイル関係 */
	List		m_oTransList ;
	TransPair	*m_pCurPair ;

	/* Winsock関係 */
	int			m_fWinsock ;
	WSADATA		m_WsaData ;
	WORD		m_wMode ;
	WORD		m_wBlock ;
	TCHAR		m_tPath[ MAX_PATH ] ;
	HANDLE		m_hFile ;
	BOOL		m_fError ;
	BOOL		m_fAbort ;
	FILETIME	m_ftWrite ;

	u_short		get_word( char *buf ) const {
		u_short	tmp = 0xFF & ((u_short)(buf)[0]) ;
		tmp = tmp * 256 + 0xFF & ((u_short)(buf)[1]) ;
		return tmp ;
	}
	u_long		get_dword( char *buf ) const {
		u_long	tmp = 0xFF & ((u_long) (buf)[0]) ;
		tmp = tmp * 256 + (0xFF & ((u_long) (buf)[1])) ;
		tmp = tmp * 256 + (0xFF & ((u_long) (buf)[2])) ;
		tmp = tmp * 256 + (0xFF & ((u_long) (buf)[3])) ;
		return tmp ;
	}

public:
	/* 転送ダイアログ関係 */
	BOOL		StartTransfer( DWORD mode, HWND hDlg, HWND hWndList, HWND hWndMsg ) ;
	void		AbortTransfer() ;

	/* サーバー処理関係 */
	BOOL		ServerMain( BOOL fOnce ) ;
	BOOL		ServerLoop( SOCKET clSocket ) ;

	/* クライアント処理関係 */
	BOOL		ClientMain() ;
	BOOL		PutFile( SOCKET clSocket, LPCTSTR src, LPCTSTR dst ) ;
	BOOL		GetFile( SOCKET clSocket, LPCTSTR src, LPTSTR dst ) ;

	void		WriteFile( LPBYTE buf, DWORD size ) ;
	void		CloseFile() ;
	void		SendWRQ( SOCKET socket, DWORD filesize, LPCTSTR path, LPFILETIME pFt ) ;
	void		SendRRQ( SOCKET socket, LPCTSTR path ) ;
	BOOL		SendFile( SOCKET socket ) ;
	void		SendAck( SOCKET socket ) ;
	void		SendError( SOCKET socket, LPCTSTR msg ) ;

	BOOL		MakeTransPair( LPCTSTR szSrcDir, LPCTSTR szDstDir ) ;

	/* 転送ダイアログ関係 */
	static MainApp	*g_pApp ;
	static HWND		g_hWndDlg ;
	static HWND		g_hWndList ;
	static HWND		g_hWndMsg ;
	/* Winsock関係 */
	static SOCKET	g_svSocket ;
	static SOCKET	g_clSocket ;
	static char		g_SendBuf[ PacketSize + 10 ] ;
	static char		g_RecvBuf[ PacketSize + 10 ] ;
	static int		g_SendSize ;

	static void	StartServer( DWORD mode ) ;
	static void	NotifyMessage( LPCTSTR message, DWORD code ) ;
	static void	AddTransferList( BOOL fPut, LPCTSTR path ) ;
} ;

class TransPair : public Link {
private:
	LPTSTR		m_tSrcPath ;
	LPTSTR		m_tDstPath ;
	BOOL		m_bProcessed ;

public:
	TransPair( LPCTSTR src, LPCTSTR dst ) ;
	~TransPair() ;

	LPCTSTR		get_srcPath() const { return m_tSrcPath ; }
	LPCTSTR		get_dstPath() const { return m_tDstPath ; }
	BOOL		get_processed() const { return m_bProcessed ; }
	void		set_processed( BOOL f ) { m_bProcessed = f ; }

	static void			Append( List *pList, TransPair *pair ) ;
	static TransPair*	GetNext( List *pList ) ;
} ;

class DirLink : public Link {
private:
	LPTSTR		m_tDir ;

public:
	DirLink( LPCTSTR dir ) ;
	~DirLink() ;

	void	get( LPTSTR src ) { _tcscpy( src, m_tDir ) ; }
} ;

#define		TFTP_RRQ			0x01
#define		TFTP_WRQ			0x02
#define		TFTP_DATA			0x03
#define		TFTP_ACK			0x04
#define		TFTP_ERROR			0x05

#define		SVMODE_IDLE			0x00
#define		SVMODE_READ			0x01
#define		SVMODE_READ_LAST	0x02
#define		SVMODE_WRITE		0x03

#define		CLMODE_WRQ			0x00
#define		CLMODE_LAST			0x01
#define		CLMODE_RRQ			0x02
#define		CLMODE_WRITE		0x03

#define		TRANS_EXECUTE		0
#define		TRANS_SEND			1
#define		TRANS_RECEIVE		2
