#define		STRICT
#ifndef		UNICODE
#define		UNICODE
#endif		/* UNICODE */
#ifndef		_UNICODE
#define		_UNICODE
#endif		/* _UNICODE */
#include	<windows.h>
#include	<windowsx.h>
#include	<commdlg.h>
#include	<tchar.h>
#include	"resource.h"

HINSTANCE	hInst ;
LPCTSTR		szWinName = TEXT("CaptCE") ;
HWND		hMainWnd ;
HICON		hIcon = 0 ;
DWORD		Argc ;
LPTSTR		Argv[ 10 ] ;

BOOL		F_LoadNotify = FALSE ;
BOOL		F_Dialog = FALSE ;
BOOL		F_NoIcon = FALSE ;
BOOL		F_NoSound = FALSE ;
TCHAR		Message[ 256 ] ;
TCHAR		SaveBase[ MAX_PATH ] = TEXT("\\CAPT") ;
DWORD		g_dwBpp = 4 ;
DWORD		SaveCount = 0 ;
DWORD		g_dwInterval = 500 ;
DWORD		g_dwTick = 60 * 2 ;

DWORD		g_dwTimerCount = 0 ;
BOOL		g_bStartCapture = FALSE ;
BOOL		g_bTimer = FALSE ;
BYTE		g_bKeyStatus = 0 ;

static	BOOL	init_application( void ) ;
static	BOOL	init_instance( int nCmdShow ) ;
static	BOOL	AddTaskBarIcon( HWND hWnd, HICON hIcon, UINT uID, LPCTSTR lpszTip ) ;
static	BOOL	DeleteTaskBarIcon( HWND hWnd, UINT uID ) ;
LRESULT CALLBACK	MainWndFunc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam ) ;
static	BOOL	DesktopToFile( void ) ;
static	BOOL	FileSaveDlg( HWND hWnd, LPTSTR buf, DWORD size ) ;
static	BOOL	find_empty_file( void ) ;
void	cmdline2args( LPTSTR cmdline, LPDWORD argc, LPTSTR *argv, LPTSTR args ) ;

void	StartTimer( HWND hWnd ) ;
void	StopTimer( HWND hWnd ) ;
void	WMTimer( HWND hWnd ) ;
void	ShowIcon( void ) ;
void	HideIcon( void ) ;
BYTE	GetShiftCtrlStatus( void ) ;

int WINAPI
#ifdef	_WIN32_WCE
WinMain( HINSTANCE hThisInst, HINSTANCE hPrevInst, LPTSTR lpszCmdLine, int nCmdShow )
#else	/* _WIN32_WCE */
WinMain( HINSTANCE hThisInst, HINSTANCE hPrevInst, LPSTR lpszCmdLine, int nCmdShow )
#endif	/* _WIN32_WCE */
{
	DWORD	i ;
	HDC		hDC ;
	MSG		msg ;
	BOOL	f_error = FALSE ;

	hInst = hThisInst ;

	if ( FindWindow( szWinName, szWinName ) != NULL ) {
		wsprintf( Message, TEXT("Already loaded") ) ;
		MessageBox( NULL, Message, szWinName, MB_OK|MB_ICONHAND ) ;
		goto ExitMain ;
	}

	hDC = GetDC( NULL ) ;
	g_dwBpp = GetDeviceCaps( hDC, NUMCOLORS ) == 256 ? 8 : 4 ;
	ReleaseDC( NULL, hDC ) ;

	/* コマンドラインの解析 */
	cmdline2args( lpszCmdLine, &Argc, Argv, NULL ) ;
	for ( i = 1 ; i < Argc ; i ++ ) {
		LPCTSTR	arg = Argv[ i ] ;

		if ( !_tcsicmp( arg, TEXT("/DIALOG") ) ) {
			F_Dialog = TRUE ;
		} else if ( !_tcsicmp( arg, TEXT("/NOICON") ) ) {
			F_NoIcon = TRUE ;
		} else if ( !_tcsicmp( arg, TEXT("/2BP") ) ) {
			g_dwBpp = 2 ;
		} else if ( !_tcsicmp( arg, TEXT("/NOSOUND") ) ) {
			F_NoSound = TRUE ;
		} else {
			f_error = TRUE ;
		}
	}
	if ( f_error ) {
		wsprintf( Message,
				  TEXT("Usage:\r\n  %s [options]\r\n\r\nOptions:\r\n%s\r\n%s\r\n%s"),
				  szWinName,
				  TEXT("  /DIALOG = Use save-dialog"),
				  TEXT("  /2BP = Save to .2BP file"),
				  TEXT("  /NOSOUND = Don't use capture-sound") ) ;
		MessageBox( NULL, Message, szWinName, MB_OK|MB_ICONHAND ) ;
		goto ExitMain ;
	}

	/* ウィンドウクラスを登録する */
	if ( !init_application() ) {
		goto ExitMain ;
	}

	/* ウィンドウを作成する */
	if ( !init_instance( nCmdShow ) ) {
		goto ExitMain ;
	}

	while ( GetMessage( &msg, NULL, 0, 0 ) ) {
		TranslateMessage( &msg ) ;
		DispatchMessage( &msg ) ;
	}

ExitMain:
	HideIcon() ;
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
	wcl.hIcon = NULL ;
	wcl.hCursor = NULL ;
	wcl.lpszMenuName = 0 ;
	wcl.cbClsExtra = 0 ;
	wcl.cbWndExtra = 0 ;
	wcl.hbrBackground = GetStockObject( WHITE_BRUSH ) ;
	if ( !RegisterClass( &wcl ) ) {
		wsprintf( Message, TEXT("RegisterClass:%d"), GetLastError() ) ;
		MessageBox( NULL, Message, szWinName, MB_OK ) ;
		return FALSE ;
	}
	return TRUE ;
}

