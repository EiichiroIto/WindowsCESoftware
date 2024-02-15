/*	
 * KANAIN : かな入力プログラム：メインルーチン
 *
 * 1996/12/21:E.Itoh
 *  (1) 作成開始
 * 修正履歴
 * 1996/12/24:E.Itoh
 *  (1) ダイアログ形状変更
 *  (2) 他画面へペーストできるようにした
 *  (3) 大文字入力できないバグを修正
 *  (4) 特殊記号の入力
 *
 */
#ifndef	STRICT
#define	STRICT
#endif	/* STRICT */

#include	<windows.h>
#include	<windowsx.h>
#include	<winuser.h>
#include	<stdio.h>
#include	<string.h>
#include	<commctrl.h>
#include	<tchar.h>
//#include	"kanain.h"
#include	"resource.h"

static	BOOL	init_application( void ) ;
static	BOOL	init_instance( int nCmdShow ) ;
LRESULT	CALLBACK	MainWndFunc( HWND, UINT, WPARAM, LPARAM ) ;
BOOL	CALLBACK	DlgFunc( HWND, UINT, WPARAM, LPARAM ) ;

void	debug_dialog( LPCTSTR message, LPCTSTR opt ) ;
void	roma2kana_unicode( LPCTSTR src, LPTSTR dst ) ;

#define		WIN_LEFT	245
#define		WIN_TOP		189

TCHAR		szWinName[] = TEXT("KanaIn") ;
TCHAR		Str[ 81 ] = TEXT("") ;
BOOL		F_draw = FALSE ;
BOOL		F_dialog = FALSE ;
HINSTANCE	hInst ;
HWND		hMainWnd, hEditWnd, hDlgWnd = NULL ;
HFONT		hFont ;

#define		MAX_TEXT		(256)
TCHAR		RomaText[ MAX_TEXT ] ;
TCHAR		KanaText[ MAX_TEXT ] ;

int WINAPI
WinMain( HINSTANCE hThisInst, HINSTANCE hPrevInst,
		 LPTSTR lpszArgs, int nWinMode )
{
	MSG			msg ;
	HACCEL		hAccl ;

	hInst = hThisInst ;
	if ( !init_application() ) {
		goto ExitMain ;
	}

	if ( !init_instance( nWinMode ) ) {
		goto ExitMain ;
	}

	PostMessage( hMainWnd, WM_COMMAND, IDM_DIALOG, 0 ) ;

#ifdef	PEGASUS
	hAccl = LoadAccelerators( hInst,
							  MAKEINTRESOURCE(IDR_ACCELERATOR1) ) ;
#else	/* PEGASUS */
	hAccl = LoadAccelerators( GetModuleHandle(NULL),
							  MAKEINTRESOURCE(IDR_ACCELERATOR1) ) ;
#endif	/* PEGASUS */

	while ( GetMessage( &msg, NULL, 0, 0 ) ) {
		if ( !TranslateAccelerator( hMainWnd, hAccl, &msg ) ) {
			TranslateMessage( &msg ) ;
		}
		DispatchMessage( &msg ) ;
	}

ExitMain:
	DeleteObject( hFont ) ;
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
	wcl.lpszMenuName = 0 ;
	wcl.cbClsExtra = 0 ;
	wcl.cbWndExtra = 0 ;
	wcl.hbrBackground = GetStockObject( WHITE_BRUSH ) ;
	if ( !RegisterClass( &wcl ) ) {
//		debug_dialog( TEXT("init_application"), TEXT("RegisterClass(1)") ) ;
		return FALSE ;
	}
	return TRUE ;
}

