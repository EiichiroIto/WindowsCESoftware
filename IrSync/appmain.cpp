#ifdef	_WIN32_WCE
#define	USE_IRDA
#endif	/* _WIN32_WCE */
#define	STRICT
#include	<windows.h>
#include	<windowsx.h>
#include	<commctrl.h>
#include	<tchar.h>
#ifdef	USE_IRDA
#include	<af_irda.h>
#endif	/* USE_IRDA */
#include	"tools.h"
#include	"resource.h"
#include	"appmain.h"
#include	"appsub.h"

TCHAR	MessageBuf[ 512 ] ;
BOOL	mkdir_recursive( LPCTSTR dir ) ;

/*-------------------------------------------------------------------------*
 * MainApp
 *-------------------------------------------------------------------------*/
MainApp::MainApp()
{
	m_pMainWnd = 0 ;
	m_fWinsock = WSAStartup( MAKEWORD(1, 1), &m_WsaData ) ;
	m_hFile = INVALID_HANDLE_VALUE ;
	_tcscpy( m_szSrcDir, TEXT("\\") ) ;
}

MainApp::~MainApp()
{
}

BOOL
MainApp::Create( HINSTANCE hInstance )
{
	m_hInst = hInstance ;

	if ( !MainWindow::RegisterClass( m_hInst ) ) {
		return FALSE ;
	}

#ifdef	_WIN32_WCE
	m_hAccl = LoadAccelerators( m_hInst,
								MAKEINTRESOURCE(IDR_ACCELERATOR1) ) ;
#else	/* _WIN32_WCE */
	m_hAccl = LoadAccelerators( GetModuleHandle(NULL),
								MAKEINTRESOURCE(IDR_ACCELERATOR1) ) ;

#endif	/* _WIN32_WCE */

	GetReg() ;

	m_pMainWnd = new MainWindow( this ) ;
	if ( !m_pMainWnd ) {
		return FALSE ;
	}
	if ( !m_pMainWnd->Create() ) {
		return FALSE ;
	}

	/* 画面を前面にする */
	SetForegroundWindow( m_pMainWnd->get_hwnd() ) ;
	return TRUE ;
}

int
MainApp::Run()
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

#define		REG_BASEKEY		TEXT("Software\\Gawaro\\")
#define		VAR_SRCDIR		TEXT("SrcDir")

/*
 * レジストリからの情報取得
 */
BOOL
MainApp::GetReg()
{
	HKEY	hk ;
	LONG	lret ;
	LPTSTR	name ;
	LPBYTE	lpData ;
	DWORD	dwType, cbData ;
	TCHAR	subkey[ MAX_PATH ] ;

	wsprintf( subkey, TEXT("%s%s"), REG_BASEKEY, szTitleName ) ;
	/* レジストリキーをオープンする */
	lret = RegOpenKeyEx( HKEY_CURRENT_USER, subkey, 0, KEY_QUERY_VALUE, &hk ) ;
	if ( lret != ERROR_SUCCESS ) {
		return FALSE ;
	}
	/* 同期ディレクトリ名 */
	name = VAR_SRCDIR ;
	dwType = REG_SZ ;
	lpData = (LPBYTE) m_szSrcDir ;
	cbData = sizeof m_szSrcDir ;
	lret = RegQueryValueEx( hk, name, NULL, &dwType, lpData, &cbData ) ;
	/* レジストリをクローズする */
	RegCloseKey( hk ) ;
	return TRUE ;
}

/*
 * レジストリへの情報設定
 */
BOOL
MainApp::SetReg() const
{
	HKEY	hk ;
	LONG	lret ;
	LPTSTR	name ;
	LPBYTE	lpData ;
	TCHAR	subkey[ MAX_PATH ] ;
	DWORD	ret, dwType, cbData ;

	wsprintf( subkey, TEXT("%s%s"), REG_BASEKEY, szTitleName ) ;
	/* レジストリキーを作成する */
	lret = RegCreateKeyEx( HKEY_CURRENT_USER, subkey, 0, TEXT(""), 0,
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
	name = VAR_SRCDIR ;
	dwType = REG_SZ ;
	lpData = (LPBYTE) m_szSrcDir ;
	cbData = (_tcslen(m_szSrcDir) + 1) * sizeof (*m_szSrcDir) ;
	lret = RegSetValueEx( hk, name, 0, dwType, lpData, cbData ) ;
	/* レジストリをクローズする */
	RegCloseKey( hk ) ;
	return TRUE ;
}

/*
 * レジストリ情報削除
 */
void
MainApp::DeleteReg() const
{
	TCHAR	subkey[ MAX_PATH ] ;
	wsprintf( subkey, TEXT("%s%s"), REG_BASEKEY, szTitleName ) ;
	RegDeleteKey( HKEY_CURRENT_USER, subkey ) ;
}

MainApp		*MainApp::g_pApp = 0 ;
HWND		MainApp::g_hWndDlg = 0 ;
HWND		MainApp::g_hWndList = 0 ;
HWND		MainApp::g_hWndMsg = 0 ;
SOCKET		MainApp::g_svSocket = 0 ;
SOCKET		MainApp::g_clSocket = 0 ;
char		MainApp::g_SendBuf[ PacketSize + 10 ] ;
char		MainApp::g_RecvBuf[ PacketSize + 10 ] ;
int			MainApp::g_SendSize = 0 ;

BOOL
MainApp::StartTransfer( DWORD mode, HWND hDlg, HWND hWndList, HWND hWndMsg )
{
	HANDLE		hThread ;
	DWORD		threadID ;

	m_fAbort = FALSE ;
	g_pApp = this ;
	g_hWndDlg = hDlg ;
	g_hWndList = hWndList ;
	g_hWndMsg = hWndMsg ;
	hThread = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE) StartServer, (void*) mode, 0, &threadID ) ;
	if ( hThread == NULL ) {
		return FALSE ;
	}
	CloseHandle( hThread ) ;
	return TRUE ;
}

