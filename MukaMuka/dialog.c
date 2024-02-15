#ifndef	UNICODE
#define	UNICODE
#endif	/* UNICODE */
#ifndef	_UNICODE
#define	_UNICODE
#endif	/* _UNICODE */
#define	STRICT
#include	<windows.h>
#include	<windowsx.h>
#include	<commdlg.h>
#include	<string.h>
#include	<tchar.h>
#include	"dialog.h"
#include	"resource.h"

#define		subkey	TEXT("Software\\Gawaro\\MukaMuka")

#define		VAR_PATHNAME	TEXT("Pathname")
#define		VAR_OPENING		TEXT("Opening")
#define		VAR_WALL		TEXT("Wall")
#define		VAR_STARTPOS	TEXT("StartPos")

/* レジストリ関係 */
BOOL
get_reg( void )
{
	HKEY	hk ;
	LONG	lret ;
	LPTSTR	name ;
	LPBYTE	lpData ;
	DWORD	dwType, cbData, data ;

	/* レジストリキーをオープンする */
	lret = RegOpenKeyEx( HKEY_CURRENT_USER,
						 subkey,
						 0,
						 KEY_QUERY_VALUE,
						 &hk ) ;
	if ( lret != ERROR_SUCCESS ) {
		return FALSE ;
	}
	/* データファイル名 */
	name = VAR_PATHNAME ;
	dwType = REG_SZ ;
	lpData = (LPBYTE) MukaPath ;
	cbData = sizeof MukaPath ;
	lret = RegQueryValueEx( hk, name, NULL, &dwType, lpData, &cbData ) ;
	/* デバッグ:オープニングフラグ */
	name = VAR_OPENING ;
	dwType = REG_DWORD ;
	lpData = (LPBYTE) &data ;
	cbData = sizeof (DWORD) ;
	lret = RegQueryValueEx( hk, name, NULL, &dwType, lpData, &cbData ) ;
	if ( lret == ERROR_SUCCESS && cbData != ERROR_MORE_DATA ) {
		F_skipstart = data ;
	}
	/* デバッグ:壁なしフラグ */
	name = VAR_WALL ;
	dwType = REG_DWORD ;
	lpData = (LPBYTE) &data ;
	cbData = sizeof (DWORD) ;
	lret = RegQueryValueEx( hk, name, NULL, &dwType, lpData, &cbData ) ;
	if ( lret == ERROR_SUCCESS && cbData != ERROR_MORE_DATA ) {
		F_nowall = data ;
	}
	/* デバッグ:スタート位置ノーチェックフラグ */
	name = VAR_STARTPOS ;
	dwType = REG_DWORD ;
	lpData = (LPBYTE) &data ;
	cbData = sizeof (DWORD) ;
	lret = RegQueryValueEx( hk, name, NULL, &dwType, lpData, &cbData ) ;
	if ( lret == ERROR_SUCCESS && cbData != ERROR_MORE_DATA ) {
		F_nopos = data ;
	}
	RegCloseKey( hk ) ;
	return TRUE ;
}

BOOL
set_reg( void )
{
	HKEY	hk ;
	LONG	lret ;
	LPTSTR	name ;
	LPBYTE	lpData ;
	DWORD	ret, dwType, cbData ;

	/* レジストリキーを作成する */
	lret = RegCreateKeyEx( HKEY_CURRENT_USER,
						   subkey,
						   0,
						   TEXT(""),
						   0,
#ifdef	_WIN32_WCE
						   0,
#else	/* _WIN32_WCE */
						   KEY_ALL_ACCESS,
#endif	/* _WIN32_WCE */
						   NULL,
						   &hk,
						   &ret ) ;
	if ( lret != ERROR_SUCCESS ) {
		return FALSE ;
	}
	/* データファイル名 */
	name = VAR_PATHNAME ;
	dwType = REG_SZ ;
	lpData = (LPBYTE) MukaPath ;
	cbData = (_tcslen(MukaPath) + 1) * sizeof (*MukaPath) ;
	lret = RegSetValueEx( hk, name, 0, dwType, lpData, cbData ) ;
	RegCloseKey( hk ) ;
	return TRUE ;
}

void
delete_reg( void )
{
	RegDeleteKey( HKEY_CURRENT_USER, subkey ) ;
}

/* データファイル読み込みダイアログ */
static	OPENFILENAME	ofn ;
static	TCHAR			buf1[ MAX_PATH + 1 ] ;
static	TCHAR			InitDir[ MAX_PATH + 1 ] = TEXT("\\") ;

BOOL
FileOpenDlg( HWND hWnd, LPTSTR buf, DWORD size )
{
	memset( &ofn, 0, sizeof ofn ) ;
	memcpy( buf1, TEXT("*.muk\0"), sizeof TEXT("*.muk\0") ) ;
	ofn.lStructSize = sizeof ofn ;
    ofn.hwndOwner = hWnd ;
    ofn.hInstance = hInst ;
    ofn.lpstrFilter = TEXT("MukaMuka Files (*.muk)\0*.muk\0\0") ;
    ofn.lpstrCustomFilter = NULL ;
    ofn.nMaxCustFilter = 0 ;
    ofn.nFilterIndex = 1 ;
    ofn.lpstrFile = buf1 ;
    ofn.nMaxFile = MAX_PATH + 1 ;
    ofn.lpstrFileTitle = NULL ;
    ofn.nMaxFileTitle = 0 ;
    ofn.lpstrInitialDir = InitDir ;
    ofn.lpstrTitle = NULL ;
    ofn.Flags = 0 ;
    ofn.nFileOffset = 0 ;
    ofn.nFileExtension = 0 ;
    ofn.lpstrDefExt = TEXT("MUK") ;
    ofn.lCustData = 0 ;
    ofn.lpfnHook = NULL ;
    ofn.lpTemplateName = NULL ;
	if ( !GetOpenFileName( &ofn ) ) {
		return FALSE ;
	}
	_tcscpy( buf, buf1 ) ;
	return TRUE ;
}

void
setup_init_dir( LPCTSTR buf )
{
	LPTSTR		p ;

	_tcscpy( InitDir, buf ) ;
	p = tcsrchr( InitDir, TEXT('\\') ) ;
	if ( p && p != InitDir ) {
		*p = 0 ;
	}
}

/* ABOUTダイアログ関係 */
BOOL CALLBACK
AboutDlgFunc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	TCHAR	buf[ MAX_PATH + 1 ] ;

	switch ( message ) {
	case WM_INITDIALOG:
		/* バージョン情報を初期化する */
		wsprintf( buf, TEXT("%s Ver %s  for WindowsCE"),
				  szWinName, Version ) ;
		SendDlgItemMessage( hDlg, IDC_VERSION,
							WM_SETTEXT, 0, (LPARAM)buf ) ;
		return TRUE ;
	case WM_COMMAND:
		switch ( GET_WM_COMMAND_ID( wParam, lParam ) ) {
		case IDOK:
		case IDCANCEL:
			EndDialog( hDlg, TRUE ) ;
			return TRUE ;
		}
		break ;
	}
	return FALSE ;
}

int
tcsicmp( LPCTSTR str1, LPCTSTR str2 )
{
	while ( *str1 && (*str1 == *str2) ) {
		str1 ++, str2 ++ ;
	}
	return (int) *str1 - (int) *str2 ;
}

LPTSTR
tcsrchr( LPCTSTR str, TCHAR c )
{
	DWORD	i ;

	for ( i = _tcslen( str ) ; i ; i -- ) {
		if ( str[ i - 1 ] == c ) {
			return (LPTSTR) &str[ i - 1 ] ;
		}
	}
	return NULL ;
}