static	BOOL
init_instance( int nCmdShow )
{
	/* メインウィンドウを作成する */
#ifdef	PEGASUS
#ifdef	TARGET_NT
	hMainWnd = CreateWindow( szWinName,
						 TEXT("KanaIn"),
						 WS_POPUP,//|WS_SYSMENU|WS_VISIBLE|WS_DLGFRAME,
						 0,
						 0,
						 CW_USEDEFAULT,
						 CW_USEDEFAULT,
						 NULL,
						 NULL,
						 hInst,
						 NULL ) ;
#else
	hMainWnd = CreateWindowEx( 0, szWinName,
						 TEXT("KanaIn"),
						 WS_VISIBLE,
						 WIN_LEFT,
						 WIN_TOP,
						 1,
						 1,
						 NULL,
						 NULL,
						 hInst,
						 NULL ) ;
#endif
#else
//	hMainWnd = CreateWindow( szWinName,
						 TEXT("KanaIn"),
						 WS_OVERLAPPEDWINDOW,
						 0,
						 0,
						 480,
						 240,
						 NULL,
						 NULL,
						 hInst,
						 NULL ) ;
#endif
	if ( !hMainWnd ) {
//		debug_dialog( TEXT("init_instance"), TEXT("CreateWindow(1)") ) ;
		return FALSE;
	}

	/* メインウィンドウを表示する */
	ShowWindow( hMainWnd, nCmdShow ) ;
	UpdateWindow( hMainWnd ) ;

	return TRUE ;
}

LRESULT CALLBACK
MainWndFunc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	HLOCAL			hMem ;
	DWORD			size ;
	BOOL			f_enter = FALSE ;

	switch ( message ) {
	case WM_DESTROYCLIPBOARD:
		return 0 ;
	case WM_SETFOCUS:
		PostMessage( hMainWnd, WM_COMMAND, IDM_DIALOG, 0 ) ;
		break ;
	case WM_CREATE:
		break ;
	case WM_DESTROY:
		PostQuitMessage( 0 ) ;
		break ;
	case WM_COMMAND:
		switch ( GET_WM_COMMAND_ID( wParam, lParam ) ) {
		case IDM_EXIT:
			SendMessage( hWnd, WM_CLOSE, 0, 0 ) ;
			return TRUE;
		case IDM_DIALOG:
			if ( F_dialog ) {
				return TRUE ;
			}
			F_dialog = TRUE ;
			if ( !DialogBox( hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, DlgFunc ) ) {
				SendMessage( hWnd, WM_CLOSE, 0, 0 ) ;
				return TRUE ;
			}
			if ( *RomaText ) {
				if ( !OpenClipboard( hWnd ) ) {
					return FALSE ;
				}
				EmptyClipboard() ;
				roma2kana_unicode( RomaText, KanaText ) ;
				size = (_tcslen( KanaText ) + 1) * sizeof (TCHAR) ;
				hMem = LocalAlloc( LMEM_FIXED, size ) ;
				if ( hMem ) {
					memset( (LPBYTE)hMem, 0, size ) ;
					_tcscpy( (LPTSTR)hMem, KanaText ) ;
					SetClipboardData( CF_UNICODETEXT, hMem ) ;
				}
				CloseClipboard() ;
			} else {
				f_enter = TRUE ;
			}
#if 0
			SetWindowPos( hWnd, HWND_BOTTOM, 0, 0, 0, 0,
						SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE ) ;
			Sleep( 500 ) ;
			if ( GetActiveWindow() == hWnd ) {
				MessageBox( hWnd, TEXT(""), TEXT(""), MB_OK ) ;
			}
#else
			keybd_event( VK_MENU, 0, 0, 0 ) ;
			keybd_event( VK_ESCAPE, 0, 0, 0 ) ;
			keybd_event( VK_ESCAPE, 0, KEYEVENTF_KEYUP, 0 ) ;
			keybd_event( VK_MENU, 0, KEYEVENTF_KEYUP, 0 ) ;
			Sleep( 500 ) ;
#endif
			PostMessage( hWnd, WM_COMMAND, IDM_SENDKEY, f_enter ) ;
			F_dialog = FALSE ;
			return TRUE ;
		case IDM_SENDKEY:
			if ( lParam ) {
				keybd_event( VK_RETURN, 0, 0, 0 ) ;
				keybd_event( VK_RETURN, 0, KEYEVENTF_KEYUP, 0 ) ;
			} else {
				keybd_event( VK_CONTROL, 0, 0, 0 ) ;
				keybd_event( 0x56, 0, 0, 0 ) ;
				keybd_event( 0x56, 0, KEYEVENTF_KEYUP, 0 ) ;
				keybd_event( VK_CONTROL, 0, KEYEVENTF_KEYUP, 0 ) ;
			}
			Sleep( 1000 ) ;
			PostMessage( hWnd, WM_COMMAND, IDM_DISPWIN, 0 ) ;
			return TRUE ;
		case IDM_DISPWIN:
			if ( hDlgWnd ) {
				SetForegroundWindow( hWnd ) ;
				SetActiveWindow( hDlgWnd ) ;
			}
			return TRUE ;
		}
		break ;
	case WM_CHAR:
		break ;
	case WM_CLOSE:
		DestroyWindow( hWnd ) ;
		break;
	default:
		return DefWindowProc( hWnd, message, wParam, lParam ) ;
	}
	return 0 ;
}

