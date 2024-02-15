#include	<windows.h>
#include	<tchar.h>
#include	"detect.h"

DWORD	DetectID = 5 ;
TCHAR	MessageBuf[ 256 ] ;
BYTE	TupleData[ 8192 ] ;

#define			DllName			TEXT("DETECT.DLL")
#define			EntryFunction	TEXT("DetectionFunction")
#define			Prefix			TEXT("DET")

extern	BOOL	FindRegSpace( LPDWORD pResult ) ;
extern	BOOL	SetupReg( DWORD index ) ;
extern	void	DeleteReg( DWORD index ) ;
extern	BOOL	GetTupleData( void ) ;

int WINAPI
WinMain( HINSTANCE hInstance, HINSTANCE hInstPrev, LPWSTR pszCmdLine, int nCmdShow )
{
	LPCTSTR	ptr ;
	DWORD	index, ret ;

	if ( *pszCmdLine ) {
		DetectID = _ttol( pszCmdLine ) ;
	}
	if ( !FindRegSpace( &index ) ) {
		ptr = TEXT("ERROR:\r\n")
			TEXT("  Already Detect port has owned by another driver.") ;
		MessageBox( NULL, ptr, TEXT("GetTuple"), MB_OK ) ;
		return 0 ;
	}
	if ( !SetupReg( index ) ) {
		ptr = TEXT("ERROR:\r\n")
			TEXT("  Could not write Registry.") ;
		MessageBox( NULL, ptr, TEXT("GetTuple"), MB_OK ) ;
		return 0 ;
	}
	ptr = TEXT("Please Insert PCMCIA Card.\r\n")
		TEXT("Then Press ENTER.") ;
	ret = MessageBox( NULL, ptr, TEXT("GetTuple"), MB_OKCANCEL ) ;
	if ( ret == IDOK ) {
		if ( GetTupleData() ) {
			ptr = TEXT("SUCCESS:\r\n")
				TEXT("  Please REMOVE PCMCIA Card.\r\n")
				TEXT("  You can get tuple data from the file:GETTUPLE.OUT") ;
			MessageBox( NULL, ptr, TEXT("GetTuple"), MB_OK ) ;
		}
	}
	DeleteReg( index ) ;
	return 0 ;
}

#define		DetectBase		TEXT("Drivers\\PCMCIA\\Detect\\")
#define		GetTupleKey		TEXT("Drivers\\PCMCIA\\GetTuple")

/*
 * レジストリ登録の場所が空いているかどうか調べる
 */
BOOL
FindRegSpace( LPDWORD pResult )
{
	HKEY	hk ;
	LONG	lret ;
	LPBYTE	lpData ;
	DWORD	index = DetectID, dwType, cbData ;
	TCHAR	subkey[ MAX_PATH ], buf[ MAX_PATH ] ;

	wsprintf( subkey, TEXT("%s%02d"), DetectBase, index ) ;
	lret = RegOpenKeyEx( HKEY_LOCAL_MACHINE, subkey, 0,
						 KEY_QUERY_VALUE, &hk ) ;
	if ( lret != ERROR_SUCCESS ) {
		*pResult = index ;
		return TRUE ;
	}
	dwType = REG_SZ ;
	lpData = (LPBYTE) buf ;
	cbData = sizeof buf ;
	lret = RegQueryValueEx( hk, TEXT("Dll"), NULL, &dwType, lpData, &cbData ) ;
	RegCloseKey( hk ) ;
	if ( lret == ERROR_SUCCESS && !_tcsicmp( buf, DllName ) ) {
		*pResult = index ;
		return TRUE ;
	}
	return FALSE ;
}

/*
 * レジストリを登録する
 */
