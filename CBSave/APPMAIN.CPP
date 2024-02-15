#define	STRICT
#include	<windows.h>
#include	<windowsx.h>
#include	<commctrl.h>
#include	<tchar.h>
#include	"kctrl.h"
#include	"tools.h"
#include	"resource.h"
#include	"appmain.h"
#include	"dialog.h"

#define		GawaroBaseKey	TEXT("Software\\Gawaro")
#define		VAR_VIEWER		TEXT("Viewer")
#define		VAR_FOLDER		TEXT("SaveFolder")
#define		VAR_FILE		TEXT("SaveFile")
#define		VAR_FLAG		TEXT("ToFile")
#define		VAR_WAIT		TEXT("CopyWait")

TCHAR	MessageBuf[ 512 ] ;
TCHAR	g_szFile[ MAX_PATH ] = TEXT("\\CBSAVE.TXT") ;
TCHAR	g_szFolder[ MAX_PATH ] = TEXT("") ;
TCHAR	g_szViewer[ MAX_PATH ] = TEXT("KVIEW.EXE") ;
BOOL	g_fToFile = TRUE ;
HWND	g_hWndDialog = NULL ;
DWORD	g_dwSleep = 1 ;

BOOL	find_new_path( LPCTSTR basedir, LPTSTR path ) ;

class MainWindow : public Window {
private:
	MainApp		*m_pApp ;			/* アプリケーションへのポインタ */
	HICON		m_hIcon ;			/* アプリアイコンのハンドル */
	BOOL		m_fLoadNotify ;		/* タスクバー格納フラグ */
	BOOL		m_fNoMenu ;

public:
	MainWindow( MainApp *app ) ;

	BOOL	Create() ;
	void	CreateFepWnd( LPRECT pos ) ;

	void	WMCreate( HWND hWnd ) ;
	BOOL	AddTaskBarIcon() const ;
	BOOL	DeleteTaskBarIcon() const ;

	void	WMDestroy() ;
	LRESULT	WMCommand( HWND hWnd, UINT msg, WPARAM wp, LPARAM lp ) ;
	void	WMNotify( HWND hWnd, int idCtrl, LPNMHDR pnmh ) ;
	BOOL	WMClose() ;

	static	BOOL	RegisterClass( HINSTANCE hInst ) ;
	static	LPCTSTR	GetClassName() { return TEXT("CBSAVE") ; }
} ;

/*-------------------------------------------------------------------------*
 * MainApp
 *-------------------------------------------------------------------------*/
