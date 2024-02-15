#ifndef	UNICODE
#define	UNICODE
#endif	/* UNICODE */
#ifndef	_UNICODE
#define	_UNICODE
#endif	/* _UNICODE */
#define	STRICT
#include	<windows.h>
#include	<stdio.h>
#include	<string.h>
#include	<tchar.h>
#include	<windowsx.h>
#include	<commctrl.h>
#include	<commdlg.h>
#include	<ctype.h>
#include	<wingdi.h>
#include	"dialog.h"
#include	"resource.h"

HINSTANCE	hInst ;
HWND		hMainWnd ;
HWND		hCmdBar ;

TCHAR		szWinName[] = TEXT("MukaMuka") ;
#define		TitleName	szWinName
TCHAR		Version[] = TEXT("0.08") ;
TCHAR		MessageBuf[ MAX_PATH ] = TEXT("") ;

HFONT		hFont ;
HWND		hMsgWnd ;

BOOL		F_quit = TRUE ;
BOOL		F_gateEnable = FALSE ;
HBITMAP		hBmp = 0 ;
DWORD		WindowTopY = 0 ;
HANDLE		hAnimThread = 0 ;
DWORD		AnimThreadID = 0 ;
BOOL		F_touch_object = FALSE ;
DWORD		BmpWidth = 480 ;
DWORD		BmpHeight = 200 ;
DWORD		StartTime = 0 ;
DWORD		CurTime = 0 ;
DWORD		TickTime = 100 ;
BOOL		F_skipstart = FALSE ;
BOOL		F_nowall = FALSE ;
BOOL		F_nopos = FALSE ;

TCHAR		MukaPath[ MAX_PATH ]   = TEXT("") ;
TCHAR		BmpPath[ MAX_PATH ]   = TEXT("") ;
TCHAR		LastPath[ MAX_PATH ]  = TEXT("") ;
#ifdef	_WIN32_WCE
TCHAR		OpenSound[ MAX_PATH ] = TEXT("\\Windows\\InfBeg.wav") ;
TCHAR		WaitSound[ MAX_PATH ] = TEXT("\\Windows\\MenuSel.wav") ;
TCHAR		OverSound[ MAX_PATH ] = TEXT("\\Windows\\Empty.wav") ;
TCHAR		GoalSound[ MAX_PATH ] = TEXT("\\Windows\\Startup.wav") ;
#endif	/* _WIN32_WCE */

#if	defined(_WIN32_WCE) && !defined(UNDER_NT)
#define		EXT_BMP		TEXT(".2BP")
#else	/* _WIN32_WCE */
#define		EXT_BMP		TEXT(".BMP")
#endif	/* _WIN32_WCE */

typedef enum {
	ANIM_CURSOR, ANIM_GATE, ANIM_BELL,
	ANIM_BLOCK, ANIM_SLIT, ANIM_BALL, ANIM_POLE
} ANIMTYPE ;

typedef struct {
	ANIMTYPE	type ;
	BOOL		fDraw ;
	DWORD		x, y ;
	DWORD		w, h ;
	DWORD		tick ;
	DWORD		state ;
	DWORD		cur_state ;
	DWORD		w2, h2 ;
	struct {
		DWORD	lx, ly, lw, lh ;
		DWORD	cur_tick ;
	} v ;
} ANIMATION, *PANIMATION ;

RECT		StartRect = { 0, 0, 31, 23 } ;
RECT		GoalRect = { 448, 176, 479, 199 } ;
ANIMATION	Animation[100] = {
	{ ANIM_CURSOR, FALSE,   0,   0,  4,  4, 1,  4,  0,  5,  5 },
	{ ANIM_GATE,   FALSE,  22,   0, 10, 24, 1, 10,  0,  0,  0 },
	{ ANIM_BELL,    TRUE,   0,   0,  0,  0, 5,  5,  0,  0,  0 },
} ;
#define		INIT_ANIMATION		(3)
DWORD		NumAnimation = INIT_ANIMATION ;

#define		CursorX		(Animation[0].x)
#define		CursorY		(Animation[0].y)
#define		F_Cursor	(Animation[0].fDraw)
#define		GateX		(Animation[1].x)
#define		GateY		(Animation[1].y)
#define		GateW		(Animation[1].w)
#define		GateH		(Animation[1].h)
#define		GateState	(Animation[1].cur_state)
#define		F_Gate		(Animation[1].fDraw)
#define		BellState	(Animation[2].cur_state)

static TBBUTTON tbSTDButton[] = {
	{ 0, 0,             TBSTATE_ENABLED, TBSTYLE_SEP,    0, 0, 0, -1},
	{ 0, IDM_START,     TBSTATE_ENABLED, TBSTYLE_CHECK,  0, 0, 0, -1},
	{ 1, IDM_WALL,      TBSTATE_ENABLED, TBSTYLE_CHECK,  0, 0, 0, -1},
} ;

static	BOOL	init_application( void ) ;
static	BOOL	init_instance( int nCmdShow ) ;
static	void	MainWnd_OnCreate( HWND hWnd, WPARAM wParam, LPARAM lParam ) ;
LRESULT CALLBACK	MainWndFunc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam ) ;

void	draw_base( HWND hWnd, HDC hDC ) ;
BOOL	read_gray_bmp( HWND hWnd, LPCTSTR fn ) ;
BOOL	check_wall( HWND hWnd, DWORD x, DWORD y ) ;
BOOL	check_start( DWORD x, DWORD y ) ;
BOOL	check_goal( DWORD x, DWORD y ) ;
void	init_animation( void ) ;
BOOL	draw_animation( HWND hWnd, HDC hDC, BOOL f_force ) ;
void	AnimationThread( HWND hWnd ) ;

BOOL	initial_read( HWND hWnd ) ;