void
MainApp::AbortTransfer()
{
	m_fAbort = TRUE ;
	g_hWndDlg = 0 ;
	g_hWndList = 0 ;
	g_hWndMsg = 0 ;
}

void
MainApp::StartServer( DWORD mode )
{
	BOOL	rc = FALSE ;
	BOOL	fOnce = mode == TRANS_EXECUTE ;

	g_clSocket = 0 ;
	/* ソケットをオープンする */
#ifdef	USE_IRDA
	g_svSocket = socket( AF_IRDA, SOCK_STREAM, NULL ) ;
#else	/* USE_IRDA */
	g_svSocket = socket( PF_INET, SOCK_STREAM, NULL ) ;
#endif	/* USE_IRDA */
	if ( g_svSocket == INVALID_SOCKET ) {
		NotifyMessage( TEXT("socket failed(%d)"), GetLastError() ) ;
	} else {
		if ( g_pApp ) {
//Fopen( TEXT("\\log"), TEXT("wb") ) ;
			if ( mode == TRANS_RECEIVE ) {
				rc = g_pApp->ServerMain( FALSE ) ;
			} else if ( mode == TRANS_SEND ) {
				rc = g_pApp->ClientMain() ;
			} else if ( mode == TRANS_EXECUTE ) {
				rc = g_pApp->ClientMain() ;
				if ( !rc ) {
					rc = g_pApp->ServerMain( TRUE ) ;
				}
				NotifyMessage( TEXT(""), GetLastError() ) ;
			}
//Fclose() ;
		}
		closesocket( g_svSocket ) ;
	}
	/* エラー発生時はダイアログに伝える */
	if ( g_hWndDlg ) {
		if ( !rc ) {
			SendMessage( g_hWndDlg, WM_COMMAND, IDM_ERROR, 0 ) ;
		} else if ( mode != TRANS_RECEIVE ) {
			SendMessage( g_hWndDlg, WM_COMMAND, IDM_END, 0 ) ;
		}
	}
}

BOOL
MainApp::ServerMain( BOOL fOnce )
{
	int				status ;
#ifdef	USE_IRDA
	SOCKADDR_IRDA	svSockAddr ;
#else	/* USE_IRDA */
	int				clAddrLen ;
	SOCKADDR_IN		svSockAddr ;
	SOCKADDR_IN		clSockAddr ;
#endif	/* USE_IRDA */

	memset( &svSockAddr, 0, sizeof (svSockAddr) ) ;
#ifdef	USE_IRDA
	svSockAddr.irdaAddressFamily = AF_IRDA ;
	svSockAddr.irdaDeviceID[0] = 0 ;
	strcpy( svSockAddr.irdaServiceName, "IrCopyServer" ) ;
#else	/* USE_IRDA */
	svSockAddr.sin_port = htons(44965) ;
	svSockAddr.sin_family = AF_INET ;
	svSockAddr.sin_addr.s_addr = htonl(INADDR_ANY) ;
#endif	/* USE_IRDA */
	/* bind を行う */
	status = bind( g_svSocket, (LPSOCKADDR) &svSockAddr, sizeof (svSockAddr) ) ;
	if ( status == SOCKET_ERROR ) {
		NotifyMessage( TEXT("bind failed(%d)"), GetLastError() ) ;
		return FALSE ;
	}
	/* listen を行う */
	status = listen( g_svSocket, 1 ) ;
	if ( status == SOCKET_ERROR ) {
		NotifyMessage( TEXT("listen failed(%d)"), GetLastError() ) ;
		return FALSE ;
	}
	NotifyMessage( TEXT("Waiting for Client Connection..."), 0 ) ;
	do {
#ifdef	_WIN32_WCE
		timeval			tv ;
		fd_set			readfds ;

		tv.tv_sec = 1 ;		tv.tv_usec = 0 ;
		FD_ZERO( &readfds ) ;
		FD_SET( g_svSocket, &readfds ) ;
		int ret = select( 0, &readfds, NULL, NULL, &tv ) ;
		if ( !ret ) {
			continue ;
		} else if ( m_fAbort ) {
			break ;
		}
#endif	/* _WIN32_WCE */
#ifdef	USE_IRDA
		g_clSocket = accept( g_svSocket, NULL, NULL ) ;
#else	/* USE_IRDA */
		clAddrLen = sizeof (clSockAddr) ;
		g_clSocket = accept( g_svSocket, (LPSOCKADDR) &clSockAddr, &clAddrLen ) ;
#endif	/* USE_IRDA */
		if ( g_clSocket == INVALID_SOCKET ) {
			NotifyMessage( TEXT("accept failed(%d)"), WSAGetLastError() ) ;
			return FALSE ;
		}
#ifdef	_WIN32_WCE
		sndPlaySound( TEXT("\\Windows\\INFBEG.WAV"), SND_FILENAME ) ;
#endif	/* _WIN32_WCE */
		if ( !ServerLoop( g_clSocket ) ) {
			NotifyMessage( TEXT("server loop exit"), 0 ) ;
			break ;
		}
#ifdef	_WIN32_WCE
		sndPlaySound( TEXT("\\Windows\\INFEND.WAV"), SND_FILENAME ) ;
#endif	/* _WIN32_WCE */
		g_clSocket = 0 ;
		if ( fOnce ) {
			break ;
		}
	} while ( !m_fAbort ) ;
	return TRUE ;
}