BOOL CALLBACK
DlgFunc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch ( message ) {
	case WM_INITDIALOG:
		hDlgWnd = hDlg ;
		SetWindowPos( hDlg, 0, WIN_LEFT, WIN_TOP, 0, 0,
					  SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOZORDER ) ;
		return TRUE ;
	case WM_COMMAND:
		switch ( GET_WM_COMMAND_ID( wParam, lParam ) ) {
		case IDOK:
			SendDlgItemMessage( hDlg, IDC_EDIT_ROMA,
								WM_GETTEXT, MAX_TEXT, (LPARAM)RomaText ) ;
			hDlgWnd = NULL ;
			EndDialog( hDlg, TRUE ) ;
			return TRUE ;
		case IDCANCEL:
			hDlgWnd = NULL ;
			EndDialog( hDlg, FALSE ) ;
			return TRUE ;
		}
		break ;
	}
	return FALSE ;
}

/*
 * デバッグダイアログ表示ルーチン
 */
void
debug_dialog( LPCTSTR message, LPCTSTR opt )
{
	TCHAR	buf[ 128 ] ;

	wsprintf( buf, TEXT("%s:%s\n(ErrorCode=%d)"), message, opt, GetLastError() ) ;
	MessageBox( NULL,
				buf,
				TEXT("Error"),
				MB_ICONSTOP|MB_OK ) ;
}

#define	isalpha(x)	(((x)>='A'&&(x)<='Z')||((x)>='a'&&(x)<='z'))
#define	isvowel(x)	((x)=='a'||(x)=='i'||(x)=='u'||(x)=='e'||(x)=='o')
#define	isvowel2(x)	((x)=='a'||(x)=='i'||(x)=='u'||(x)=='e'||(x)=='o'||(x)=='n'||(x)=='y')
#define	iskanji(c)	((((0xFF&(c))>=0x81)&&((0xFF&(c))<=0x9F))||(((0xFF&(c))>=0xE0)&&((0xFF&(c))<=0xFC)))
#define	tolower(c)	(((c)>='A'&&(c)<='Z')?(c)-'A'+'a':(c))

/*
 * ローマ字→かな変換
 */
typedef	enum roma_mode {
    roma_none = 0,
    roma_k, roma_s, roma_t, roma_n, roma_h, roma_f, roma_m,
    roma_y, roma_r, roma_w,
    roma_l, roma_c, roma_g, roma_z, roma_j, roma_d, roma_b, roma_p,
    roma_v, roma_ky, roma_sy, roma_sh, roma_ty, roma_ch, roma_ts, roma_ny,
    roma_hy, roma_fy, roma_my, roma_ry,
    roma_gy, roma_zy, roma_dy, roma_dh, roma_by, roma_py,
    roma_ly, roma_lt,
    roma_hw, roma_kw, roma_nw,
    roma_gw
} ROMA_MODE ;