int WINAPI
#ifdef	_WIN32_WCE
WinMain( HINSTANCE hThisInst, HINSTANCE hPrevInst,
		 LPTSTR lpszArgs, int nWinMode )
#else	/* _WIN32_WCE */
WinMain( HINSTANCE hThisInst, HINSTANCE hPrevInst,
		 LPSTR lpszArgs, int nWinMode )
#endif	/* _WIN32_WCE */
{
	MSG			msg ;
	HACCEL		hAccl ;

	hInst = hThisInst ;

#ifdef	_WIN32_WCE
	InitCommonControls();
#endif	/* _WIN32_WCE */

	/* UnInstallの処理 */
	if ( !tcsicmp( (LPCTSTR) lpszArgs, TEXT("/UNINSTALL") ) ) {
		delete_reg() ;
		goto ExitMain ;
	}

	/* レジストリを読み込む */
	get_reg() ;
	/* コマンドラインにファイル名が指定された場合 */
	if ( *lpszArgs ) {
		_tcscpy( MukaPath, (LPCTSTR) lpszArgs ) ;
	}

	/* ウィンドウクラスの登録 */
	if ( !init_application() ) {
		goto ExitMain ;
	}

	/* メインウィンドウの作成 */
	if ( !init_instance( nWinMode ) ) {
		goto ExitMain ;
	}

	SetCursor( (HCURSOR) IDC_WAIT ) ;

#ifdef	_WIN32_WCE
	hAccl = LoadAccelerators( hInst,
							  MAKEINTRESOURCE(IDR_ACCELERATOR1) ) ;
#else	/* _WIN32_WCE */
	hAccl = LoadAccelerators( GetModuleHandle(NULL),
							  MAKEINTRESOURCE(IDR_ACCELERATOR1) ) ;
#endif	/* _WIN32_WCE */
	SetCursor( NULL ) ;

	PostMessage( hMainWnd, WM_COMMAND, IDM_INIT, 0 ) ;

	while ( GetMessage( &msg, NULL, 0, 0 ) ) {
		if ( !TranslateAccelerator( hMainWnd, hAccl, &msg ) ) {
			TranslateMessage( &msg ) ;
		}
		DispatchMessage( &msg ) ;
	}

	/* レジストリに書き込む */
	set_reg() ;

ExitMain:
	F_quit = TRUE ;
	if ( hAnimThread ) {
		/* スレッドハンドルをクローズする */
		CloseHandle( hAnimThread ) ;
		hAnimThread = 0 ;
	}
	if ( hFont ) {
		DeleteObject( hFont ) ;
	}
	return TRUE ;
}

static	BOOL
init_application( void )
{
	WNDCLASS	wcl ;

	/* メインウィンドウクラスを登録する */
	wcl.hInstance = hInst ;
	wcl.lpszClassName = szWinName ;
	wcl.lpfnWndProc = MainWndFunc ;
	wcl.style = CS_HREDRAW|CS_VREDRAW ;
	wcl.hIcon   = NULL ;
	wcl.hCursor = NULL ;
#ifdef	_WIN32_WCE
	wcl.lpszMenuName = 0 ;
#else	/* _WIN32_WCE */
	wcl.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1) ;
#endif	/* _WIN32_WCE */
	wcl.cbClsExtra = 0 ;
	wcl.cbWndExtra = 0 ;
	wcl.hbrBackground = GetStockObject( WHITE_BRUSH ) ;
	if ( !RegisterClass( &wcl ) ) {
		return FALSE ;
	}
	return TRUE ;
}

static	BOOL
init_instance( int nCmdShow )
{
	LOGFONT			lf ;
	const	DWORD	x0 = 0 ;

	/* メインウィンドウを作成する */
	hMainWnd = CreateWindowEx( 0, szWinName,
						TitleName,
						WS_VISIBLE,
#ifdef	_WIN32_WCE
						0, 0, CW_USEDEFAULT, CW_USEDEFAULT,
#else	/* _WIN32_WCE */
						0, 0, 480, 240,
#endif	/* _WIN32_WCE */
						NULL,
						NULL,
						hInst,
						NULL ) ;
	if ( !hMainWnd ) {
		return FALSE;
	}
	hMsgWnd = CreateWindow( TEXT("STATIC"), TEXT(""),
					WS_CHILD|SS_CENTER|WS_BORDER,
					120, 70, 240, 36,
					hMainWnd, NULL, hInst, NULL ) ;
	if ( !hMsgWnd ) {
		return FALSE;
	}
	memset( &lf, 0, sizeof lf ) ;
	lf.lfPitchAndFamily = FIXED_PITCH | FF_MODERN ;
	lf.lfWeight = FW_BOLD ;
	lf.lfHeight = 20 ;
	hFont = CreateFontIndirect( &lf ) ;
	if ( hFont ) {
		SendMessage( hMsgWnd, WM_SETFONT, (WPARAM) hFont, MAKELPARAM(FALSE,0) ) ;
	}

#ifdef	_WIN32_WCE
	/* コマンドバーを作成する */
	hCmdBar = CommandBar_Create( hInst, hMainWnd, 1 ) ;
	/* メニューをつける */
	CommandBar_InsertMenubar( hCmdBar, hInst, IDR_MENU1, 0 ) ;
	/* コマンドボタンをつける */
	CommandBar_AddBitmap( hCmdBar, hInst, IDB_BUTTONS,
						  8, 16, 16 ) ;
	CommandBar_AddButtons( hCmdBar, sizeof(tbSTDButton) / sizeof(TBBUTTON),
						   tbSTDButton ) ;
	/* 終了ボタンをつける */
	CommandBar_AddAdornments( hCmdBar, 0, IDM_EXIT ) ;
	WindowTopY = CommandBar_Height( hCmdBar ) ;
#endif	/* _WIN32_WCE */

	/* メインウィンドウを表示する */
	ShowWindow( hMainWnd, nCmdShow ) ;
	UpdateWindow( hMainWnd ) ;

	return TRUE ;
}