BOOL
MainApp::ServerLoop( SOCKET clSocket )
{
	FILETIME	ft ;
	WORD		cmd ;
	int			num_recv ;
	TCHAR		buf[ MAX_PATH ] ;
	DWORD		len, total, rest, tmp ;

	total = rest = 0 ;
	m_pCurPair = NULL ;
	m_wMode = SVMODE_IDLE ;
	while ( !m_fAbort ) {
		memset( g_RecvBuf, 0, sizeof g_RecvBuf ) ;
		num_recv = recv( clSocket, g_RecvBuf, sizeof g_RecvBuf, 0 ) ;
		if ( num_recv == SOCKET_ERROR ) {
#ifdef	_WIN32_WCE
			sndPlaySound( TEXT("\\Windows\\INFINTR.WAV"), SND_FILENAME ) ;
#endif	/* _WIN32_WCE */
			NotifyMessage( TEXT("recv failed(%d)"), WSAGetLastError() ) ;
			break ;
		} else if ( !num_recv ) {
			break ;
		}
//Fwrite( g_RecvBuf, num_recv ) ;
//Fputs( "!!" ) ;
		cmd = get_word( &g_RecvBuf[0] ) ;
		switch ( m_wMode ) {
		case SVMODE_WRITE:
			if ( m_hFile == INVALID_HANDLE_VALUE ) {
				m_wMode = SVMODE_IDLE ;
				break ;
			}
			len = num_recv ;
			WriteFile( (LPBYTE) g_RecvBuf, len ) ;
			if ( rest > len ) {
				rest -= len ;
				wsprintf( MessageBuf, TEXT("Receiving:%d/%d"), total - rest, total ) ;
				NotifyMessage( MessageBuf, 0 ) ;
			} else {
				rest = 0 ;
				if ( m_ftWrite.dwLowDateTime || m_ftWrite.dwHighDateTime ) {
					SetFileTime( m_hFile, &m_ftWrite, &m_ftWrite, &m_ftWrite ) ;
				}
				CloseFile() ;
				SendAck( clSocket ) ;
				NotifyMessage( TEXT("Done."), 0 ) ;
				m_wMode = SVMODE_IDLE ;
			}
			break ;
		case SVMODE_READ:
			if ( cmd != TFTP_ACK || m_hFile == INVALID_HANDLE_VALUE ) {
				m_wMode = SVMODE_IDLE ;
				break ;
			}
			while ( SendFile( clSocket ) ) {
				rest += PacketSize ;
				wsprintf( MessageBuf, TEXT("Sending:%d/%d"), rest, total ) ;
				NotifyMessage( MessageBuf, 0 ) ;
			}
			/* ファイルの終わりに到達した場合 */
			CloseFile() ;
			m_wMode = SVMODE_READ_LAST ;
			break ;
		case SVMODE_READ_LAST:
			if ( cmd == TFTP_ACK && m_pCurPair ) {
//				m_pCurPair->set_success( TRUE ) ;
			}
			m_pCurPair = NULL ;
			NotifyMessage( TEXT("Done."), 0 ) ;
			m_wMode = SVMODE_IDLE ;
			break ;
		default:
			if ( cmd == TFTP_WRQ ) {
				rest = total = get_dword( &g_RecvBuf[2] ) ;
				NotifyMessage( TEXT("total=%d"), total ) ;
#ifdef	UNICODE
				mbstowcs( m_tPath, &g_RecvBuf[6], sizeof m_tPath / sizeof (TCHAR) ) ;
#ifdef	_WIN32_WCE
				LPCTSTR		ptr = _tcschr( m_tPath, TEXT(':') ) ;
				if ( ptr ) {
					_tcscpy( m_tPath, ptr + 1 ) ;
				}
#endif	/* _WIN32_WCE */
#else	/* UNICODE */
				strcpy( m_tPath, &g_RecvBuf[6] ) ;
#endif	/* UNICODE */
				/* ローカルのディレクトリに変更する */
				_tcscpy( buf, m_szSrcDir ) ;
				catdir( buf, m_tPath, NULL ) ;
				_tcscpy( m_tPath, buf ) ;
				mkdir_recursive( m_tPath ) ;
				/* 日付情報を取り出す */
				tmp = 6 + strlen( &g_RecvBuf[6] ) + 1 ;
				num_recv -= tmp ;
				if ( num_recv ) {
					m_ftWrite.dwLowDateTime = get_dword( &g_RecvBuf[tmp] ) ;
					m_ftWrite.dwHighDateTime = get_dword( &g_RecvBuf[tmp+4] ) ;
				} else {
					m_ftWrite.dwLowDateTime = m_ftWrite.dwHighDateTime = 0 ;
				}
				AddTransferList( FALSE, m_tPath ) ;
				CloseFile() ;
				m_hFile = CreateFile( m_tPath, GENERIC_WRITE, FILE_SHARE_READ, NULL,
									  CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0 ) ;
				if ( m_hFile == INVALID_HANDLE_VALUE ) {
					SendError( clSocket, TEXT("File not found") ) ;
					wsprintf( MessageBuf, TEXT("Can't create file:%s(%d)"), m_tPath, GetLastError() ) ;
					NotifyMessage( MessageBuf, 0 ) ;
					m_wMode = SVMODE_IDLE ;
					break ;
				}
				SendAck( clSocket ) ;
				if ( !rest ) {
					/* ファイルサイズが０の場合はそのまま終了 */
					CloseFile() ;
					m_wMode = SVMODE_IDLE ;
					break ;
				}
				m_wMode = SVMODE_WRITE ;
			} else if ( cmd == TFTP_RRQ ) {
#ifdef	UNICODE
				mbstowcs( m_tPath, &g_RecvBuf[2], sizeof m_tPath / sizeof (TCHAR) ) ;
#else	/* UNICODE */
				strcpy( m_tPath, &g_RecvBuf[2] ) ;
#endif	/* UNICODE */
				m_pCurPair = NULL ;
				if ( !*m_tPath ) {
					/* ファイル名の指定がない場合 */
					m_pCurPair = TransPair::GetNext( &m_oTransList ) ;
					if ( !m_pCurPair ) {
						/* 全ファイルの転送が終了した場合 */
						SendAck( clSocket ) ;
						continue ;
					}
					m_pCurPair->set_processed( TRUE ) ;
					/* ファイル名を取得する */
					_tcscpy( m_tPath, m_pCurPair->get_srcPath() ) ;
				}
				AddTransferList( TRUE, m_tPath ) ;
				CloseFile() ;
				m_hFile = CreateFile( m_tPath, GENERIC_READ, FILE_SHARE_READ, NULL,
									  OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 ) ;
				if ( m_hFile == INVALID_HANDLE_VALUE ) {
					SendError( clSocket, TEXT("File not found") ) ;
					wsprintf( MessageBuf, TEXT("Can't open file:%s(%d)"), m_tPath, GetLastError() ) ;
					NotifyMessage( MessageBuf, 0 ) ;
					m_wMode = SVMODE_IDLE ;
					break ;
				}
				/* ファイルサイズを取得する */
				total = GetFileSize( m_hFile, NULL ) ;
				rest = 0 ;
				NotifyMessage( TEXT("total=%d"), total ) ;
				/* ファイルの書き込み日付を取得する */
				if ( !GetFileTime( m_hFile, NULL, NULL, &ft ) ) {
					memset( &ft, 0, sizeof ft ) ;
				}
				/* ファイル名の転送を行う */
				SendWRQ( clSocket, total, m_pCurPair->get_dstPath(), &ft ) ;
				m_wMode = SVMODE_READ ;
			}
			break ;
		}
	}
	CloseFile() ;
	closesocket( clSocket ) ;
//debug( TEXT("close") ) ;
	return TRUE ;
}