BOOL
SetupReg( DWORD index )
{
	HKEY	hk ;
	LONG	lret ;
	BOOL	rc = TRUE ;
	LPTSTR	name ;
	LPBYTE	lpData ;
	DWORD	ret, dwType, cbData ;
	TCHAR	subkey[ MAX_PATH ] ;

	/* PCMCIA エントリ */
	/* レジストリキーを作成する */
	lret = RegCreateKeyEx( HKEY_LOCAL_MACHINE, GetTupleKey, 0, TEXT(""), 0,
						   0,
						   NULL, &hk, &ret ) ;
	if ( lret != ERROR_SUCCESS ) {
		return FALSE ;
	}
	/* DLL名 */
	name   = TEXT("Dll") ;
	dwType = REG_SZ ;
	lpData = (LPBYTE) DllName ;
	cbData = (_tcslen(DllName) + 1) * sizeof (*DllName) ;
	lret   = RegSetValueEx( hk, name, 0, dwType, lpData, cbData ) ;
	rc = rc && (lret == ERROR_SUCCESS) ;
	/* Prefix */
	name   = TEXT("Prefix") ;
	dwType = REG_SZ ;
	lpData = (LPBYTE) Prefix ;
	cbData = (_tcslen(Prefix) + 1) * sizeof (*Prefix) ;
	lret   = RegSetValueEx( hk, name, 0, dwType, lpData, cbData ) ;
	rc = rc && (lret == ERROR_SUCCESS) ;
	/* レジストリクローズ */
	RegCloseKey( hk ) ;
	if ( !rc ) {
		RegDeleteKey( HKEY_LOCAL_MACHINE, GetTupleKey ) ;
	}
	/* DETECT エントリ */
	wsprintf( subkey, TEXT("%s%02d"), DetectBase, index ) ;
	/* レジストリキーを作成する */
	lret = RegCreateKeyEx( HKEY_LOCAL_MACHINE, subkey, 0, TEXT(""), 0,
						   0,
						   NULL, &hk, &ret ) ;
	if ( lret != ERROR_SUCCESS ) {
		return FALSE ;
	}
	/* DLL名 */
	name   = TEXT("Dll") ;
	dwType = REG_SZ ;
	lpData = (LPBYTE) DllName ;
	cbData = (_tcslen(DllName) + 1) * sizeof (*DllName) ;
	lret   = RegSetValueEx( hk, name, 0, dwType, lpData, cbData ) ;
	rc = rc && (lret == ERROR_SUCCESS) ;
	/* エントリ名 */
	name   = TEXT("Entry") ;
	dwType = REG_SZ ;
	lpData = (LPBYTE) EntryFunction ;
	cbData = (_tcslen(EntryFunction) + 1) * sizeof (*EntryFunction) ;
	lret   = RegSetValueEx( hk, name, 0, dwType, lpData, cbData ) ;
	rc = rc && (lret == ERROR_SUCCESS) ;
	/* レジストリクローズ */
	RegCloseKey( hk ) ;
	/* レジストリ書き込み失敗 */
	if ( !rc ) {
		RegDeleteKey( HKEY_LOCAL_MACHINE, subkey ) ;
		RegDeleteKey( HKEY_LOCAL_MACHINE, GetTupleKey ) ;
		return FALSE ;
	}
	return TRUE ;
}

/*
 * 登録したレジストリを削除する
 */
void
DeleteReg( DWORD index )
{
	TCHAR	subkey[ MAX_PATH ] ;

	wsprintf( subkey, TEXT("%s%02d"), DetectBase, index ) ;
	RegDeleteKey( HKEY_LOCAL_MACHINE, subkey ) ;
	RegDeleteKey( HKEY_LOCAL_MACHINE, GetTupleKey ) ;
}

BOOL
GetTupleData( void )
{
	BOOL	ret ;
	TCHAR	buf[ MAX_PATH ], *ptr ;
	DWORD	i, numTupleData = 0, nWriteSize ;
	HANDLE	hDevice = INVALID_HANDLE_VALUE, hFile ;

	/* 認識したデバイスを探す */
	wsprintf( buf, TEXT("%s1:"), Prefix ) ;
	for ( i = 0 ; i < 10 ; i ++ ) {
		Sleep( 1000 ) ;
		hDevice = CreateFile( buf, GENERIC_READ|GENERIC_WRITE, 0, NULL,
							  OPEN_EXISTING, 0, NULL ) ;
		if ( hDevice != INVALID_HANDLE_VALUE ) {
			/* 見つかった場合 */
			break ;
		}
		Sleep( 2000 ) ;
	}
	if ( hDevice == INVALID_HANDLE_VALUE ) {
		/* 見つからなかった場合 */
		ptr = TEXT("ERROR:\r\n")
			TEXT("  Could not find any PCMCIA card.") ;
		MessageBox( NULL, ptr, TEXT("GetTuple"), MB_OK ) ;
		return FALSE ;
	}
	/* タプルデータを読み込む */
	memset( TupleData, 0, sizeof TupleData ) ;
	ret = DeviceIoControl( hDevice, IOCTL_PCMCIA_TEST_TUPLES, NULL, 0,
						   TupleData, sizeof TupleData,
						   &numTupleData, NULL ) ;
	CloseHandle( hDevice ) ;
	if ( !ret || !numTupleData ) {
		/* タプルデータを読み込めなかった場合 */
		ptr = TEXT("ERROR:\r\n")
			TEXT("  Could not read tuple data.") ;
		MessageBox( NULL, ptr, TEXT("GetTuple"), MB_OK ) ;
		return FALSE ;
	}
	/* タプルデータを書き込む */
	hFile = CreateFile( TEXT("GETTUPLE.OUT"), GENERIC_WRITE, 0, NULL,
						CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL ) ;
	if ( hFile == INVALID_HANDLE_VALUE ) {
		/* タプルデータを書き込めなかった場合 */
		ptr = TEXT("ERROR:\r\n")
			TEXT("  Could not write tuple data.") ;
		MessageBox( NULL, ptr, TEXT("GetTuple"), MB_OK ) ;
		return FALSE ;
	}
	WriteFile( hFile, TupleData, numTupleData, &nWriteSize, NULL ) ;
	CloseHandle( hFile ) ;
	return TRUE ;
}
