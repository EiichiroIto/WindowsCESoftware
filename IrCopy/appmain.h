#ifdef	_WIN32_WCE
#include	<winsock.h>
#endif	/* _WIN32_WCE */
#include	"list.h"

#define		szTitleName		TEXT("IrCopy")
#define		AppName			TEXT("IrCopy.exe")

#define		PacketSize		(240)

class MainWindow ;
class ListArea ;
class HeaderArea ;
class DateArea ;

class TransPair : public Link {
private:
	DWORD		m_dwMode ;
	LPTSTR		m_tSrcPath ;
	LPTSTR		m_tDstPath ;
	BOOL		m_fSuccess ;
	BOOL		m_fProcessed ;

public:
	TransPair( DWORD mode, LPCTSTR src, LPCTSTR dst ) ;
	~TransPair() ;

	DWORD		get_mode() const { return m_dwMode ; }
	LPCTSTR		get_srcPath() const { return m_tSrcPath ; }
	LPCTSTR		get_dstPath() const { return m_tDstPath ; }
	BOOL		get_success() const { return m_fSuccess ; }
	BOOL		get_processed() const { return m_fProcessed ; }
	void		set_success( BOOL f ) { m_fSuccess = f ; }
	void		set_processed( BOOL f ) { m_fProcessed = f ; }
	void		print() const ;

	static void			Append( List *pList, TransPair *pair ) ;
	static BOOL			ReadFromFile( List *pList, LPCTSTR fn ) ;
	static BOOL			WriteToFile( List *pList, LPCTSTR fn ) ;
	static TransPair*	GetNext( List *pList ) ;
} ;

class MainApp {
private:
	HINSTANCE	m_hInst ;
	HACCEL		m_hAccl ;
	MainWindow	*m_pMainWnd ;			/* メイン画面 */
	ListArea	*m_pListArea ;			/* 一覧表示領域 */

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
	MainApp() ;
	~MainApp() ;

	BOOL		Create( HINSTANCE hInst ) ;
	int			Run() ;

	HINSTANCE	get_inst() const { return m_hInst ; }
	ListArea	*get_list() { return m_pListArea ; }

	/* 転送ファイル関係 */
	BOOL		ReadFromFile( LPCTSTR fn ) ;
	BOOL		ReadFromFile() { return ReadFromFile( ListFn ) ; }
	BOOL		WriteToFile( LPCTSTR fn ) ;
	BOOL		WriteToFile() { return WriteToFile( ListFn ) ; }
	void		GetFromList() ;
	void		PutToList() ;
	void		InitialRead() ;
	BOOL		Import() ;
	BOOL		Export() ;

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
	void		SendWRQ( SOCKET socket, DWORD filesize, LPCTSTR path ) ;
	void		SendRRQ( SOCKET socket, LPCTSTR path ) ;
	BOOL		SendFile( SOCKET socket ) ;
	void		SendAck( SOCKET socket ) ;
	void		SendError( SOCKET socket, LPCTSTR msg ) ;

	void		RefreshList() ;

	static LPCTSTR	ListFn ;
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

extern "C" {
	extern	TCHAR	MessageBuf[ 512 ] ;
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
