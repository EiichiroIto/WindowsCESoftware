#ifndef	_H_DIALOG
#define	_H_DIALOG

class MainApp ;

typedef struct {
	TCHAR	srcPath[ MAX_PATH ] ;
	TCHAR	dstPath[ MAX_PATH ] ;
} TransInfo, *PTransInfo ;

BOOL	AboutDialogExec( HINSTANCE hInst, HWND hWnd, LPBOOL fDelete ) ;
BOOL	TransferDialogExec( HINSTANCE hInst, HWND hWnd, DWORD dwMode, MainApp *pApp ) ;
BOOL	OpenDialogExec( HINSTANCE hInst, HWND hWnd, LPTSTR szPath ) ;

#endif	/* _H_DIALOG */