typedef enum kana_code {
    kana_a = 0xB1, kana_k = 0xB6, kana_s = 0xBB, kana_t = 0xC0, kana_n = 0xC5,
    kana_h = 0xCA, kana_m = 0xCF, kana_y = 0xD4, kana_r = 0xD7, kana_w = 0xDC,
    kana_wo = 0xA6, kana_nn = 0xDD, kana_daku = 0xDE, kana_handaku = 0xDF,
    kana_l = 0xA7, kana_ly = 0xAC, kana_ltu = 0xAF,
	kana_touten = 0xA1, kana_left = 0xA2, kana_right = 0xA3,
	kana_kuten = 0xA4, kana_naka = 0xA5
} KANA_CODE ;

static	BYTE	alpha_table[] = {
    0xFF, 0x00, roma_b, roma_c, roma_d, 3, roma_f, roma_g, roma_h, 1, roma_j,
    roma_k, roma_l, roma_m, roma_n, 4,
    roma_p, roma_none, roma_r, roma_s, roma_t, 2, roma_v, roma_w, roma_l,
    roma_y, roma_z, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
} ;

static	BYTE	kana_table[] = {
    kana_a,
    kana_k, kana_s, kana_t, kana_n, kana_h, kana_h, kana_m,
    kana_y, kana_r, kana_w,
    kana_l, kana_s, kana_k, kana_s, kana_s, kana_t, kana_h, kana_h,
    kana_a, kana_k, kana_s, kana_s, kana_t, kana_t, kana_t, kana_n,
    kana_h, kana_h, kana_m, kana_r,
    kana_k, kana_s, kana_t, kana_t, kana_h, kana_h,
    kana_ly, kana_ltu,
    kana_h, kana_k, kana_n,
    kana_k
} ;

/* 拗音への対処(インデックスは rmode) */
static	BYTE	youon_table[] = {
    roma_none,
    roma_ky, roma_sy, roma_ty, roma_ny, roma_hy, roma_fy, roma_my,
    roma_none, roma_ry, roma_none,
    roma_ly, roma_ty, roma_gy, roma_zy, roma_zy, roma_dy, roma_by, roma_py,
} ;
int		num_youon = sizeof youon_table / sizeof youon_table[0] ;

/* ハングル文字への対処(インデックスは rmode) */
static	BYTE	hankul_table[] = {
    roma_none,
    roma_kw, roma_none, roma_none, roma_nw, roma_hw, roma_none, roma_none,
    roma_none, roma_none, roma_none,
    roma_none, roma_none, roma_gw
} ;
int		num_hankul = sizeof hankul_table / sizeof hankul_table[0] ;

BYTE
get_kana_ly( BYTE idx )
{
    switch ( idx ) {
    case 0:	return kana_ly + 0 ;
    case 1:	return kana_l + 1 ;
    case 2:	return kana_ly + 1 ;
    case 3:	return kana_l + 3 ;
    case 4:	return kana_ly + 2 ;
    }
    return 0 ;
}

DWORD
is_dakuon( DWORD mode )
{
    if ( ( mode >= roma_g && mode <= roma_b )
      || ( mode == roma_v )
      || ( mode >= roma_gy && mode <= roma_by )
      || ( mode == roma_gw ) ) {
		return 1 ;
    }
    return 0 ;
}

