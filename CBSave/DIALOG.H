#define		DIALOG_RET_CHANGE		0x01
#define		DIALOG_RET_CHANGEDICT	0x80
#define		DIALOG_RET_EXIT			0x40
#define		DIALOG_RET_UNINSTALL	0x20

BOOL	SetupDialogExec( HINSTANCE hInst, HWND hWnd ) ;
BOOL	YesNoDialogExec( HINSTANCE hInst, HWND hWnd, LPCTSTR pMessage, LPCTSTR pTitle ) ;