static	BOOL
init_instance( int nCmdShow )
{
	/* メインウィンドウを作成する */
	hMainWnd = CreateWindowEx( 0, szWinName, szWinName, WS_CAPTION,
#ifdef	_WIN32_WCE
						 0, 0, 240, 120,
#else	/* _WIN32_WCE */
						 0, 0, 240, 120,
#endif	/* _WIN32_WCE */
						 NULL, NULL, hInst, NULL ) ;
	if ( !hMainWnd ) {
		wsprintf( Message, TEXT("CreateWindow:%d"), GetLastError() ) ;
		MessageBox( NULL, Message, szWinName, MB_OK ) ;
		return FALSE;
	}
	/* タスクバーに登録する */
	ShowIcon() ;

	return F_LoadNotify ;
}

static	BOOL
AddTaskBarIcon( HWND hWnd, HICON hIcon, UINT uID, LPCTSTR lpszTip )
{
	BOOL			res ;
	NOTIFYICONDATA	tnid ;

	memset( &tnid, 0, sizeof tnid ) ;
	tnid.cbSize = sizeof(NOTIFYICONDATA) ;
	tnid.hWnd = hWnd ;
	tnid.uID = IDM_NOTIFY ;
	tnid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP ;
	tnid.uCallbackMessage = WM_COMMAND ;
	tnid.hIcon = hIcon ;
	if ( lpszTip ) {
		_tcscpy( tnid.szTip, lpszTip ) ;
	} else {
		tnid.szTip[0] = '\0' ;
	}
	res = Shell_NotifyIcon( NIM_ADD, &tnid ) ;
	return res ;
}

static	BOOL
DeleteTaskBarIcon( HWND hWnd, UINT uID )
{
	NOTIFYICONDATA	tnid ;

	memset( &tnid, 0, sizeof tnid ) ;
	tnid.cbSize = sizeof (NOTIFYICONDATA) ;
	tnid.hWnd = hWnd ;
	tnid.uID = IDM_NOTIFY ;
	return Shell_NotifyIcon( NIM_DELETE, &tnid ) ;
}

