#define	STRICT
#include	<windows.h>
#include	<commctrl.h>
#include	<tchar.h>
#include	"appmain.h"

DWORD	AppVersion = 9 ;
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

	BOOL		exitVal = FALSE ;
	MainApp		*pApp = new MainApp() ;
	if ( !pApp ) {
		goto exit ;
	}
	if ( pApp->Create( hThisInst ) ) {
		exitVal = pApp->Run() ;
	}
	delete pApp ;

exit:
	return exitVal ;
}