BOOL
MainApp::ClientMain()
{
	int				status ;
#ifdef	USE_IRDA
	SOCKADDR_IRDA	svSockAddr ;
	DEVICELIST		dl ;
	int				dwOptLen ;
#else	/* USE_IRDA */
	SOCKADDR_IN		svSockAddr ;
#endif	/* USE_IRDA */

	NotifyMessage( TEXT("Searching Server..."), 0 ) ;
	memset( &svSockAddr, 0, sizeof (svSockAddr) ) ;
#ifdef	USE_IRDA
	memset( &dl, 0, sizeof dl ) ;
	dwOptLen = sizeof dl ;
#ifdef	_WIN32_WCE
	if ( getsockopt( g_svSocket, SOL_IRLMP, IRLMP_ENUMDEVICES, (char*) &dl, &dwOptLen ) ) {
		NotifyMessage( TEXT("getsockopt failed(%d)"), WSAGetLastError() ) ;
		return FALSE ;
	}
#else	/* _WIN32_WCE */
	int		mode = TRUE ;
	setsockopt( g_svSocket, SOL_IRLMP, IRLMP_DISCOVERY_MODE, (const char*) &mode, sizeof mode ) ;
	recv( g_svSocket, (char*) &dl, sizeof dl, 0 ) ;
	mode = FALSE ;
	setsockopt( g_svSocket, SOL_IRLMP, IRLMP_DISCOVERY_MODE, (const char*) &mode, sizeof mode ) ;
#endif	/* _WIN32_WCE */
	if ( !*dl.Device[0].irdaDeviceName ) {
		return FALSE ;
	}
	_tcscpy( MessageBuf, TEXT("Server:") ) ;
#ifdef	UNICODE
	mbstowcs( &MessageBuf[7], dl.Device[0].irdaDeviceName, 30 ) ;
#else	/* UNICODE */
	strcpy( &MessageBuf[7], dl.Device[0].irdaDeviceName ) ;
#endif	/* UNICODE */
	NotifyMessage( MessageBuf, 0 ) ;
	svSockAddr.irdaAddressFamily = AF_IRDA ;
	memcpy( svSockAddr.irdaDeviceID, dl.Device[0].irdaDeviceID, 4 ) ;
	strcpy( svSockAddr.irdaServiceName, "IrCopyServer" ) ;
#else	/* USE_IRDA */
	svSockAddr.sin_port = htons(44965) ;
	svSockAddr.sin_family = AF_INET ;
	svSockAddr.sin_addr.s_addr = inet_addr("127.0.0.1") ;
#endif	/* USE_IRDA */
	/* connect を行う */
	status = connect( g_svSocket, (LPSOCKADDR) &svSockAddr, sizeof (svSockAddr) ) ;
	if ( status == SOCKET_ERROR ) {
		NotifyMessage( TEXT("connect failed(%d)"), WSAGetLastError() ) ;
		return FALSE ;
	}
	TransPair	*cur ;
	Iterator	l( m_oTransList ) ;
	while ( cur = (TransPair*) l() ) {
		LPCTSTR		pSrc = cur->get_srcPath() ;
		LPCTSTR		pDst = cur->get_dstPath() ;
		if ( !pSrc || !pDst ) {
			break ;
		}
		BOOL		rc = PutFile( g_svSocket, pSrc, pDst ) ;
		cur->set_processed( TRUE ) ;
	}
	TCHAR		path[ MAX_PATH ] ;
	do {
		*path = 0 ;
		GetFile( g_svSocket, TEXT(""), path ) ;
	} while ( *path ) ;
	return TRUE ;
}