LRESULT CALLBACK
MainWndFunc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	PAINTSTRUCT	ps ;
	DWORD		cmd ;
	HDC			hDC ;
	RECT		rect ;
	DWORD		fwKeys, x, y, tmp ;

	switch ( message ) {
	case WM_DESTROY:
		if ( hBmp ) {
			DeleteObject( hBmp ) ;
			hBmp = 0 ;
		}
		PostQuitMessage( 0 ) ;
		break ;
	case WM_PAINT:
		if ( F_quit ) {
			hDC = BeginPaint( hWnd, &ps ) ;
			draw_base( hWnd, hDC ) ;
			draw_animation( hWnd, hDC, TRUE ) ;
			EndPaint( hWnd, &ps ) ;
		}
		return DefWindowProc( hWnd, message, wParam, lParam ) ;
	case WM_LBUTTONDOWN:
		fwKeys = wParam	;
		x = LOWORD(lParam) ;
		y = HIWORD(lParam) ;

		/* 実行中ならなにもしない */
		if ( !F_quit ) {
			break ;
		}

		/* 開始位置かどうかチェックする */
		if ( !check_start( x, y ) ) {
			break ;
		}

		CursorX = x, CursorY = y ;

		/* メッセージを消す */
		ShowWindow( hMsgWnd, SW_HIDE ) ;
		PostMessage( hWnd, WM_COMMAND, IDM_GO, 0 ) ;
		break ;

	case WM_LBUTTONUP:
		if ( F_quit ) {
			break ;
		}

		fwKeys = wParam	;
		x = LOWORD(lParam) ;
		y = HIWORD(lParam) ;
		/* カーソル描画を止める */
		F_Cursor = FALSE ;
		/* ゲームオーバー */
		F_quit = TRUE ;
		if ( check_start( x, y ) ) {
			PostMessage( hWnd, WM_COMMAND, IDM_HELP, 0 ) ;
		} else {
			PostMessage( hWnd, WM_COMMAND, IDM_GAMEOVER, 0 ) ;
		}
		break ;

	case WM_MOUSEMOVE:
		fwKeys = wParam	;
		x = LOWORD(lParam) ;
		y = HIWORD(lParam) ;
		if ( F_quit ) {
			break ;
		}
		if ( fwKeys & MK_LBUTTON ) {
			CursorX = x, CursorY = y ;
			if ( check_wall( hWnd, x, y ) ) {
				F_quit = TRUE ;
				PostMessage( hWnd, WM_COMMAND, IDM_GAMEOVER, 0 ) ;
			} else if ( check_goal( x, y ) ) {
				F_quit = TRUE ;
				PostMessage( hWnd, WM_COMMAND, IDM_GOAL, 0 ) ;
			}
		}
		break ;
	case WM_COMMAND:
		cmd = GET_WM_COMMAND_ID( wParam, lParam ) ;
		switch ( cmd ) {
		case IDM_EXIT:
			SendMessage( hWnd, WM_CLOSE, 0, 0 ) ;
			break ;
		case IDM_OPEN:
			/* 強制的にダイアログを開くようにする */
			*MukaPath = 0 ;
			// break ;
		case IDM_INIT:
			ShowWindow( hMsgWnd, SW_HIDE ) ;
			if ( !initial_read( hWnd ) ) {
				/* 読み込めなかった場合 */
				SendMessage( hWnd, WM_CLOSE, 0, 0 ) ;
				break ;
			}
			break ;
		case IDM_HELP:
			/* アニメーションスレッドをクローズする */
			if ( hAnimThread ) {
				CloseHandle( hAnimThread ) ;
				hAnimThread = 0 ;
			}
			_tcscpy( MessageBuf, TEXT("Tap left-top-corner on screen.\r\n") ) ;
			_tcscat( MessageBuf, TEXT("Drag a circle through white-road.\r\n") ) ;
			_tcscat( MessageBuf, TEXT("Go to right-bottom-corner.") ) ;
			SetWindowText( hMsgWnd, MessageBuf ) ;
			GetClientRect( hWnd, &rect ) ;
			x = (rect.right - rect.left + 1 - 320) / 2 ;
			y = (rect.bottom - rect.top + 1 - 50) / 2 ;
			MoveWindow( hMsgWnd, x, y, 320, 50, FALSE ) ;
			ShowWindow( hMsgWnd, SW_SHOW ) ;
			break ;
		case IDM_GO:
			/* メッセージ表示域をValidateする */
			GetWindowRect( hMsgWnd, &rect ) ;
			ValidateRect( hWnd, &rect ) ;

			/* 物体の表示を初期状態にする */
			GateState = BellState = 0 ;
			F_Cursor = TRUE ;
			F_gateEnable = FALSE ;

			/* 画面を更新する */
			hDC = GetDC( hWnd ) ;
			draw_base( hWnd, hDC ) ;
			draw_animation( hWnd, hDC, TRUE ) ;
			ReleaseDC( hWnd, hDC ) ;

			/* アニメーションを開始させる */
			if ( hAnimThread ) {
				CloseHandle( hAnimThread ) ;
				hAnimThread = 0 ;
			}
			F_quit = FALSE ;
			hAnimThread = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE) AnimationThread, hMainWnd, 0, &AnimThreadID ) ;
			if ( hAnimThread == NULL ) {
				break ;
			}

			break ;
		case IDM_GAMEOVER:
			/* アニメーションスレッドをクローズする */
			if ( hAnimThread ) {
				CloseHandle( hAnimThread ) ;
				hAnimThread = 0 ;
			}
			SetWindowText( hMsgWnd, TEXT("GAME OVER") ) ;
			GetClientRect( hWnd, &rect ) ;
			x = (rect.right - rect.left + 1 - 100) / 2 ;
			y = (rect.bottom - rect.top + 1 - 20) / 2 ;
			MoveWindow( hMsgWnd, x, y, 100, 20, FALSE ) ;
			ShowWindow( hMsgWnd, SW_SHOW ) ;
