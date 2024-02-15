#define	STRICT
#include	<windows.h>
#include	<commctrl.h>
#include	<tchar.h>
#include	"kctrl.h"
#include	"appmain.h"

DWORD	AppVersion = 4 ;
DWORD	DllVersion = 0 ;
#define	MAX_ARGV	10
DWORD	Argc = 0 ;
LPTSTR	Argv[ MAX_ARGV ] ;
BOOL	Quote[ MAX_ARGV ] ;

int WINAPI
#ifdef	_WIN32_WCE
WinMain( HINSTANCE hThisInst, HINSTANCE hPrevInst,
		 LPTSTR lpszArgs, int nWinMode )
#else	/* _WIN32_WCE */
WinMain( HINSTANCE hThisInst, HINSTANCE hPrevInst,
		 LPSTR lpszArgs, int nWinMode )
#endif	/* _WIN32_WCE */
{
	/* ���ɋN������Ă��邩�ǂ����`�F�b�N���� */
	if ( FindWindow( szTitleName, NULL ) != NULL ) {
		return TRUE ;
	}

#ifdef	_WIN32_WCE
	InitCommonControls() ;
#endif	/* _WIN32_WCE */

	/* KCTRL.DLL�̏��������s�� */
	if ( !InitKanjiControls() ) {
		wsprintf( MessageBuf, TEXT("KCTRL.DLL Initialize error\r\nError=%x"),
				  GetLastError() ) ;
		MessageBeep( MB_ICONEXCLAMATION ) ;
		MessageBox( NULL, MessageBuf, TEXT("KCTRL Error"), MB_OK ) ;
		return -1 ;
	}
	DllVersion = GetKVersion() ;

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
	ReleaseKanjiControls() ;
	return exitVal ;
}