BOOL
MainApp::PutFile( SOCKET clSocket, LPCTSTR src, LPCTSTR dst )
{
	FILETIME	ft ;
	WORD		cmd ;
	int			num_recv ;
	DWORD		size, total ;
	BOOL		fQuit = FALSE, ret = FALSE ;

	AddTransferList( TRUE, src ) ;
	m_hFile = CreateFile( src, GENERIC_READ, FILE_SHARE_READ, NULL,
						  OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 ) ;
	if ( m_hFile == INVALID_HANDLE_VALUE ) {
		return FALSE ;
	}
	total = GetFileSize( m_hFile, NULL ) ;
	size = 0 ;
	if ( !GetFileTime( m_hFile, NULL, NULL, &ft ) ) {
		memset( &ft, 0, sizeof ft ) ;
	}
	SendWRQ( clSocket, total, dst, &ft ) ;
	m_wMode = CLMODE_WRQ ;
	do {
		memset( g_RecvBuf, 0, sizeof g_RecvBuf ) ;
		num_recv = recv( clSocket, g_RecvBuf, sizeof g_RecvBuf, 0 ) ;
		if ( num_recv == SOCKET_ERROR ) {
#ifdef	_WIN32_WCE
			sndPlaySound( TEXT("\\Windows\\INFINTR.WAV"), SND_FILENAME ) ;
#endif	/* _WIN32_WCE */
			NotifyMessage( TEXT("recv failed(%d)"), WSAGetLastError() ) ;
			break ;
		} else if ( !num_recv ) {
			break ;
		}
		cmd = get_word( &g_RecvBuf[0] ) ;
		switch ( m_wMode ) {
		case CLMODE_WRQ:
			if ( cmd != TFTP_ACK ) {
				fQuit = TRUE ;
				break ;
			}
			if ( !total ) {
				/* ファイルサイズが０の場合はそのまま終了 */
				ret = TRUE ;
				fQuit = TRUE ;
				break ;
			}
			while ( SendFile( clSocket ) ) {
				size += PacketSize ;
				wsprintf( MessageBuf, TEXT("Sending:%d/%d"), size, total ) ;
				NotifyMessage( MessageBuf, 0 ) ;
			}
			/* ファイルの終わりに到達した場合 */
			CloseFile() ;
			m_wMode = CLMODE_LAST ;
			break ;
		case CLMODE_LAST:
			ret = TRUE ;
			fQuit = TRUE ;
			NotifyMessage( TEXT("Done."), 0 ) ;
			break ;
		default:
			NotifyMessage( TEXT("Invalid Mode"), 0 ) ;
			fQuit = TRUE ;
			break ;
		}
	} while ( !fQuit ) ;
	CloseFile() ;
	return ret ;
}