#ifdef	_WIN32_WCE
			sndPlaySound( OverSound, SND_ASYNC ) ;
#else	/* _WIN32_WCE */
			MessageBeep( MB_ICONASTERISK ) ;
#endif	/* _WIN32_WCE */
			break ;
		case IDM_GOAL:
			/* アニメーションスレッドをクローズする */
			if ( hAnimThread ) {
				CloseHandle( hAnimThread ) ;
				hAnimThread = 0 ;
			}
			SetWindowText( hMsgWnd, TEXT("Congratulation!!!") ) ;
			GetClientRect( hWnd, &rect ) ;
			x = (rect.right - rect.left + 1 - 200) / 2 ;
			y = (rect.bottom - rect.top + 1 - 20) / 2 ;
			MoveWindow( hMsgWnd, x, y, 200, 20, FALSE ) ;
			ShowWindow( hMsgWnd, SW_SHOW ) ;
#ifdef	_WIN32_WCE
			sndPlaySound( GoalSound, SND_ASYNC ) ;
#else	/* _WIN32_WCE */
			MessageBeep( MB_ICONASTERISK ) ;
#endif	/* _WIN32_WCE */
			break ;
#ifdef	_WIN32_WCE
		case IDM_START:
			/* オープニングスキップ切り替え */
			tmp = SendMessage( hCmdBar, TB_GETSTATE, (WPARAM) IDM_START, (LPARAM) 0 ) ;
			if ( tmp != -1 ) {
				F_skipstart = tmp & TBSTATE_CHECKED ;
			}
			return TRUE ;
		case IDM_WALL:
			/* 壁なし切り替え */
			tmp = SendMessage( hCmdBar, TB_GETSTATE, (WPARAM) IDM_WALL, (LPARAM) 0 ) ;
			if ( tmp != -1 ) {
				F_nowall = tmp & TBSTATE_CHECKED ;
			}
			return TRUE ;
#endif	/* _WIN32_WCE */
		case IDM_ABOUT:
			/* Aboutダイアログを表示する */
			DialogBox( hInst, MAKEINTRESOURCE(IDD_DLG_ABOUT), hWnd, AboutDlgFunc ) ;
			break ;
		}
		break ;
	case WM_CLOSE:
		DestroyWindow( hWnd ) ;
		break;
	default:
		return DefWindowProc( hWnd, message, wParam, lParam ) ;
	}
	return 0 ;
}

void
draw_base( HWND hWnd, HDC hDC )
{
	DWORD	rc ;
	RECT	rect ;
	HBITMAP	oldBmp ;
	HDC		hdcBits ;

	GetClientRect( hWnd, &rect ) ;
	if ( hBmp ) {
		hdcBits = CreateCompatibleDC( hDC ) ;
		oldBmp = SelectObject( hdcBits, hBmp ) ;
		rc = BitBlt( hDC, 0, 0 + WindowTopY, rect.right - rect.left + 1, rect.bottom - rect.top + 1, hdcBits, 0, 0, SRCCOPY ) ;
		SelectObject( hdcBits, oldBmp ) ;
		DeleteDC( hdcBits ) ;
	} else {
		FillRect( hDC, &rect, GetStockObject( WHITE_BRUSH ) ) ;
	}
}

static	BITMAPFILEHEADER	bmfh ;
static	struct {
	BITMAPINFOHEADER		bmih ;
	RGBQUAD					rgq[ 16 ] ;
} bmi ;