LRESULT CALLBACK
MainWndFunc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch ( message ) {
	case WM_DESTROY:
		PostQuitMessage( 0 ) ;
		break ;
	case WM_TIMER:
		WMTimer( hWnd ) ;
		break ;
	case WM_COMMAND:
		switch ( GET_WM_COMMAND_ID( wParam, lParam ) ) {
		case IDM_EXIT:
			MessageBeep( MB_ICONASTERISK ) ;
			SendMessage( hWnd, WM_CLOSE, 0, 0 ) ;
			break;
		case IDM_NOTIFY:
			if ( lParam == WM_LBUTTONDOWN ) {
				StopTimer( hWnd ) ;
				if ( GetAsyncKeyState( VK_MENU ) & 0x8000 ) {
					SendMessage( hWnd, WM_COMMAND, (WPARAM) IDM_EXIT, (LPARAM) 0 ) ;
				} else {
					g_bKeyStatus = GetShiftCtrlStatus() ;
					if ( g_bKeyStatus ) {
						StartTimer( hWnd ) ;
					} else {
						PostMessage( hWnd, WM_COMMAND, (WPARAM) IDM_SAVE, (LPARAM) 0 ) ;
					}
				}
			}
			break ;
		case IDM_SAVE:
			if ( F_NoIcon ) {
				HideIcon() ;
				Sleep( 500 ) ;
			}
			DesktopToFile() ;
			if ( F_NoIcon ) {
				ShowIcon() ;
			}
			break ;
		}
		break ;
	case WM_CLOSE:
		StopTimer( hWnd ) ;
		DestroyWindow( hWnd ) ;
		break;
	default:
		return DefWindowProc( hWnd, message, wParam, lParam ) ;
	}
	return 0 ;
}

void
StartTimer( HWND hWnd )
{
	KillTimer( hWnd, IDC_TIMER_1 ) ;
	if ( F_NoIcon ) {
		HideIcon() ;
	}
	if ( !F_NoSound ) {
		sndPlaySound( TEXT("\\Windows\\Default.WAV"), SND_FILENAME ) ;
	}
	g_dwTimerCount = g_dwTick ;
	g_bStartCapture = FALSE ;
	g_bTimer = TRUE ;
	SetTimer( hWnd, IDC_TIMER_1, g_dwInterval, NULL ) ;
}

void
StopTimer( HWND hWnd )
{
	if ( g_bTimer ) {
		g_bTimer = FALSE ;
		KillTimer( hWnd, IDC_TIMER_1 ) ;
	}
	ShowIcon() ;
}

void
WMTimer( HWND hWnd )
{
	BOOL	bKeyStatus = GetShiftCtrlStatus() ;

	if ( !g_bTimer ) {
		KillTimer( hWnd, IDC_TIMER_1 ) ;
		return ;
	}
	if ( !g_bStartCapture ) {
		if ( !bKeyStatus ) {
			g_bStartCapture = TRUE ;
		}
	} else if ( bKeyStatus == g_bKeyStatus ) {
		g_bTimer = FALSE ;
		KillTimer( hWnd, IDC_TIMER_1 ) ;
		DesktopToFile() ;
		ShowIcon() ;
	}
	if ( g_dwTimerCount ) {
		if ( g_dwTimerCount * g_dwInterval == 10 * 1000 ) {
			sndPlaySound( TEXT("\\Windows\\Alarm2.WAV"), SND_FILENAME ) ;
		}
		g_dwTimerCount -- ;
	} else {
		StopTimer( hWnd ) ;
	}
}

void
ShowIcon( void )
{
	if ( !F_LoadNotify ) {
		hIcon = LoadImage( hInst, MAKEINTRESOURCE(IDI_SAVE_ICON),
						   IMAGE_ICON, 16, 16, 0 ) ;
		F_LoadNotify = AddTaskBarIcon( hMainWnd, hIcon, IDI_SAVE_ICON, TEXT("Capture Screen") ) ;
		if ( !F_LoadNotify ) {
			MessageBox( hMainWnd, TEXT(""), TEXT(""), MB_OK ) ;
		}
	}
}

void
HideIcon( void )
{
	if ( F_LoadNotify ) {
		DeleteTaskBarIcon( hMainWnd, IDI_SAVE_ICON ) ;
		DestroyIcon( hIcon ) ;
		F_LoadNotify = FALSE ;
	}
}

BYTE
GetShiftCtrlStatus( void )
{
	BOOL	fControl = GetAsyncKeyState( VK_CONTROL ) & 0x8000 ;
	BOOL	fShift   = GetAsyncKeyState( VK_SHIFT ) & 0x8000 ;

	return (fControl ? 0x01 : 0x00) + (fShift ? 0x02 : 0x00) ;
}