BOOL
MainApp::GetFile( SOCKET clSocket, LPCTSTR src, LPTSTR dst )
{
	WORD		cmd ;
	int			num_recv ;
	TCHAR		buf[ MAX_PATH ] ;
	DWORD		len, total, rest, tmp ;
	BOOL		fQuit = FALSE, ret = FALSE ;

	total = rest = 0 ;
	SendRRQ( clSocket, src ) ;
	m_wMode = CLMODE_RRQ ;
	m_fError = FALSE ;
	do {
		memset( g_RecvBuf, 0, sizeof g_RecvBuf ) ;
		num_recv = recv( clSocket, g_RecvBuf, sizeof g_RecvBuf, 0 ) ;
		if ( !num_recv || num_recv == SOCKET_ERROR ) {
			break ;
		}
//Fwrite( g_RecvBuf, num_recv ) ;
//Fputs( "!!" ) ;
		cmd = get_word( &g_RecvBuf[0] ) ;
		switch ( m_wMode ) {
		case CLMODE_RRQ:
			if ( cmd != TFTP_WRQ ) {
				if ( cmd == TFTP_ACK ) {
					/* 転送すべきものがない場合 */
					ret = TRUE ;
				} else if ( cmd == TFTP_ERROR ) {
#ifdef	UNICODE
					mbstowcs( MessageBuf, &g_RecvBuf[2], sizeof MessageBuf / sizeof (TCHAR) ) ;
#else	/* UNICODE */
					strcpy( MessageBuf, &g_RecvBuf[2] ) ;
#endif	/* UNICODE */
					NotifyMessage( MessageBuf, 0 ) ;
				} else {
					NotifyMessage( TEXT("Invalid Command:%d"), cmd ) ;
				}
				fQuit = TRUE ;
				break ;
			}
			/* パス名を取得する */
			if ( !*dst ) {
#ifdef	UNICODE
				mbstowcs( dst, &g_RecvBuf[6], MAX_PATH ) ;
#ifdef	_WIN32_WCE
				LPTSTR		ptr = _tcschr( dst, TEXT(':') ) ;
				if ( ptr ) {
					dst = ptr + 1 ;
				}
#endif	/* _WIN32_WCE */
#else	/* UNICODE */
				strcpy( dst, &g_RecvBuf[6] ) ;
#endif	/* UNICODE */
			}
			_tcscpy( m_tPath, dst ) ;
			/* ローカルのディレクトリに変更する */
			_tcscpy( buf, m_szSrcDir ) ;
			catdir( buf, m_tPath, NULL ) ;
			_tcscpy( m_tPath, buf ) ;
			mkdir_recursive( m_tPath ) ;
			/* ファイルサイズを取得する */
			rest = total = get_dword( &g_RecvBuf[2] ) ;
			/* ファイル日付を取得する */
			tmp = 6 + strlen( &g_RecvBuf[6] ) + 1 ;
			num_recv -= tmp ;
			if ( num_recv ) {
				m_ftWrite.dwLowDateTime = get_dword( &g_RecvBuf[tmp] ) ;
				m_ftWrite.dwHighDateTime = get_dword( &g_RecvBuf[tmp+4] ) ;
			} else {
				m_ftWrite.dwLowDateTime = m_ftWrite.dwHighDateTime = 0 ;
			}
			AddTransferList( FALSE, m_tPath ) ;
			m_hFile = CreateFile( m_tPath, GENERIC_WRITE, FILE_SHARE_READ, NULL,
								  CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0 ) ;
			if ( m_hFile == INVALID_HANDLE_VALUE ) {
				SendError( clSocket, TEXT("File not found") ) ;
				wsprintf( MessageBuf, TEXT("Can't create file:%s(%d)"), m_tPath, GetLastError() ) ;
				NotifyMessage( MessageBuf, 0 ) ;
				fQuit = TRUE ;
				break ;
			}
			SendAck( clSocket ) ;
			if ( !rest ) {
				/* ファイルサイズが０の場合はそのまま終了 */
				CloseFile() ;
				ret = TRUE ;
				fQuit = TRUE ;
				break ;
			}
			m_wMode = CLMODE_WRITE ;
			break ;
		case CLMODE_WRITE:
			if ( m_hFile == INVALID_HANDLE_VALUE ) {
				fQuit = TRUE ;
				break ;
			}
			len = num_recv ;
			WriteFile( (LPBYTE) g_RecvBuf, len ) ;
			if ( rest > len ) {
				rest -= len ;
				wsprintf( MessageBuf, TEXT("Receiving:%d/%d"), total - rest, total ) ;
				NotifyMessage( MessageBuf, 0 ) ;
			} else {
				rest = 0 ;
				if ( m_ftWrite.dwLowDateTime || m_ftWrite.dwHighDateTime ) {
					SetFileTime( m_hFile, &m_ftWrite, &m_ftWrite, &m_ftWrite ) ;
				}
				CloseFile() ;
				SendAck( clSocket ) ;
				NotifyMessage( TEXT("Done."), 0 ) ;
				/* 正常終了 */
				ret = TRUE ;
				fQuit = TRUE ;
			}
			break ;
		default:
			NotifyMessage( TEXT("Invalid Mode"), 0 ) ;
			fQuit = TRUE ;
			break ;
		}
	} while ( !fQuit ) ;
	CloseFile() ;
	return ret ;
}

void
MainApp::WriteFile( LPBYTE buf, DWORD size )
{
	BOOL	rc ;
	DWORD	nWrite ;

	if ( size && m_hFile != INVALID_HANDLE_VALUE ) {
		rc = ::WriteFile( m_hFile, buf, size, &nWrite, NULL ) ;
		if ( !rc || size != nWrite ) {
			m_fError = TRUE ;
			CloseHandle( m_hFile ) ;
			DeleteFile( m_tPath ) ;
			m_hFile = INVALID_HANDLE_VALUE ;
		}
	}
}