/* 壁用のビットマップファイルを読み込む */
BOOL
read_gray_bmp( HWND hWnd, LPCTSTR fn )
{
	HDC		hDC ;
	HANDLE	hFile ;
	HBITMAP	hNewBmp = 0 ;
	LPBYTE	bmpbuf, ptr ;
	BOOL	ret = FALSE ;
	DWORD	nRead, y, sw, bpp ;

	hFile = CreateFile( fn, GENERIC_READ, FILE_SHARE_READ, NULL,
						OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 ) ;
	if ( hFile == INVALID_HANDLE_VALUE ) {
		return FALSE ;
	}
	ReadFile( hFile, &bmfh, sizeof bmfh, &nRead, NULL ) ;
	if ( memcmp( &bmfh, "BM", 2 ) ) {
		goto error ;
	}
	ReadFile( hFile, &bmi.bmih, sizeof bmi.bmih, &nRead, NULL ) ;
	bpp = 8 / bmi.bmih.biBitCount ;
	sw = ((bmi.bmih.biWidth + bpp - 1) / bpp + 3) / 4 * 4 ;
#if	defined(_WIN32_WCE) && !defined(UNDER_NT)
	if ( bmi.bmih.biBitCount != 2 ) {
		goto error ;
	}
	bmi.bmih.biSize = sizeof (bmi.bmih) ;
	bmi.bmih.biPlanes = 1 ;
	bmi.bmih.biBitCount = 2 ;
	bmi.bmih.biCompression = BI_RGB ;
	bmi.bmih.biSizeImage = 0 ;
	bmi.bmih.biXPelsPerMeter = 0 ;
	bmi.bmih.biYPelsPerMeter = 0 ;
	bmi.bmih.biClrUsed = 0 ;
	bmi.bmih.biClrImportant = 0 ;
	bmi.rgq[0].rgbBlue = bmi.rgq[0].rgbGreen = bmi.rgq[0].rgbRed = 0x00 ;
	bmi.rgq[1].rgbBlue = bmi.rgq[1].rgbGreen = bmi.rgq[1].rgbRed = 0x55 ;
	bmi.rgq[2].rgbBlue = bmi.rgq[2].rgbGreen = bmi.rgq[2].rgbRed = 0xaa ;
	bmi.rgq[3].rgbBlue = bmi.rgq[3].rgbGreen = bmi.rgq[3].rgbRed = 0xFF ;

	hDC = GetDC( hWnd ) ;
	hNewBmp = CreateDIBSection( hDC, (PBITMAPINFO) &bmi.bmih, DIB_RGB_COLORS, &bmpbuf, NULL, 0 ) ;
	if ( hNewBmp == NULL ) {
		ReleaseDC( hWnd, hDC ) ;
		goto error ;
	}
	ReleaseDC( hWnd, hDC ) ;
	SetFilePointer( hFile, bmfh.bfOffBits, NULL, FILE_BEGIN ) ;
	ptr = bmpbuf ;
	for ( y = 0 ; y < (DWORD) bmi.bmih.biHeight ; y ++, ptr += sw ) {
		ReadFile( hFile, ptr, sw, &nRead, NULL ) ;
	}
#else	/* _WIN32_WCE */
	if ( bmi.bmih.biBitCount != 4 ) {
		goto error ;
	}
	bmi.bmih.biSize = sizeof (bmi.bmih) ;
	bmi.bmih.biPlanes = 1 ;
	bmi.bmih.biBitCount = 4 ;
	bmi.bmih.biCompression = BI_RGB ;
	bmi.bmih.biSizeImage = 0 ;
	bmi.bmih.biXPelsPerMeter = 0 ;
	bmi.bmih.biYPelsPerMeter = 0 ;
	bmi.bmih.biClrUsed = 0 ;
	bmi.bmih.biClrImportant = 0 ;
	ReadFile( hFile, &bmi.rgq, sizeof (RGBQUAD) * 16, &nRead, NULL ) ;
	hDC = GetDC( hWnd ) ;
	hNewBmp = CreateDIBSection( hDC, (PBITMAPINFO) &bmi.bmih, DIB_RGB_COLORS, &bmpbuf, NULL, 0 ) ;
	if ( hNewBmp == NULL ) {
		ReleaseDC( hWnd, hDC ) ;
		goto error ;
	}
	ReleaseDC( hWnd, hDC ) ;
	ptr = bmpbuf ;
	SetFilePointer( hFile, bmfh.bfOffBits, NULL, FILE_BEGIN ) ;
	for ( y = 0 ; y < (DWORD) bmi.bmih.biHeight ; y ++, ptr += sw ) {
		ReadFile( hFile, ptr, sw, &nRead, NULL ) ;
	}
#endif	/* _WIN32_WCE */
	BmpWidth = (DWORD) bmi.bmih.biWidth ;
	BmpHeight = (DWORD) bmi.bmih.biHeight ;
	if ( hBmp ) {
		DeleteObject( hBmp ) ;
	}
	hBmp = hNewBmp ;

	ret = TRUE ;
error:
	CloseHandle( hFile ) ;
	return ret ;
}

/* 壁と接触しているか調べる */
BOOL
check_wall( HWND hWnd, DWORD x, DWORD y )
{
	DWORD	rc ;
	HBITMAP	oldBmp ;
	HDC		hDC, hdcBits ;

	if ( F_nowall ) {
		return FALSE ;
	}
	hDC     = GetDC( hWnd ) ;
	hdcBits = CreateCompatibleDC( hDC ) ;
	oldBmp  = SelectObject( hdcBits, hBmp ) ;
	rc      = GetBValue( GetPixel( hdcBits, x, y - WindowTopY ) ) ;
	SelectObject( hdcBits, oldBmp ) ;
	DeleteDC( hdcBits ) ;
	ReleaseDC( hWnd, hDC ) ;
	return rc != 0xFF ;
}

/* 出発地点にいるかどうか調べる */
BOOL
check_start( DWORD x, DWORD y )
{
	POINT	point ;
	point.x = x, point.y = y ;
	return PtInRect( &StartRect, point ) ;
}

/* ゴールにいるかどうか調べる */
BOOL
check_goal( DWORD x, DWORD y )
{
	POINT	point ;
	point.x = x, point.y = y ;
	return PtInRect( &GoalRect, point ) ;
}

/* アニメーションデータを初期化する */
void
init_animation( void )
{
	PANIMATION	anim = &Animation[INIT_ANIMATION] ;
	DWORD		count = NumAnimation - INIT_ANIMATION ;

	F_Gate = TRUE ;
	/* スタート領域・ゴール領域の値を補正する */
	StartRect.top    += WindowTopY ;
	StartRect.bottom += WindowTopY ;
	GoalRect.top     += WindowTopY ;
	GoalRect.bottom  += WindowTopY ;
	/* ゲートの座標を決定する */
	GateX = StartRect.right - GateW + 1 ;
	GateY = StartRect.top ;
	GateH = StartRect.bottom - StartRect.top + 1 ;
	do {
		anim->y += WindowTopY ;
		anim->v.lx = anim->v.ly = anim->v.lw = anim->v.lh = 0 ;
		anim->v.cur_tick = 0 ;
	} while ( anim ++, -- count ) ;
}