void
roma2kana( const BYTE *src, LPBYTE _dst )
{
    const BYTE*		p = src ;
    LPBYTE			dst = _dst ;
    ROMA_MODE		rmode = roma_none ;
    BYTE			c ;

    while ( (c = 0xFF & (*p++)) != 0 ) {
		BYTE		idx ;

//printf( "rmode=%d, c=%c\n", rmode, c ) ;
		if ( iskanji( c ) ) {
			*dst++ = c ;
			*dst++ = *p++ ;
			continue ;
		} else if ( !isalpha(c) ) {
			if ( c == '.' ) {
				c = kana_touten ;
			} else if ( c == '[' ) {
				c = kana_left ;
			} else if ( c == ']' ) {
				c = kana_right ;
			} else if ( c == ',' ) {
				c = kana_kuten ;
			} else if ( c == '/' ) {
				c = kana_naka ;
			}
			if ( rmode == roma_n ) {
				*dst++ = kana_nn ;
				if ( c != '\'' && c != ' ' ) {
					*dst++ = c ;
				}
			} else {
				*dst++ = c ;
			}
			rmode = roma_none ;
			continue ;
		}
		c = tolower(c) ;
		idx = alpha_table[c-'`'] ;
		if ( rmode != roma_none ) {
			if ( rmode < num_youon && c == 'y' ) {
				idx = youon_table[ rmode ] ;
				if ( idx != roma_none ) {
					rmode = idx ;
					continue ;
				}
			} else if ( rmode < num_hankul && c == 'w' ) {
				idx = hankul_table[ rmode ] ;
				if ( idx != roma_none ) {
					rmode = idx ;
					continue ;
				}
			}
		}
		if ( rmode == roma_t && c == 'c' ) {
			rmode = roma_c ;
		}
		if ( !isvowel( c ) && idx == rmode && rmode != roma_n && rmode != roma_m && rmode != roma_none ) {
			*dst++ = kana_ltu ;
			continue ;
		} else if ( rmode == roma_m && (idx == roma_m || idx == roma_b || idx == roma_p ) ) {
			*dst++ = kana_nn ;
			rmode = idx ;
			continue ;
#ifdef	DItoDHI
		} else if ( rmode == roma_d && c == 'i' ) {
			rmode = roma_dh ;
#endif	/* DItoDHI */
		}
		switch ( rmode ) {
		case roma_none:	case roma_l:	case roma_s:
		case roma_t:
			if ( isvowel( c ) ) {
				*dst++ = kana_table[ rmode ] + idx ;
			} else if ( rmode == roma_l && c == 't' ) {
				rmode = roma_lt ;
			} else if ( rmode == roma_s && c == 'h' ) {
				rmode = roma_sh ;
			} else if ( rmode == roma_t && c == 's' ) {
				rmode = roma_ts ;
			} else {
				rmode = idx ;
			}
			break ;
		case roma_n:
			if ( isvowel( c ) ) {
				*dst++ = kana_table[ rmode ] + idx ;
			} else if ( c == 'n' ) {
				*dst++ = kana_nn ;
				rmode = roma_none ;
			} else {
				*dst++ = kana_nn ;
				rmode = idx ;
			}
			break ;
		case roma_b:	case roma_g:	case roma_k:
		case roma_m:	case roma_p:	case roma_r:
		case roma_z:	case roma_d:
			if ( isvowel( c ) ) {
				*dst++ = kana_table[ rmode ] + idx ;
				if ( is_dakuon( rmode ) ) {
					*dst++ = kana_daku ;
				} else if ( rmode == roma_p ) {
					*dst++ = kana_handaku ;
				}
			} else if ( rmode == roma_d && c == 'h' ) {
				rmode = roma_dh ;
			} else {
				rmode = idx ;
			}
			break ;
		case roma_h:
			if ( isvowel( c ) ) {
				*dst++ = kana_table[ rmode ] + idx ;
			} else {
				*dst++ = kana_a + 2 ;
				rmode = idx ;
			}
			break ;
		case roma_c:	case roma_lt:
			if ( rmode == roma_c && c == 'h' ) {
				rmode = roma_ch ;
			} else if ( rmode == roma_lt && c == 'u' ) {
				*dst++ = kana_ltu ;
			} else {
				rmode = idx ;
			}
			break ;
		case roma_f:	case roma_v:	case roma_ts:
			if ( isvowel( c ) ) {
				*dst++ = kana_table[ rmode ] + 2 ;
				if ( is_dakuon( rmode ) ) {
					*dst++ = kana_daku ;
				}
				if ( c != 'u' ) {
					*dst++ = kana_l + idx ;
				}
			} else {
				rmode = idx ;
			}
			break ;
		case roma_w:
			if ( c == 'a' ) {
				*dst++ = kana_w ;
			} else if ( c == 'i' || c == 'e' ) {
				*dst++ = kana_a + 2 ;
				*dst++ = kana_l + idx ;
			} else if ( c == 'u' ) {
				*dst++ = kana_a + 2 ;
			} else if ( c == 'o' ) {
#ifdef	HANKUL
				*dst++ = kana_a + 2 ;
				*dst++ = kana_l + 4 ;
#else	/* HANKUL */
				*dst++ = kana_wo ;
#endif	/* HANKUL */
			} else {
				rmode = idx ;
			}
			break ;
		case roma_y:
			if ( c == 'a' ) {
				*dst++ = kana_y + 0 ;
			} else if ( c == 'i' ) {
				*dst++ = kana_a + 1 ;
			} else if ( c == 'u' ) {
				*dst++ = kana_y + 1 ;
			} else if ( c == 'e' ) {
				*dst++ = kana_a + 1 ;
				*dst++ = kana_l + 3 ;
			} else if ( c == 'o' ) {
				*dst++ = kana_y + 2 ;
			} else {
				rmode = idx ;
			}
			break ;
		case roma_ky:	case roma_sy:	case roma_ty:
		case roma_ny:	case roma_hy:	case roma_my:
		case roma_ry:
		case roma_gy:	case roma_zy:	case roma_dy:
		case roma_by:	case roma_py:
			if ( isvowel( c ) ) {
				*dst++ = kana_table[ rmode ] + 1 ;
				if ( rmode == roma_py ) {
					*dst++ = kana_handaku ;
				} else if ( is_dakuon( rmode ) ) {
					*dst++ = kana_daku ;
				}
				*dst++ = get_kana_ly( idx ) ;
			} else {
				rmode = idx ;
			}
			break ;
		case roma_j:	case roma_sh:	case roma_ch:
			if ( isvowel( c ) ) {
				*dst++ = kana_table[ rmode ] + 1 ;
				if ( is_dakuon( rmode ) ) {
					*dst++ = kana_daku ;
				}
				if ( c != 'i' ) {
					*dst++ = get_kana_ly( idx ) ;
				}
			} else {
				rmode = idx ;
			}
			break ;
		case roma_fy:
			if ( isvowel( c ) ) {
				*dst++ = kana_table[ rmode ] + 2 ;
				*dst++ = get_kana_ly( idx ) ;
			} else {
				rmode = idx ;
			}
			break ;
		case roma_ly:
			if ( isvowel( c ) ) {
				*dst++ = get_kana_ly( idx ) ;
			} else {
				rmode = idx ;
			}
			break ;
		case roma_dh:
			if ( isvowel( c ) ) {
				*dst++ = kana_table[ rmode ] + 3 ;
				*dst++ = kana_daku ;
				*dst++ = get_kana_ly( idx ) ;
			} else {
				rmode = idx ;
			}
			break ;
		case roma_gw:
		case roma_hw:
		case roma_kw:
		case roma_nw:
			if ( isvowel( c ) ) {
				*dst++ = kana_table[ rmode ] + 2 ;
				if ( is_dakuon( rmode ) ) {
					*dst++ = kana_daku ;
				}
				*dst++ = kana_l + idx ;
			} else {
				rmode = idx ;
			}
			break ;
		default:
			break ;
		}
		if ( isvowel( c ) ) {
			rmode = roma_none ;
		}
	}
	if ( rmode == roma_n ) {
		*dst++ = kana_nn ;
    }
    *dst = 0 ;
}

void
roma2kana_unicode( LPCTSTR src, LPTSTR dst )
{
	TCHAR	c, cb ;
	BYTE	src1[ 128 ], dst1[ 128 ] ;
	LPBYTE	p ;

	p = src1 ;
	while ( ( c = *src++ ) != 0 ) {
		*p++ = 0xFF & c ;
	}
	*p = 0 ;
	roma2kana( src1, dst1 ) ;
	p = dst1 ;
	while ( ( cb = *p++ ) != 0 ) {
		*dst++ = (TCHAR) cb ;
	}
	*dst = 0 ;
}