MainApp::MainApp()
{
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

	m_pMainWnd = new MainWindow( this ) ;
	if ( !m_pMainWnd ) {
		return FALSE ;
	}
	if ( !m_pMainWnd->Create() ) {
		delete m_pMainWnd ;
		return FALSE ;
	}
	GetReg() ;
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

/*
 * Select AllとCopyを行う
 */
void
MainApp::CopyParent() const
{
	if ( OpenClipboard( NULL ) ) {
		EmptyClipboard() ;
		CloseClipboard() ;
	}
	keybd_event( VK_CONTROL, 0, 0, 0 ) ;
	keybd_event( 0x41, 0, 0, 0 ) ;
	keybd_event( 0x41, 0, KEYEVENTF_KEYUP, 0 ) ;
	keybd_event( VK_CONTROL, 0, KEYEVENTF_KEYUP, 0 ) ;
	keybd_event( VK_CONTROL, 0, 0, 0 ) ;
	keybd_event( 0x43, 0, 0, 0 ) ;
	keybd_event( 0x43, 0, KEYEVENTF_KEYUP, 0 ) ;
	keybd_event( VK_CONTROL, 0, KEYEVENTF_KEYUP, 0 ) ;
}

DWORD
CopySub( LPTSTR dst, LPCTSTR src, DWORD srcsize )
{
	TCHAR	c ;
	int		count_crlf = 0 ;
	BOOL	fCR = FALSE, fLF = FALSE ;

	set_convmode( MODE_NORMAL ) ;
	setup_codeconv( dst ) ;
	do {
		c = *src++ ;
		if ( !c ) {
			break ;
		}
		if ( c == 13 ) {
			if ( !fLF ) {
				fCR = TRUE ;
				if ( ++ count_crlf <= 2 ) {
					jis_unicode_char( 13 ) ;
					jis_unicode_char( 10 ) ;
				}
			}
			continue ;
		} else if ( c == 10 ) {
			if ( !fCR ) {
				fLF = TRUE ;
				if ( ++ count_crlf <= 2 ) {
					jis_unicode_char( 13 ) ;
					jis_unicode_char( 10 ) ;
				}
			}
			continue ;
		}
		switch ( c ) {
		case 0x201A:	c = 0x82 ;	break ;
		case 0x0192:	c = 0x83 ;	break ;
		case 0x201E:	c = 0x84 ;	break ;
		case 0x2026:	c = 0x85 ;	break ;
		case 0x2020:	c = 0x86 ;	break ;
		case 0x2021:	c = 0x87 ;	break ;
		case 0x02C6:	c = 0x88 ;	break ;
		case 0x2030:	c = 0x89 ;	break ;
		case 0x0160:	c = 0x8A ;	break ;
		case 0x2039:	c = 0x8B ;	break ;
		case 0x0152:	c = 0x8C ;	break ;
		case 0x2018:	c = 0x91 ;	break ;
		case 0x2019:	c = 0x92 ;	break ;
		case 0x201C:	c = 0x93 ;	break ;
		case 0x201D:	c = 0x94 ;	break ;
		case 0x2022:	c = 0x95 ;	break ;
		case 0x2013:	c = 0x96 ;	break ;
		case 0x2014:	c = 0x97 ;	break ;
		case 0x02DC:	c = 0x98 ;	break ;
		case 0x2122:	c = 0x99 ;	break ;
		case 0x0161:	c = 0x9A ;	break ;
		case 0x203A:	c = 0x9B ;	break ;
		case 0x0153:	c = 0x9C ;	break ;
		case 0x0178:	c = 0x9F ;	break ;
		}
		if ( c != 0x0020 ) {
			count_crlf = 0 ;
		}
		jis_unicode_char( c ) ;
	} while ( -- srcsize ) ;
	jis_unicode_char( 0 ) ;
	return dst ? 0 : getcount_codeconv() ;
}

BOOL
MainApp::Copy( HWND hWnd )
{
	LPTSTR	dst ;
	HLOCAL	hGMem, hDstMem ;
	LPCTSTR	cbptr ;
	DWORD	cbsize, dstsize ;
	TCHAR	c, fn[ MAX_PATH ] ;

	if ( !OpenClipboard( hWnd ) ) {
		return FALSE ;
	}
	hGMem = GetClipboardData( CF_UNICODETEXT ) ;
	cbsize = LocalSize( hGMem ) ;
	cbptr = (LPCTSTR) hGMem ;
	if ( !cbsize ) {
		goto error ;
	}
	if ( g_fToFile ) {
		_tcscpy( fn, g_szFile ) ;
	} else if ( !find_new_path( g_szFolder, fn ) ) {
		goto error ;
	}
	/* コード変換に必要なバッファサイズを求める */
	dstsize = CopySub( 0, cbptr, cbsize ) ;
	if ( !dstsize ) {
		/* 何もなかった場合 */
		goto error ;
	}
	/* コード変換用のバッファを確保する */
	hDstMem = LocalAlloc( LMEM_FIXED, dstsize * sizeof (TCHAR) ) ;
	dst = (LPTSTR) hDstMem ;
	if ( !dst ) {
		/* バッファを確保できなかった場合 */
		goto error ;
	}
	/* コード変換を行う */
	CopySub( dst, cbptr, cbsize ) ;
	/* 変換結果を書き込むファイルをオープンする */
	if ( !Fopen( fn, TEXT("wb") ) ) {
		LocalFree( (HLOCAL) dst ) ;
		goto error ;
	}
	/* UnicodeをSJISに変換しながらファイルに書き込む */
	do {
		c = *dst++ ;
		if ( !c ) {
			break ;
		}
		c = unicode2sjis_char( c ) ;
		if ( c >= 0x100 ) {
			Fputc( c / 0x100 ) ;
			Fputc( c % 0x100 ) ;
		} else {
			Fputc( c ) ;
		}
	} while ( -- dstsize ) ;
	/* ファイルをクローズする */
	Fclose() ;
	/* 変換用バッファを解放する */
	LocalFree( (HLOCAL) hDstMem ) ;
	/* クリップボードを閉じる */
	CloseClipboard() ;
	/* ビューアを起動する */
	if ( *g_szViewer && *fn ) {
		PROCESS_INFORMATION		pi ;

		CreateProcess( g_szViewer, fn,
			NULL, NULL, FALSE,
			0,
			NULL, NULL, NULL,
			&pi ) ;
	}
	return TRUE ;

error:
	CloseClipboard() ;
	return FALSE ;
}

BOOL
MainApp::GetReg()
{
	HKEY	hk ;
	LONG	lret ;
	LPCTSTR	name ;
	LPBYTE	lpData ;
	TCHAR	sValue[ 256 ] ;
	DWORD	dwType, cbData, dwValue ;

	wsprintf( sValue, TEXT("%s\\%s"), GawaroBaseKey, szTitleName ) ;
	/* レジストリキーをオープンする */
	lret = RegOpenKeyEx( HKEY_CURRENT_USER, sValue, 0,
						 KEY_QUERY_VALUE, &hk ) ;
	if ( lret == ERROR_SUCCESS ) {
		/* Viewer */
		name = VAR_VIEWER ;
		dwType = REG_SZ ;
		lpData = (LPBYTE) sValue ;
		cbData = sizeof sValue ;
		if ( RegQueryValueEx( hk, name, NULL, &dwType, lpData, &cbData ) == ERROR_SUCCESS ) {
			_tcscpy( g_szViewer, sValue ) ;
		}
		/* Folder */
		name = VAR_FOLDER ;
		dwType = REG_SZ ;
		lpData = (LPBYTE) sValue ;
		cbData = sizeof sValue ;
		if ( RegQueryValueEx( hk, name, NULL, &dwType, lpData, &cbData ) == ERROR_SUCCESS ) {
			_tcscpy( g_szFolder, sValue ) ;
		}
		/* File */
		name = VAR_FILE ;
		dwType = REG_SZ ;
		lpData = (LPBYTE) sValue ;
		cbData = sizeof sValue ;
		if ( RegQueryValueEx( hk, name, NULL, &dwType, lpData, &cbData ) == ERROR_SUCCESS ) {
			_tcscpy( g_szFile, sValue ) ;
		}
		/* Flag */
		name = VAR_FLAG ;
		dwType = REG_DWORD ;
		lpData = (LPBYTE) &dwValue ;
		cbData = sizeof dwValue ;
		if ( RegQueryValueEx( hk, name, NULL, &dwType, lpData, &cbData ) == ERROR_SUCCESS ) {
			g_fToFile = dwValue ;
		}
		/* Wait */
		name = VAR_WAIT ;
		dwType = REG_DWORD ;
		lpData = (LPBYTE) &dwValue ;
		cbData = sizeof dwValue ;
		if ( RegQueryValueEx( hk, name, NULL, &dwType, lpData, &cbData ) == ERROR_SUCCESS ) {
			g_dwSleep = dwValue ;
		}
		/* レジストリクローズ */
		RegCloseKey( hk ) ;
	}
	return TRUE ;
}

void
MainApp::SetReg() const
{
	HKEY	hk ;
	LONG	lret ;
	LPTSTR	name ;
	LPBYTE	lpData ;
	TCHAR	sValue[ 256 ] ;
	DWORD	dwType, cbData, dwValue, ret ;

	/* キー名称を作成する */
	wsprintf( sValue, TEXT("%s\\%s"), GawaroBaseKey, szTitleName ) ;
	/* レジストリキーを作成する */
	lret = RegCreateKeyEx( HKEY_CURRENT_USER, sValue, 0, TEXT(""), 0,
#ifdef	_WIN32_WCE
						   0,
#else	/* _WIN32_WCE */
						   KEY_ALL_ACCESS,
#endif	/* _WIN32_WCE */
						   NULL, &hk, &ret ) ;
	if ( lret == ERROR_SUCCESS ) {
		/* Viewer */
		name = VAR_VIEWER ;
		dwType = REG_SZ ;
		lpData = (LPBYTE) g_szViewer ;
		cbData = (_tcslen(g_szViewer) + 1) * sizeof (*g_szViewer) ;
		RegSetValueEx( hk, name, 0, dwType, lpData, cbData ) ;
		/* Folder */
		name = VAR_FOLDER ;
		dwType = REG_SZ ;
		lpData = (LPBYTE) g_szFolder ;
		cbData = (_tcslen(g_szFolder) + 1) * sizeof (*g_szFolder) ;
		RegSetValueEx( hk, name, 0, dwType, lpData, cbData ) ;
		/* File */
		name = VAR_FILE ;
		dwType = REG_SZ ;
		lpData = (LPBYTE) g_szFile ;
		cbData = (_tcslen(g_szFile) + 1) * sizeof (*g_szFile) ;
		RegSetValueEx( hk, name, 0, dwType, lpData, cbData ) ;
		/* Flag */
		name = VAR_FLAG ;
		dwType = REG_DWORD ;
		dwValue = g_fToFile ? 1 : 0 ;
		lpData = (LPBYTE) &dwValue ;
		cbData = sizeof dwValue ;
		RegSetValueEx( hk, name, 0, dwType, lpData, cbData ) ;
		/* Wait */
		name = VAR_WAIT ;
		dwType = REG_DWORD ;
		dwValue = g_dwSleep ;
		lpData = (LPBYTE) &dwValue ;
		cbData = sizeof dwValue ;
		RegSetValueEx( hk, name, 0, dwType, lpData, cbData ) ;
		/* レジストリクローズ */
		RegCloseKey( hk ) ;
	}
}

void
MainApp::DeleteReg() const
{
	TCHAR	sValue[ 256 ] ;

	wsprintf( sValue, TEXT("%s\\%s"), GawaroBaseKey, szTitleName ) ;
	RegDeleteKey( HKEY_CURRENT_USER, sValue ) ;
}

/*-------------------------------------------------------------------------*
 * MainWindow
 *-------------------------------------------------------------------------*/
MainWindow::MainWindow( MainApp *pApp )
{
	m_pApp = pApp ;
	m_hIcon = 0 ;
	m_fNoMenu = FALSE ;
}

void
MainWindow::WMCreate( HWND hWnd )
{
	m_hWnd = hWnd ;
	/* アイコンを作成しタスクバーに登録する */
	m_hIcon = (HICON) LoadImage( m_pApp->get_inst(), MAKEINTRESOURCE(IDI_APPICON),
								 IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR ) ;
	if ( m_hIcon ) {
		SendMessage( hWnd, WM_SETICON, FALSE, (LPARAM) m_hIcon ) ;
	}
	/* タスクバーに登録する */
	m_fLoadNotify = AddTaskBarIcon() ;
}

/*
 * タスクバーに通知アイコンを登録する
 */
BOOL
MainWindow::AddTaskBarIcon() const
{
	NOTIFYICONDATA	tnid ;
	HICON			hIcon ;

	/* 通知エリア用のアイコンを作成する */
	hIcon = (HICON) LoadImage( m_pApp->get_inst(), MAKEINTRESOURCE(IDI_APPICON),
							   IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR ) ;
	memset( &tnid, 0, sizeof tnid ) ;
	tnid.cbSize = sizeof (NOTIFYICONDATA) ;
	tnid.hWnd = m_hWnd ;
	tnid.uID = IDM_NOTIFY ;
	tnid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP ;
	tnid.uCallbackMessage = WM_COMMAND ;
	tnid.hIcon = hIcon ;
	_tcscpy( tnid.szTip, szTitleName ) ;
	return Shell_NotifyIcon( NIM_ADD, &tnid ) ;
}

/*
 * 通知アイコンを削除する
 */
BOOL
MainWindow::DeleteTaskBarIcon() const
{
	NOTIFYICONDATA	tnid ;

	memset( &tnid, 0, sizeof tnid ) ;
	tnid.cbSize = sizeof (NOTIFYICONDATA) ;
	tnid.hWnd = m_hWnd ;
	tnid.uID = IDM_NOTIFY ;
	return Shell_NotifyIcon( NIM_DELETE, &tnid ) ;
}

void
MainWindow::WMDestroy()
{
	DeleteTaskBarIcon() ;
	if ( m_hIcon ) {
		::DeleteObject( m_hIcon ) ;
		m_hIcon = 0 ;
	}
	PostQuitMessage( 0 ) ;
}

LRESULT
MainWindow::WMCommand( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	DWORD	ret ;
	WORD	notifyCode = HIWORD(wParam), cmd = LOWORD(wParam) ;

	switch ( cmd ) {
	case IDM_EXIT:
		SendMessage( hWnd, WM_CLOSE, 0, 0 ) ;
		break ;
	case IDM_NOTIFY:
		if ( lParam == WM_LBUTTONDOWN ) {
			if ( GetAsyncKeyState( VK_MENU ) & 0x8000 ) {
				SendMessage( hWnd, WM_CLOSE, 0, 0 ) ;
			} else {
				if ( m_fNoMenu ) {
					if ( g_hWndDialog ) {
						SetForegroundWindow( g_hWndDialog ) ;
					}
					break ;
				}
				SetForegroundWindow( hWnd ) ;
				/* ポップアップメニューを作成する */
				HMENU	hMenu = ::CreatePopupMenu() ;
				if ( hMenu ) {
					::AppendMenu( hMenu, MF_STRING, IDM_COPYSAVE0, TEXT("Copy && Save") ) ;
					::AppendMenu( hMenu, MF_STRING, IDM_SAVE, TEXT("Save") ) ;
					::AppendMenu( hMenu, MF_SEPARATOR, 0, 0 ) ;
					::AppendMenu( hMenu, MF_STRING, IDM_SETUP, TEXT("Setup...") ) ;
					::AppendMenu( hMenu, MF_SEPARATOR, 0, 0 ) ;
					::AppendMenu( hMenu, MF_STRING, IDM_EXIT, TEXT("Exit") ) ;
					::TrackPopupMenu( hMenu,
							TPM_BOTTOMALIGN|TPM_RIGHTALIGN,
							GetSystemMetrics( SM_CXSCREEN ),
							GetSystemMetrics( SM_CYSCREEN ) - 30,
							0,
							hWnd,
							NULL ) ;
					::DestroyMenu( hMenu ) ;
				}
			}
		}
		break ;
	case IDM_COPYSAVE0:
		PostMessage( hWnd, WM_COMMAND, IDM_COPYSAVE, 0 ) ;
		break ;
	case IDM_COPYSAVE:
		if ( m_fNoMenu ) {
			break ;
		}
		m_fNoMenu = TRUE ;
		ret = g_dwSleep ;
		if ( YesNoDialogExec( m_pApp->get_inst(), NULL, TEXT("Select && Copy this application?"), szTitleName ) ) {
			m_pApp->CopyParent() ;
			Sleep( g_dwSleep * 1000 ) ;
			if ( m_pApp->Copy( hWnd ) ) {
				if ( ret != g_dwSleep ) {
					m_pApp->SetReg() ;
				}
			} else {
				MessageBeep( MB_ICONHAND ) ;
			}
		}
		m_fNoMenu = FALSE ;
		break ;
	case IDM_SAVE:
		if ( m_fNoMenu ) {
			break ;
		}
		m_fNoMenu = TRUE ;
		if ( !m_pApp->Copy( hWnd ) ) {
			MessageBeep( MB_ICONHAND ) ;
		}
		m_fNoMenu = FALSE ;
		break ;
	case IDM_SETUP:
		/* セットアップ */
		if ( m_fNoMenu ) {
			break ;
		}
		m_fNoMenu = TRUE ;
		ret = SetupDialogExec( m_pApp->get_inst(), hWnd ) ;
		if ( ret & DIALOG_RET_EXIT ) {
			SendMessage( hWnd, WM_CLOSE, 0, 0 ) ;
		} else if ( ret & DIALOG_RET_UNINSTALL ) {
			m_pApp->DeleteReg() ;
			SendMessage( hWnd, WM_CLOSE, 0, 0 ) ;
		} else if ( ret ) {
			m_pApp->SetReg() ;
		}
		m_fNoMenu = FALSE ;
		break ;
	}
	return 0 ;
}

void
MainWindow::WMNotify( HWND hWnd, int idCtrl, LPNMHDR pnmh )
{
}

BOOL
MainWindow::WMClose()
{
	return FALSE ;
}

BOOL
MainWindow::Create()
{
	HWND	hWnd = CreateWindowEx( 0,
						GetClassName(),
						szTitleName,
						WS_BORDER,
						0, 0, 10, 10,
						NULL,
						NULL,
						m_pApp->get_inst(),
						this ) ;
	if ( hWnd == 0 ) {
		return FALSE ;
	}
	ShowWindow( hWnd, SW_HIDE ) ;
    UpdateWindow( hWnd ) ;
    return TRUE ;
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
	wc.lpszMenuName  = 0 ;
	wc.lpszClassName = 	GetClassName() ;
	return ::RegisterClass( &wc ) ;
}

BOOL
find_new_path( LPCTSTR basedir, LPTSTR path )
{
	int		i ;

	for ( i = 0 ; i < 10000 ; i ++ ) {
		wsprintf( path, TEXT("%s\\CBSV%04d.TXT"),
				  basedir, i ) ;
		if ( 0xFFFFFFFF == GetFileAttributes( path ) ) {
			if ( GetLastError() == ERROR_FILE_NOT_FOUND ) {
				break ;
			}
			return FALSE ;
		}
	}
	return TRUE ;
}