BOOL
draw_animation( HWND hWnd, HDC hDC, BOOL f_force )
{
	HPEN		oldPen = 0 ;
	HBRUSH		oldBrush = 0 ;
	BOOL		f_touch = FALSE ;
	PANIMATION	anim  = Animation ;
	DWORD		count = NumAnimation ;
	RECT		rectCursor, rectObj, rect ;
	DWORD		tmp, tmp2, x, y, w, h, w2, h2 ;

	oldBrush = SelectObject( hDC, GetStockObject( WHITE_BRUSH ) ) ;
	oldPen   = SelectObject( hDC, GetStockObject( BLACK_PEN ) ) ;
	do {
		if ( !anim->fDraw ) {
			continue ;
		}
		if ( !f_force && (++ anim->v.cur_tick < anim->tick) ) {
			continue ;
		}
		anim->v.cur_tick = 0 ;
		x = anim->x, y = anim->y, w = anim->w, h = anim->h ;
		w2 = anim->w2, h2 = anim->h2 ;

		if ( anim->type != ANIM_CURSOR && anim->type != ANIM_BALL && anim->v.lw && anim->v.lh ) {
			SelectObject( hDC, GetStockObject( WHITE_BRUSH ) ) ;
			PatBlt( hDC, anim->v.lx, anim->v.ly, anim->v.lw, anim->v.lh, PATCOPY ) ;
		}
		switch ( anim->type ) {
		case ANIM_CURSOR:
			tmp = 8 ;
			SelectObject( hDC, GetStockObject( WHITE_PEN ) ) ;
			Ellipse( hDC, anim->v.lx, anim->v.ly, anim->v.lx + anim->v.lw - 1, anim->v.ly + anim->v.lh - 1 ) ;
			x -= tmp / 2, y -= tmp / 2 ;
			/* カーソルは最後に描く */
			anim->v.lx = x, anim->v.ly = y, anim->v.lw = tmp, anim->v.lh = tmp ;
			/* カーソルの位置 */
			rectCursor.left = anim->x - 2 ;
			rectCursor.top = anim->y - 2 ;
			rectCursor.right = anim->x + 2 ;
			rectCursor.bottom = anim->y + 2 ;
			break ;

		case ANIM_GATE:
			if ( anim->cur_state >= anim->state - 1 ) {
				/* 扉が開いたら何もしない */
				anim->cur_state = anim->state - 2 ;
				break ;
			}
			h -= h * anim->cur_state / (anim->state - 1) ;
			SelectObject( hDC, GetStockObject( BLACK_BRUSH ) ) ;
			PatBlt( hDC, x, y, w, h, PATCOPY ) ;
			anim->v.lx = x, anim->v.ly = y, anim->v.lw = w, anim->v.lh = h ;

			/* 接触チェック */
			rectObj.left = x, rectObj.top = y ;
			rectObj.right = x + w - 1, rectObj.bottom = y + h - 1 ;
			f_touch |= IntersectRect( &rect, &rectCursor, &rectObj ) ;

			if ( !F_gateEnable ) {
				continue ;
			}

			break ;

		case ANIM_BELL:
			if ( f_force ) {
				break ;
			}
			if ( anim->cur_state == anim->state - 2 ) {
#ifdef	_WIN32_WCE
				sndPlaySound( OpenSound, SND_ASYNC ) ;
#else	/* _WIN32_WCE */
				MessageBeep( MB_OK ) ;
#endif	/* _WIN32_WCE */
				StartTime = GetTickCount() ;
				F_gateEnable = TRUE ;
			} else if ( anim->cur_state >= anim->state - 1 ) {
				/* Stateが終わったらなにもしない */
				anim->cur_state = anim->state - 2 ;
				break ;
			} else {
#ifdef	_WIN32_WCE
				sndPlaySound( WaitSound, SND_ASYNC ) ;
#else	/* _WIN32_WCE */
				MessageBeep( MB_OK ) ;
#endif	/* _WIN32_WCE */
			}
			break ;

		case ANIM_BLOCK:
			/* 上下するブロック */
			tmp = (h - h2) * 2 ;
			if ( anim->cur_state < anim->state / 2 ) {
				tmp *= anim->cur_state ;
			} else {
				tmp *= (anim->state - anim->cur_state) ;
			}
			tmp /= anim->state ;
			y += tmp ;
			SelectObject( hDC, GetStockObject( BLACK_BRUSH ) ) ;
			PatBlt( hDC, x, y, w2, h2, PATCOPY ) ;
			anim->v.lx = x, anim->v.ly = y, anim->v.lw = w2, anim->v.lh = h2 ;

			/* 接触チェック */
			rectObj.left = x, rectObj.top = y ;
			rectObj.right = x + w2 - 1, rectObj.bottom = y + h2 - 1 ;
			f_touch |= IntersectRect( &rect, &rectCursor, &rectObj ) ;

			break ;

		case ANIM_SLIT:
			SelectObject( hDC, GetStockObject( BLACK_BRUSH ) ) ;
			tmp = h ;
			tmp *= w2 ? (anim->state - anim->cur_state - 1) : anim->cur_state ;
			tmp /= anim->state ;
			if ( tmp + h2 <= h ) {
				if ( tmp ) {
					PatBlt( hDC, x, y, w, tmp, PATCOPY ) ;
					/* 接触チェック */
					rectObj.left = x, rectObj.top = y ;
					rectObj.right = x + w - 1, rectObj.bottom = y + tmp - 1 ;
					f_touch |= IntersectRect( &rect, &rectCursor, &rectObj ) ;
				}
				PatBlt( hDC, x, y + tmp + h2, w, h - (tmp + h2), PATCOPY ) ;
				/* 接触チェック */
				rectObj.left = x, rectObj.top = y + tmp + h2;
				rectObj.right = x + w - 1, rectObj.bottom = y + h - 1 ;
				f_touch |= IntersectRect( &rect, &rectCursor, &rectObj ) ;
			} else {
				PatBlt( hDC, x, y + tmp + h2 - h, w, h - h2, PATCOPY ) ;
				/* 接触チェック */
				rectObj.left = x, rectObj.top = y + tmp + h2 - h ;
				rectObj.right = x + w - 1, rectObj.bottom = y + tmp - 1  ;
				f_touch |= IntersectRect( &rect, &rectCursor, &rectObj ) ;
			}
			anim->v.lx = x, anim->v.ly = y, anim->v.lw = w, anim->v.lh = h ;
			break ;

		case ANIM_BALL:
			tmp = anim->state / 10 ;
			if ( !tmp ) {
				break ;
			}
			tmp2 = w / (tmp * 8) ;
			if ( anim->cur_state < tmp ) {
				x += w - w2 ;
				y += tmp2 * (tmp - anim->cur_state - 1) ;
			} else if ( anim->cur_state >= anim->state - tmp ) {
				y += tmp2 * (anim->cur_state - anim->state + tmp) ;
			} else {
				tmp2 = (w - w2) * (anim->cur_state - tmp) ;
				tmp2 /= anim->state - tmp - tmp ;
				x += w - w2 - tmp2 ;
			}
			/* 古い位置の玉を消す */
			SelectObject( hDC, GetStockObject( WHITE_PEN ) ) ;
			SelectObject( hDC, GetStockObject( WHITE_BRUSH ) ) ;
			Ellipse( hDC, anim->v.lx, anim->v.ly, anim->v.lx + anim->v.lw - 1, anim->v.ly + anim->v.lh - 1 ) ;
			/* 新しい位置に描く */
			SelectObject( hDC, GetStockObject( BLACK_PEN ) ) ;
			SelectObject( hDC, GetStockObject( BLACK_BRUSH ) ) ;
			Ellipse( hDC, x, y, x + w2 - 1, y + w2 - 1 ) ;
			/* 新しい位置を記録 */
			anim->v.lx = x, anim->v.ly = y, anim->v.lw = w2, anim->v.lh = w2 ;
			/* 接触チェック */
			x += w2 / 2, y += w2 / 2 ;
			tmp2 = (x - rectCursor.left - 2) ;
			tmp  = tmp2 * tmp2 ;
			tmp2 = (y - rectCursor.top - 2) ;
			tmp += tmp2 * tmp2 ;
			tmp2 = w2 / 2 + 2 ;
			f_touch |= tmp < tmp2 * tmp2 ;
			break ;

		case ANIM_POLE:
			/* 伸び縮み棒 */
			tmp = anim->w ;
			if ( anim->cur_state < anim->state / 4 ) {
				tmp = 0 ;
			} else if ( anim->cur_state < anim->state / 2 ) {
				tmp *= anim->cur_state - anim->state / 4 ;
				tmp /= anim->state / 4 ;
			} else {
				tmp *= anim->state - anim->cur_state ;
				tmp /= anim->state / 2 ;
			}
			if ( w2 ) {
				/* 右→左 */
				x = x + w - tmp ;
			}
			w = tmp ;
			SelectObject( hDC, GetStockObject( BLACK_BRUSH ) ) ;
			PatBlt( hDC, x, y, w, h, PATCOPY ) ;
			anim->v.lx = x, anim->v.ly = y, anim->v.lw = w, anim->v.lh = h ;
			/* 接触チェック */
			rectObj.left = x, rectObj.top = y ;
			rectObj.right = x + w - 1, rectObj.bottom = y + h - 1 ;
			f_touch |= IntersectRect( &rect, &rectCursor, &rectObj ) ;

			break ;

		}
		if ( !f_force ) {
			anim->cur_state = (anim->cur_state + 1) % anim->state ;
		}
	} while ( anim ++, -- count ) ;

	/* カーソルを描く */
	anim = &Animation[0] ;
	if ( anim->fDraw ) {
		SelectObject( hDC, GetStockObject( BLACK_PEN ) ) ;
		SelectObject( hDC, GetStockObject( WHITE_BRUSH ) ) ;
		Ellipse( hDC, anim->v.lx, anim->v.ly, anim->v.lx + anim->v.lw - 1, anim->v.ly + anim->v.lh - 1 ) ;
	}
	SelectObject( hDC, oldBrush ) ;
	SelectObject( hDC, oldPen ) ;
	return f_touch ;
}