void
MainApp::CloseFile()
{
	if ( m_hFile != INVALID_HANDLE_VALUE ) {
		CloseHandle( m_hFile ) ;
		m_hFile = INVALID_HANDLE_VALUE ;
	}
}

void
MainApp::SendWRQ( SOCKET socket, DWORD filesize, LPCTSTR path, LPFILETIME pFt )
{
	DWORD		tmp ;
	char		*ptr ;
	u_short		cmd = TFTP_WRQ ;

	/* コマンドをセットする */
	g_SendBuf[0] = (char) (cmd / 256) ;
	g_SendBuf[1] = (char) (cmd % 256) ;
	/* ファイルサイズをセットする */
	g_SendBuf[2] = (char) ((filesize >> 24) & 0xFF) ;
	g_SendBuf[3] = (char) ((filesize >> 16) & 0xFF) ;
	g_SendBuf[4] = (char) ((filesize >> 8) & 0xFF) ;
	g_SendBuf[5] = (char) (filesize & 0xFF) ;
	ptr = &g_SendBuf[6] ;
	/* ファイル名をセットする */
#ifdef	UNICODE
	wcstombs( ptr, path, sizeof g_SendBuf - 6 ) ;
#else	/* UNICODE */
	strcpy( ptr, path ) ;
#endif	/* UNICODE */
	tmp = strlen( ptr ) + 1 ;
	ptr += tmp ;
	/* ファイルの書き込み日付をセットする */
	*ptr++ = (char) ((pFt->dwLowDateTime >>  24) & 0xFF) ;
	*ptr++ = (char) ((pFt->dwLowDateTime >>  16) & 0xFF) ;
	*ptr++ = (char) ((pFt->dwLowDateTime >>   8) & 0xFF) ;
	*ptr++ = (char) ((pFt->dwLowDateTime & 0xFF) & 0xFF) ;
	*ptr++ = (char) ((pFt->dwHighDateTime >>  24) & 0xFF) ;
	*ptr++ = (char) ((pFt->dwHighDateTime >>  16) & 0xFF) ;
	*ptr++ = (char) ((pFt->dwHighDateTime >>   8) & 0xFF) ;
	*ptr++ = (char) ((pFt->dwHighDateTime & 0xFF) & 0xFF) ;
	/* 送信パケットのサイズを求める */
	g_SendSize = 6 + tmp + 8 ;
	send( socket, g_SendBuf, g_SendSize, 0 ) ;
}

void
MainApp::SendRRQ( SOCKET socket, LPCTSTR path )
{
	char		*ptr ;
	u_short		cmd = TFTP_RRQ ;

	g_SendBuf[0] = (char) (cmd / 256) ;
	g_SendBuf[1] = (char) (cmd % 256) ;
	ptr = &g_SendBuf[2] ;
#ifdef	UNICODE
	wcstombs( ptr, path, sizeof g_SendBuf - 2 ) ;
#else	/* UNICODE */
	strcpy( ptr, path ) ;
#endif	/* UNICODE */
	g_SendSize = 2 + strlen( ptr ) + 1 ;
	send( socket, g_SendBuf, g_SendSize, 0 ) ;
}

BOOL
MainApp::SendFile( SOCKET socket )
{
	DWORD		nRead = 0 ;

	ReadFile( m_hFile, &g_SendBuf, PacketSize, &nRead, 0 ) ;
	g_SendSize = nRead ;
	send( socket, g_SendBuf, g_SendSize, 0 ) ;
	return nRead == PacketSize ;
}

void
MainApp::SendAck( SOCKET socket )
{
	u_short		cmd = TFTP_ACK ;

	g_SendBuf[0] = (char) (cmd / 256) ;
	g_SendBuf[1] = (char) (cmd % 256) ;
	g_SendSize = 2 ;
	m_fError = send( socket, g_SendBuf, g_SendSize, 0 ) == SOCKET_ERROR ;
}

void
MainApp::SendError( SOCKET socket, LPCTSTR str )
{
	char		*ptr ;
	u_short		cmd = TFTP_ERROR ;

	g_SendBuf[0] = (char) (cmd / 256) ;
	g_SendBuf[1] = (char) (cmd % 256) ;
	ptr = &g_SendBuf[2] ;
#ifdef	UNICODE
	wcstombs( ptr, str, sizeof g_SendBuf - 2 ) ;
#else	/* UNICODE */
	strcpy( ptr, str ) ;
#endif	/* UNICODE */
	g_SendSize = 2 + strlen( ptr ) + 1 ;
	m_fError = send( socket, g_SendBuf, g_SendSize, 0 ) == SOCKET_ERROR ;
}

void
MainApp::NotifyMessage( LPCTSTR message, DWORD code )
{
	if ( g_hWndMsg ) {
		wsprintf( MessageBuf, message, code ) ;
		SetWindowText( g_hWndMsg, MessageBuf ) ;
	}
}

void
MainApp::AddTransferList( BOOL fPut, LPCTSTR path )
{
	if ( g_hWndList ) {
		int		index ;
		TCHAR	buf[ MAX_PATH ] ;

		wsprintf( buf, TEXT("%s %s"), fPut ? TEXT("->") : TEXT("<-"), path ) ;
		index = SendMessage( g_hWndList, LB_ADDSTRING, 0, (LPARAM) buf ) ;
		SendMessage( g_hWndList, LB_SETCURSEL, (WPARAM) index, 0 ) ;
	}
}

