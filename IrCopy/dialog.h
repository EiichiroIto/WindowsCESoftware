#ifndef	_H_DIALOG
#define	_H_DIALOG

class MainApp ;

typedef struct {
	TCHAR	srcPath[ MAX_PATH ] ;
	TCHAR	dstPath[ MAX_PATH ] ;
} TransInfo, *PTransInfo ;

BOOL	FileOpenDlg( HINSTANCE hInst, HWND hWnd, LPTSTR buf, DWORD size ) ;
BOOL	OpenListDlg( HINSTANCE hInst, HWND hWnd, LPTSTR buf, DWORD size ) ;
BOOL	SaveListDlg( HINSTANCE hInst, HWND hWnd, LPTSTR buf, DWORD size ) ;
BOOL	AboutDialogExec( HINSTANCE hInst, HWND hWnd, LPBOOL fDelete ) ;
BOOL	AddDialogExec( HINSTANCE hInst, HWND hWnd, BOOL fEdit, TransInfo *pInfo ) ;
BOOL	TransferDialogExec( HINSTANCE hInst, HWND hWnd, DWORD dwMode, MainApp *pApp ) ;

#endif	/* _H_DIALOG */