void
AnimationThread( HWND hWnd )
{
	HDC			hDC ;
	RECT		rect ;
	TCHAR		buf[ 20 ] ;
	DWORD		elapse_min, elapse_sec, elapse_msec ;
	DWORD		enterTime = GetTickCount() ;

	/* 開始時刻をクリアする */
	StartTime = 0 ;
	/* 時計表示領域を作成する */
	rect.left = 400, rect.top = 0, rect.right = 479, rect.bottom = 15 ;
	rect.top += WindowTopY ;
	rect.bottom += WindowTopY ;
	/* 接触フラグをクリアする */
	F_touch_object = FALSE ;
	/* アニメーション・メイン・ループ */
	while ( !F_quit ) {
		hDC = GetDC( hWnd ) ;
		/* 全てのアイテムを表示する */
		F_touch_object = draw_animation( hWnd, hDC, FALSE ) ;
		/* 経過時間を表示する */
		CurTime = GetTickCount() ;
		if ( StartTime ) {
			elapse_min  = CurTime - StartTime ;
			elapse_msec = elapse_min % 1000 ;	elapse_min /= 1000 ;
			elapse_sec  = elapse_min % 60 ;		elapse_min /= 60 ;
			wsprintf( buf, TEXT("%02d:%02d:%02d"),
					  elapse_min, elapse_sec, elapse_msec ) ;
			DrawText( hDC, buf, 8, &rect, DT_CENTER ) ;
		}
		ReleaseDC( hWnd, hDC ) ;
		if ( F_touch_object ) {
			F_quit = TRUE ;
			PostMessage( hWnd, WM_COMMAND, IDM_GAMEOVER, 0 ) ;
			break ;
		}
		Sleep( TickTime ) ;
	}
	ExitThread( 0 ) ;
}