BOOL
MainApp::MakeTransPair( LPCTSTR szSrcDir, LPCTSTR szDstDir )
{
	WIN32_FIND_DATA	find ;
	HANDLE			hFind ;
	List			dirList ;
	DWORD			attr, len ;
	TCHAR			findPath[ MAX_PATH ], dirBase[ MAX_PATH ] ;

	m_oTransList.clear() ;
	attr = GetFileAttributes( szSrcDir ) ;
	if ( attr == 0xFFFFFFFF || !(attr & FILE_ATTRIBUTE_DIRECTORY) ) {
		return FALSE ;
	}
	len = _tcslen( szSrcDir ) + 1 ;
	dirList.append( new DirLink( szSrcDir ) ) ;
	while ( 1 ) {
		Iterator	l( dirList ) ;
		DirLink		*cur = (DirLink*) l() ;
		if ( !cur ) {
			break ;
		}
		/* 先頭の項目を取得し削除する */
		cur->get( dirBase ) ;
		dirList.delete_first() ;
		/* ワイルドカードを追加する */
		_tcscpy( findPath, dirBase ) ;
		catdir( findPath, TEXT("*.*"), NULL ) ;
		/* ディレクトリを検索する */
		hFind = FindFirstFile( findPath, &find ) ;
		if ( hFind != INVALID_HANDLE_VALUE ) {
			do {
				if ( !_tcscmp( find.cFileName, TEXT(".") ) ) {
					continue ;
				} else if ( !_tcscmp( find.cFileName, TEXT("..") ) ) {
					continue ;
				}
				_tcscpy( findPath, dirBase ) ;
				catdir( findPath, find.cFileName, NULL ) ;
				if ( find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
					dirList.append( new DirLink( findPath ) ) ;
				} else {
					m_oTransList.append( new TransPair( findPath, &findPath[len] ) ) ;
				}
			} while ( FindNextFile( hFind, &find ) ) ;
			FindClose( hFind ) ;
		}
	}
	return TRUE ;
}

/*-------------------------------------------------------------------------*
 * TransPair
 *-------------------------------------------------------------------------*/
TransPair::TransPair( LPCTSTR src, LPCTSTR dst )
{
	DWORD		size ;

	m_bProcessed = FALSE ;
	if ( src ) {
		size = (_tcslen( src ) + 1) * sizeof (TCHAR) ;
		m_tSrcPath = new TCHAR[ size ] ;
		memcpy( m_tSrcPath, src, size ) ;
	} else {
		m_tSrcPath = 0 ;
	}
	if ( dst ) {
		size = (_tcslen( dst ) + 1) * sizeof (TCHAR) ;
		m_tDstPath = new TCHAR[ size ] ;
		memcpy( m_tDstPath, dst, size ) ;
	} else {
		m_tDstPath = 0 ;
	}
}

TransPair::~TransPair()
{
	if ( m_tSrcPath ) {
		delete[] m_tSrcPath ;
	}
	if ( m_tDstPath ) {
		delete[] m_tDstPath ;
	}
}

void
TransPair::Append( List *pList, TransPair *pair )
{
	pList->append( (PLink) pair ) ;
}

TransPair*
TransPair::GetNext( List *pList )
{
	Iterator	l( *pList ) ;
	TransPair	*cur ;

	while ( cur = (TransPair*) l() ) {
		if ( !cur->get_processed() ) {
			return cur ;
		}
	}
	return cur ;
}

/*-------------------------------------------------------------------------*
 * DirLink
 *-------------------------------------------------------------------------*/
DirLink::DirLink( LPCTSTR dir )
{
	DWORD		size ;

	if ( dir ) {
		size = (_tcslen( dir ) + 1) * sizeof (TCHAR) ;
		m_tDir = new TCHAR[ size ] ;
		memcpy( m_tDir, dir, size ) ;
	} else {
		m_tDir = 0 ;
	}
}

DirLink::~DirLink()
{
	if ( m_tDir ) {
		delete[] m_tDir ;
	}
}

/*
 * 関数名：mkdir_recursive
 * 引き数：dir(ディレクトリ名)
 * 戻り値：BOOL(TRUE=正常, FALSE=異常)
 *   概要：再帰的にディレクトリを作成する。
 */
BOOL
mkdir_recursive( LPCTSTR dir )
{
	DWORD		attr ;
	LPCTSTR		p = dir, start = dir ;
	TCHAR		path[ MAX_PATH + 1 ], c ;

	while ( (c = *p) != NULL ) {
		if ( c != '\\' ) {
		} else if ( p != start ) {
			int		len = p - start ;
			memcpy( path, start, len * sizeof (TCHAR) ) ;
			path[ len ] = 0 ;
			attr = GetFileAttributes( path ) ;
			if ( attr == 0xFFFFFFFF ) {
				if ( CreateDirectory( path, NULL ) != TRUE ) {
					return FALSE ;
				}
			} else if ( !(attr & FILE_ATTRIBUTE_DIRECTORY) ) {
				return FALSE ;
			}
		}
		p ++ ;
	}
	return TRUE ;
}