TCHAR				BmpFn[ MAX_PATH ] ;
BITMAPFILEHEADER	BmpFH ;
struct {
	BITMAPINFOHEADER	Bmih ;
	RGBQUAD				Rgq[ 256 ] ;
} Bmi ;
BYTE				BmpLine[ 640 / 2 ] ;
RGBQUAD				DefaultRgq[ 256 ] = {
#include	"palette.h"
} ;

typedef struct tagPALETTEENTRY	{ // pe
	BYTE peRed ;
	BYTE peGreen ;
	BYTE peBlue ;
	BYTE peFlags ;
} PALETTEENTRY, *LPPALETTEENTRY ;

UINT	(*GetSystemPaletteEntries)( HDC hdc, UINT iStartIndex, UINT nEntries, LPPALETTEENTRY lppe ) = NULL ;

PALETTEENTRY	g_pe[ 256 ] ;

static	BOOL
DesktopToFile( void )
{
	HANDLE	hFile ;
	DWORD	writesize ;
	PBYTE	frameBuffer, src, dst ;
	HDC		hDC, hComDC ;
	HBITMAP	hDIB, hBmpOld ;
	DWORD	rectH, rectW, filesize, i, j, cw, szPalette ;
	TCHAR	buf[ 128 ] ;

	rectW = GetSystemMetrics( SM_CXSCREEN ) ;
	rectH = GetSystemMetrics( SM_CYSCREEN ) ;
	/* BITMAPINFOHEADERを作成する */
	Bmi.Bmih.biWidth = rectW ;
	Bmi.Bmih.biHeight = rectH ;
	Bmi.Bmih.biSize = sizeof (Bmi.Bmih) ;
	Bmi.Bmih.biPlanes = 1 ;

	Bmi.Bmih.biCompression = BI_RGB ;
	Bmi.Bmih.biSizeImage = 0 ;
	Bmi.Bmih.biXPelsPerMeter = 0 ;
	Bmi.Bmih.biYPelsPerMeter = 0 ;
	Bmi.Bmih.biClrUsed = 0 ;
	Bmi.Bmih.biClrImportant = 0 ;
	Bmi.Bmih.biBitCount = (g_dwBpp == 8 ? 8 : 2) ;

	/* 画面の内容を取得する */
	hDC = GetDC( NULL ) ;

	if ( g_dwBpp != 8 ) {
		memset( Bmi.Rgq, 0, sizeof Bmi.Rgq ) ;
		Bmi.Rgq[0].rgbBlue = Bmi.Rgq[0].rgbGreen = Bmi.Rgq[0].rgbRed = 0 ;
		Bmi.Rgq[1].rgbBlue = Bmi.Rgq[1].rgbGreen = Bmi.Rgq[1].rgbRed = 132 ;
		Bmi.Rgq[2].rgbBlue = Bmi.Rgq[2].rgbGreen = Bmi.Rgq[2].rgbRed = 198 ;
		Bmi.Rgq[3].rgbBlue = Bmi.Rgq[3].rgbGreen = Bmi.Rgq[3].rgbRed = 255 ;
	} else {
		HINSTANCE	hLib ;

		/* とりあえずDefaultのパレットをロード */
		memcpy( Bmi.Rgq, DefaultRgq, sizeof Bmi.Rgq ) ;
		hLib = LoadLibrary( TEXT("COREDLL.DLL") ) ;
		if ( hLib ) {
			GetSystemPaletteEntries = (BOOL(*)(HDC,UINT,UINT,LPPALETTEENTRY)) GetProcAddress( hLib, TEXT("GetSystemPaletteEntries") ) ;
			if ( GetSystemPaletteEntries ) {
				memset( g_pe, 0, sizeof g_pe ) ;
				if ( (*GetSystemPaletteEntries)( hDC, 0, 256, g_pe ) ) {
					for ( i = 0 ; i < 256 ; i ++ ) {
						Bmi.Rgq[i].rgbBlue  = g_pe[i].peBlue ;
						Bmi.Rgq[i].rgbGreen = g_pe[i].peGreen ;
						Bmi.Rgq[i].rgbRed   = g_pe[i].peRed ;
					}
				}
			}
			FreeLibrary( hLib ) ;
		}
	}
	szPalette = (1 << g_dwBpp) * sizeof (RGBQUAD) ;

	hDIB = CreateDIBSection( hDC, (PBITMAPINFO)&Bmi.Bmih, DIB_RGB_COLORS,
							 &frameBuffer, NULL, 0 ) ;
	if ( !hDIB ) {
		ReleaseDC( NULL, hDC ) ;
		MessageBeep( MB_ICONASTERISK ) ;
		return FALSE ;
	}
	hComDC = CreateCompatibleDC( hDC ) ;
	if ( !hComDC ) {
		DeleteObject( hDIB ) ;
		ReleaseDC( NULL, hDC ) ;
		MessageBeep( MB_ICONASTERISK ) ;
		return FALSE ;
	}

	if ( !F_NoSound ) {
		sndPlaySound( TEXT("\\Windows\\OpenProg.WAV"), SND_FILENAME ) ;
	}

	hBmpOld = SelectBitmap( hComDC, hDIB ) ;
	if ( !BitBlt( hComDC, 0, 0, rectW, rectH, hDC, 0, 0, SRCCOPY ) ) {
		wsprintf( buf, TEXT("%d"), GetLastError() ) ;
		MessageBox( NULL, buf, TEXT("BitBlt"), MB_OK ) ;
	}

	/* 格納するファイル名を取得する */
	if ( F_Dialog ) {
		SetForegroundWindow( hMainWnd ) ;
		if ( !FileSaveDlg( hMainWnd, BmpFn, sizeof BmpFn ) ) {
			MessageBeep( MB_ICONASTERISK ) ;
			goto exit ;
		}
	} else {
		if ( !find_empty_file() ) {
			MessageBeep( MB_ICONASTERISK ) ;
			goto exit ;
		}
	}

	Bmi.Bmih.biBitCount = (WORD) g_dwBpp ;
	cw = rectW / ( 8 / g_dwBpp ) ;
	filesize = cw * rectH ;
	if ( g_dwBpp == 4 ) {
		Bmi.Bmih.biClrUsed = 4 ;
		cw = rectW / 4 ;
	}

	/* BITMAPFILEHEADERを作成する */
    BmpFH.bfType = 0x4D42 ;
	BmpFH.bfSize = sizeof BmpFH + sizeof Bmi.Bmih + szPalette + filesize ;
    BmpFH.bfReserved1 = BmpFH.bfReserved2 = 0 ;
    BmpFH.bfOffBits = sizeof BmpFH + sizeof Bmi.Bmih + szPalette ;

	/* ビットマップファイルに保存する */
	hFile = CreateFile( BmpFn, GENERIC_WRITE, 0, 0,
						CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0 ) ;
	if ( hFile == INVALID_HANDLE_VALUE ) {
		MessageBeep( MB_ICONASTERISK ) ;
		goto exit ;
	}
	WriteFile( hFile, &BmpFH, sizeof BmpFH, &writesize, NULL ) ;
	WriteFile( hFile, &Bmi.Bmih, sizeof Bmi.Bmih, &writesize, NULL ) ;
	WriteFile( hFile, &Bmi.Rgq, szPalette, &writesize, NULL ) ;
	if ( g_dwBpp == 4 ) {
		src = frameBuffer ;
		for ( j = 0 ; j < rectH ; j ++ ) {
			dst = BmpLine ;
			for ( i = 0 ; i < cw ; i ++ ) {
				BYTE	c = *src ++ ;
				*dst++ = ((c & 0x30) >> 4) | ((c & 0xC0) >> 6) << 4 ;
				*dst++ = (c & 0x03) | ((c & 0x0C) >> 2) << 4 ;
			}
			WriteFile( hFile, BmpLine, rectW / 2, &writesize, NULL ) ;
		}
	} else {
		WriteFile( hFile, frameBuffer, filesize, &writesize, NULL ) ;
	}
	CloseHandle( hFile ) ;

	if ( !F_NoSound ) {
		sndPlaySound( TEXT("\\Windows\\Close.WAV"), SND_FILENAME ) ;
	}

exit:
	SelectBitmap( hComDC, hBmpOld ) ;
	DeleteObject( hDIB ) ;
	DeleteDC( hComDC ) ;
	ReleaseDC( NULL, hDC ) ;

	return TRUE ;
}