struct {
	char	type[ 1 ] ;		/* 'sgBSO' */
	char	x[ 3 ] ;
	char	y[ 3 ] ;
	char	w[ 3 ] ;
	char	h[ 3 ] ;
	char	dummy1[ 1 ] ;
	char	tick[ 3 ] ;
	char	state[ 3 ] ;
	char	cur_state[ 3 ] ;
	char	dummy2[ 1 ] ;
	char	w2[ 3 ] ;
	char	h2[ 3 ] ;
	char	crlf[ 2 ] ;
} MukaItem ;

DWORD
natol3( char *ptr )
{
	DWORD	ret = (*ptr++) - '0' ;
	ret = ret * 10 + (*ptr++) - '0' ;
	ret = ret * 10 + (*ptr++) - '0' ;
	return ret ;
}

BOOL
read_muka_file( LPCTSTR fn )
{
	BOOL		ret ;
	ANIMTYPE	type ;
	PANIMATION	anim ;
	HANDLE		hFile ;
	DWORD		nRead ;

	F_Cursor = F_Gate = FALSE ;
	StartRect.left  = 0,   StartRect.top    = 0 ;
	StartRect.right = 31,  StartRect.bottom = 23 ;
	GoalRect.left   = 448, GoalRect.top     = 176 ;
	GoalRect.right  = 479, GoalRect.bottom  = 199 ;
	NumAnimation = INIT_ANIMATION ;
	anim = &Animation[ NumAnimation ] ;
	hFile = CreateFile( fn, GENERIC_READ, FILE_SHARE_READ, NULL,
						OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 ) ;
	if ( hFile == INVALID_HANDLE_VALUE ) {
		return FALSE ;
	}
	while ( 1 ) {
		ret = ReadFile( hFile, &MukaItem, sizeof MukaItem, &nRead, NULL ) ;
		if ( !ret || nRead != sizeof MukaItem ) {
			break ;
		}
		if ( *MukaItem.type == 's' ) {
			StartRect.left   = natol3( MukaItem.x ) ;
			StartRect.top    = natol3( MukaItem.y ) ;
			StartRect.right  = StartRect.left + natol3( MukaItem.w ) ;
			StartRect.bottom = StartRect.top  + natol3( MukaItem.h ) ;
			continue ;
		} else if ( *MukaItem.type == 'g' ) {
			GoalRect.left   = natol3( MukaItem.x ) ;
			GoalRect.top    = natol3( MukaItem.y ) ;
			GoalRect.right  = GoalRect.left + natol3( MukaItem.w ) ;
			GoalRect.bottom = GoalRect.top  + natol3( MukaItem.h ) ;
			continue ;
		} else if ( *MukaItem.type == 'B' ) {
			type = ANIM_BLOCK ;
		} else if ( *MukaItem.type == 'S' ) {
			type = ANIM_SLIT ;
		} else if ( *MukaItem.type == 'O' ) {
			type = ANIM_BALL ;
		} else if ( *MukaItem.type == 'P' ) {
			type = ANIM_POLE ;
		} else {
			continue ;
		}
		anim->type = type ;
		anim->fDraw = TRUE ;
		anim->x = natol3( MukaItem.x ) ;
		anim->y = natol3( MukaItem.y ) ;
		anim->w = natol3( MukaItem.w ) ;
		anim->h = natol3( MukaItem.h ) ;
		anim->tick = natol3( MukaItem.tick ) ;
		anim->state = natol3( MukaItem.state ) ;
		anim->cur_state = natol3( MukaItem.cur_state ) ;
		anim->w2 = natol3( MukaItem.w2 ) ;
		anim->h2 = natol3( MukaItem.h2 ) ;
		anim ++ ;
		NumAnimation ++ ;
	}
	CloseHandle( hFile ) ;
	return TRUE ;
}

/* イライラデータと壁ビットマップを読み込む */
BOOL
read_all_data( HWND hWnd )
{
	/* 古いビットマップは破棄する */
	if ( hBmp ) {
		DeleteObject( hBmp ) ;
		hBmp = 0 ;
	}
	/* イライラファイルを読み込む */
	if ( !read_muka_file( MukaPath ) ) {
		return FALSE ;
	}
	/* ビットマップファイル名を作成する */
	_tcscpy( BmpPath, MukaPath ) ;
	_tcscpy( &BmpPath[ _tcslen( BmpPath ) - 4 ], EXT_BMP ) ;
	/* 壁ビットマップを読み込む */
	if ( !read_gray_bmp( hWnd, BmpPath ) ) {
		return FALSE ;
	}
	/* 読み込んだファイル名を保存する */
	_tcscpy( LastPath, MukaPath ) ;
	/* アニメーションデータを初期化する */
	init_animation() ;
	/* 画面の再描画を行う */
	InvalidateRect( hWnd, NULL, TRUE ) ;
	return TRUE ;
}

BOOL
initial_read( HWND hWnd )
{
	while ( 1 ) {
		if ( !*MukaPath ) {
			if ( !FileOpenDlg( hWnd, MukaPath, sizeof MukaPath ) ) {
				/* キャンセルの場合は最後に読み込んだファイルを読み直す */
				if ( !*LastPath ) {
					return FALSE ;
				}
				_tcscpy( MukaPath, LastPath ) ;
				return read_all_data( hWnd ) ;
			}
		}
		/* データを読み込む */
		if ( !read_all_data( hWnd ) ) {
			*MukaPath = 0 ;
			continue ;
		}
		/* 初期ディレクトリ設定 */
		setup_init_dir( MukaPath ) ;
		/* 正常終了 */
		break ;
	}
	return TRUE ;
}
