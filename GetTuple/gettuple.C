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
 * ���W�X�g���o�^�̏ꏊ���󂢂Ă��邩�ǂ������ׂ�
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
 * ���W�X�g����o�^����
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

	/* PCMCIA �G���g�� */
	/* ���W�X�g���L�[���쐬���� */
	lret = RegCreateKeyEx( HKEY_LOCAL_MACHINE, GetTupleKey, 0, TEXT(""), 0,
						   0,
						   NULL, &hk, &ret ) ;
	if ( lret != ERROR_SUCCESS ) {
		return FALSE ;
	}
	/* DLL�� */
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
	/* ���W�X�g���N���[�Y */
	RegCloseKey( hk ) ;
	if ( !rc ) {
		RegDeleteKey( HKEY_LOCAL_MACHINE, GetTupleKey ) ;
	}
	/* DETECT �G���g�� */
	wsprintf( subkey, TEXT("%s%02d"), DetectBase, index ) ;
	/* ���W�X�g���L�[���쐬���� */
	lret = RegCreateKeyEx( HKEY_LOCAL_MACHINE, subkey, 0, TEXT(""), 0,
						   0,
						   NULL, &hk, &ret ) ;
	if ( lret != ERROR_SUCCESS ) {
		return FALSE ;
	}
	/* DLL�� */
	name   = TEXT("Dll") ;
	dwType = REG_SZ ;
	lpData = (LPBYTE) DllName ;
	cbData = (_tcslen(DllName) + 1) * sizeof (*DllName) ;
	lret   = RegSetValueEx( hk, name, 0, dwType, lpData, cbData ) ;
	rc = rc && (lret == ERROR_SUCCESS) ;
	/* �G���g���� */
	name   = TEXT("Entry") ;
	dwType = REG_SZ ;
	lpData = (LPBYTE) EntryFunction ;
	cbData = (_tcslen(EntryFunction) + 1) * sizeof (*EntryFunction) ;
	lret   = RegSetValueEx( hk, name, 0, dwType, lpData, cbData ) ;
	rc = rc && (lret == ERROR_SUCCESS) ;
	/* ���W�X�g���N���[�Y */
	RegCloseKey( hk ) ;
	/* ���W�X�g���������ݎ��s */
	if ( !rc ) {
		RegDeleteKey( HKEY_LOCAL_MACHINE, subkey ) ;
		RegDeleteKey( HKEY_LOCAL_MACHINE, GetTupleKey ) ;
		return FALSE ;
	}
	return TRUE ;
}

/*
 * �o�^�������W�X�g�����폜����
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

	/* �F�������f�o�C�X��T�� */
	wsprintf( buf, TEXT("%s1:"), Prefix ) ;
	for ( i = 0 ; i < 10 ; i ++ ) {
		Sleep( 1000 ) ;
		hDevice = CreateFile( buf, GENERIC_READ|GENERIC_WRITE, 0, NULL,
							  OPEN_EXISTING, 0, NULL ) ;
		if ( hDevice != INVALID_HANDLE_VALUE ) {
			/* ���������ꍇ */
			break ;
		}
		Sleep( 2000 ) ;
	}
	if ( hDevice == INVALID_HANDLE_VALUE ) {
		/* ������Ȃ������ꍇ */
		ptr = TEXT("ERROR:\r\n")
			TEXT("  Could not find any PCMCIA card.") ;
		MessageBox( NULL, ptr, TEXT("GetTuple"), MB_OK ) ;
		return FALSE ;
	}
	/* �^�v���f�[�^��ǂݍ��� */
	memset( TupleData, 0, sizeof TupleData ) ;
	ret = DeviceIoControl( hDevice, IOCTL_PCMCIA_TEST_TUPLES, NULL, 0,
						   TupleData, sizeof TupleData,
						   &numTupleData, NULL ) ;
	CloseHandle( hDevice ) ;
	if ( !ret || !numTupleData ) {
		/* �^�v���f�[�^��ǂݍ��߂Ȃ������ꍇ */
		ptr = TEXT("ERROR:\r\n")
			TEXT("  Could not read tuple data.") ;
		MessageBox( NULL, ptr, TEXT("GetTuple"), MB_OK ) ;
		return FALSE ;
	}
	/* �^�v���f�[�^���������� */
	hFile = CreateFile( TEXT("GETTUPLE.OUT"), GENERIC_WRITE, 0, NULL,
						CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL ) ;
	if ( hFile == INVALID_HANDLE_VALUE ) {
		/* �^�v���f�[�^���������߂Ȃ������ꍇ */
		ptr = TEXT("ERROR:\r\n")
			TEXT("  Could not write tuple data.") ;
		MessageBox( NULL, ptr, TEXT("GetTuple"), MB_OK ) ;
		return FALSE ;
	}
	WriteFile( hFile, TupleData, numTupleData, &nWriteSize, NULL ) ;
	CloseHandle( hFile ) ;
	return TRUE ;
}