/*
 * データファイル読み込みダイアログ
 */
static	OPENFILENAME	ofn ;
static	TCHAR			buf1[ MAX_PATH ] ;

static	BOOL
FileSaveDlg( HWND hWnd, LPTSTR buf, DWORD size )
{
	memset( &ofn, 0, sizeof ofn ) ;
	*buf1 = 0 ;
	ofn.lStructSize = sizeof ofn ;
    ofn.hwndOwner = hWnd ;
	ofn.hInstance = hInst ;
	if ( g_dwBpp == 2 ) {
		ofn.lpstrFilter = TEXT("2BP Files (*.2bp)\0*.2bp\0\0") ;
		ofn.lpstrDefExt = TEXT("2BP") ;
	} else {
		ofn.lpstrFilter = TEXT("Bitmap Files (*.bmp)\0*.bmp\0\0") ;
		ofn.lpstrDefExt = TEXT("BMP") ;
	}
	ofn.lpstrCustomFilter = NULL ;
	ofn.nMaxCustFilter = 0 ;
	ofn.nFilterIndex = 1 ;
	ofn.lpstrFile = buf1 ;
	ofn.nMaxFile = MAX_PATH ;
	ofn.lpstrFileTitle = NULL ;
	ofn.nMaxFileTitle = 0 ;
	ofn.lpstrInitialDir = NULL ;
	ofn.lpstrTitle = NULL ;
	ofn.Flags = 0 ;
	ofn.nFileOffset = 0 ;
	ofn.nFileExtension = 0 ;
	ofn.lCustData = 0 ;
	ofn.lpfnHook = NULL ;
	ofn.lpTemplateName = NULL ;
	if ( !GetSaveFileName( &ofn ) ) {
		return FALSE ;
	}
	_tcscpy( buf, buf1 ) ;
	return TRUE ;
}

