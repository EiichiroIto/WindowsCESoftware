#ifndef	UNICODE
#define	UNICODE
#endif	/* UNICODE */
#ifndef	_UNICODE
#define	_UNICODE
#endif	/* _UNICODE */
#define	STRICT
#include	<windows.h>
#include	<commctrl.h>
#include	<tchar.h>
#include	"kctrl.h"
#include	"appmain.h"

DWORD	AppVersion = 43 ;
DWORD	DllVersion = 0 ;

int WINAPI
#ifdef	_WIN32_WCE
WinMain( HINSTANCE hThisInst, HINSTANCE hPrevInst,
		 LPTSTR lpszArgs, int nWinMode )
#else	/* _WIN32_WCE */
WinMain( HINSTANCE hThisInst, HINSTANCE hPrevInst,
		 LPSTR lpszArgs, int nWinMode )
#endif	/* _WIN32_WCE */
{
#ifdef	_WIN32_WCE
	InitCommonControls() ;
#endif	/* _WIN32_WCE */
	/* KCTRL.DLL‚Ì‰Šú‰»‚ðs‚¤ */
	if ( !InitKanjiControls() ) {
		wsprintf( MessageBuf, TEXT("KCTRL.DLL Initialize error\r\nError=%x"),
				  GetLastError() ) ;
		MessageBeep( MB_ICONEXCLAMATION ) ;
		MessageBox( NULL, MessageBuf, TEXT("KCTRL Error"), MB_OK|MB_ICONASTERISK ) ;
		return -1 ;
	}
	DllVersion = GetKVersion() ;

	BOOL		exitVal = FALSE ;
	TrainApp	*pApp = new TrainApp() ;
	if ( !pApp ) {
		goto exit ;
	}
	if ( pApp->Create( hThisInst ) ) {
		exitVal = pApp->Run() ;
	}
	delete pApp ;

exit:
	ReleaseKanjiControls() ;
	return exitVal ;
}
