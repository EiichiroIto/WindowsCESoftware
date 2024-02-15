#define		CTRL_LINEEDIT		TEXT("LINEEDIT")
#define		CTRL_TIMEEDIT		TEXT("TIMEEDIT")
#define		CTRL_BIGLIST		TEXT("BIGLIST")
#define		CTRL_LABEL			TEXT("KLABEL")

#define		RBM_GETSJIS			(WM_USER+0x100)
#define		RBM_SETSJIS			(WM_USER+0x101)
#define		RBM_GETMODIFY		(WM_USER+0x102)
#define		RBM_SETMODIFY		(WM_USER+0x103)
#define		RBN_ENTER			0x0100
#define		RBN_ESC				0x0101
#define		RBN_TAB				0x0102
#define		RBN_STAB			0x0103
#define		RBN_CHANGE			0x0104
#define		RBN_KEYDOWN			0x0105
#define		RBN_KILLFOCUS		0x0106

#define		TEM_GETTIMEVAL		(WM_USER+0x100)
#define		TEM_SETTIMEVAL		(WM_USER+0x101)
#define		TEM_GETMODIFY		(WM_USER+0x102)
#define		TEM_SETMODIFY		(WM_USER+0x103)
#define		TEN_ENTER			0x0100
#define		TEN_ESC				0x0101
#define		TEN_TAB				0x0102
#define		TEN_STAB			0x0103
#define		TEN_CHANGE			0x0104
#define		TEN_KEYDOWN			0x0105
#define		TEN_KILLFOCUS		0x0106

#define		BLM_SETPROC			(WM_USER+0x100)
#define		BLM_SETSPACEPROC	(WM_USER+0x101)
#define		BLM_GETCURSEL		(WM_USER+0x102)
#define		BLM_SETCURSEL		(WM_USER+0x103)
#define		BLN_ENTER			0x0100
#define		BLN_ESC				0x0101
#define		BLN_TAB				0x0102
#define		BLN_STAB			0x0103
#define		BLN_CHANGE			0x0104

#define		LAM_SETINVERT		(WM_USER+0x100)
#define		LAM_SETCROSS		(WM_USER+0x101)
#define		LAN_CLICK			0x100

#define		IDM_FEPOK			(41260)
#define		IDM_FEPACTIVE		(41261)
#define		IDM_FEPKEY			(41262)

extern "C" {
	BOOL	LineEditRegisterClass( HINSTANCE hInst ) ;
	BOOL	BigListRegisterClass( HINSTANCE hInst ) ;
	BOOL	LabelRegisterClass( HINSTANCE hInst ) ;
	BOOL	TimeEditRegisterClass( HINSTANCE hInst ) ;
} ;