/*
 * 　名称：ファイル存在チェック
 * 関数名：Fexist
 * 引き数：src(ファイル名)
 * 戻り値：BOOL(TRUE=ある, FALSE=ない場合)
 *   概要：ファイルが存在するかどうか調べる
 */
BOOL
Fexist( LPCTSTR src )
{
	HANDLE	hSrc ;

	hSrc = CreateFile( src,
					   GENERIC_READ,
					   FILE_SHARE_READ,
					   NULL,
					   OPEN_EXISTING,
					   FILE_ATTRIBUTE_NORMAL,
					   NULL ) ;
	if ( hSrc == INVALID_HANDLE_VALUE ) {
		return FALSE ;
	}
	CloseHandle( hSrc ) ;
	return TRUE ;
}

static	BOOL
find_empty_file( void )
{
	DWORD	org = SaveCount ;

	while ( 1 ) {
		wsprintf( BmpFn, TEXT("%s%04d.%s"),
				  SaveBase, SaveCount ++,
				  g_dwBpp == 2 ? TEXT("2BP") : TEXT("BMP") ) ;
		if ( !Fexist( BmpFn ) ) {
			break ;
		}
		if ( SaveCount == org ) {
			return FALSE ;
		}
	}
	return TRUE ;
}

/*
 * コマンドライン関係
 */
void
cmdline2args( LPTSTR cmdline, LPDWORD argc, LPTSTR *argv, LPTSTR args )
{
	TCHAR	c ;
	BOOL	f_skip = TRUE, f_quote = FALSE ;

	if ( args ) {
		*args = 0 ;
	}
	argv[ 0 ] = TEXT("") ;
	*argc = 1 ;
	while ( c = *cmdline ) {
		if ( f_skip ) {
			if ( !(c == TEXT(' ') || c == TEXT('\t')) ) {
				if ( c == TEXT('"') ) {
					f_quote = TRUE ;
					cmdline ++ ;
				}
				if ( args && *argc == 2 ) {
					_tcscpy( args, cmdline ) ;
				}
				argv[ (*argc) ++ ] = cmdline ;
				f_skip = FALSE ;
			}
		} else {
			if ( f_quote ) {
				if ( c == TEXT('"') ) {
					f_quote = FALSE ;
					*cmdline = 0 ;
					f_skip = TRUE ;
				}
			} else if ( (c == TEXT(' ') || c == TEXT('\t')) ) {
				*cmdline = 0 ;
				f_skip = TRUE ;
			}
		}
		cmdline ++ ;
	}
}
