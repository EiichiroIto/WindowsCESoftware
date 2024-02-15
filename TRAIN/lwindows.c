#include	"lfunc.h"
#include	"kctrl.h"
//#include	<stdio.h>
//#include	<stdlib.h>

BYTE	EmptyStr[1] = {0} ;

void
fatal( const BYTE msg[] )
{
	TCHAR	unicode[ 128 ] ;

	sjis2unicode( msg, unicode, sizeof unicode ) ;
	MessageBox( NULL, unicode, TEXT("TRAIN"), MB_OK|MB_ICONASTERISK ) ;
	ExitThread(99) ;
}

void
wcs_fatal( LPCTSTR msg )
{
	MessageBox( NULL, msg, TEXT("TRAIN"), MB_OK|MB_ICONASTERISK ) ;
	ExitThread(99) ;
}

void
fatal_msg( int val )
{
	BYTE	*p ;

	switch ( val ) {
	case 3:
		p = "E005:Obsolete Data" ;
		break ;
	case 6:
		p = "E009:Insufficient nodes" ;
		break ;
	case 19:
		p = "E011:Insufficient RSTAT" ;
		break ;
	case 20:
		p = "???" ;
		break ;
	default:
		p = "unknown" ;
		break ;
	}
	fatal( p ) ;
}

/*----------------------------------------------------------------------------
 * 低レベルファイル関数
 *--------------------------------------------------------------------------*/
HANDLE		hFile = INVALID_HANDLE_VALUE ;

void
Fclose( void )
{
	if ( hFile != INVALID_HANDLE_VALUE ) {
		CloseHandle( hFile ) ;
		hFile = INVALID_HANDLE_VALUE ;
	}
}

BOOL
Fopen( LPCTSTR fn, LPCTSTR mode )
{
	BOOL	f_last ;
	DWORD	access, crmode ;

	f_last = FALSE ;
	access = crmode = 0 ;
	if ( *mode == L'r' && *(mode+1) != L'+' ) {
		access = GENERIC_READ ;
		crmode = OPEN_EXISTING ;
	} else if ( *mode == L'w' && *(mode+1) != L'+' ) {
		access = GENERIC_WRITE ;
		crmode = CREATE_ALWAYS ;
	} else if ( *mode == L'a' ) {
		access = GENERIC_WRITE ;
		crmode = OPEN_ALWAYS ;
		f_last = TRUE ;
	} else {
		access = GENERIC_READ|GENERIC_WRITE ;
		crmode = OPEN_ALWAYS ;
	}
	hFile = CreateFile( fn,
						access,
						FILE_SHARE_READ|FILE_SHARE_WRITE,
						NULL,
						crmode,
						FILE_ATTRIBUTE_NORMAL,
						NULL ) ;
	if ( hFile == INVALID_HANDLE_VALUE ) {
		return FALSE ;
	}
	if ( f_last ) {
		SetFilePointer( hFile, 0, NULL, FILE_END ) ;
	}
	return TRUE ;
}

int
Fread( void* buf, int size )
{
	DWORD	readsize ;
	ReadFile( hFile, buf, (DWORD) size, &readsize, NULL ) ;
	return readsize ;
}

WORD
Fread_far( void* _buf, WORD size )
{
	return (WORD) Fread( _buf, size ) ;
}

int
FreadBuf( void* buf, int size )
{
	return Fread( buf, size ) ;
}

int
Fwrite( void* buf, int size )
{
	DWORD	writesize = 0 ;
	WriteFile( hFile, buf, (DWORD) size, &writesize, NULL ) ;
	return writesize ;
}

int
Fwrite_far( void* buf, WORD size )
{
	return Fwrite( buf, (DWORD) size ) ;
}

void
Fseek( long ptr, int whence )
{
	SetFilePointer( hFile, ptr, NULL, whence ) ;
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

void
get_nowstr( BYTE *buf )
{
	SYSTEMTIME	now ;
	TCHAR		unicode[ 30 ] ;

	GetLocalTime( &now ) ;
	wsprintf( unicode, TEXT("%02d-%02d-%02d %02d:%02d"),
			  now.wYear % 100, now.wMonth, now.wDay,
			  now.wHour, now.wMinute ) ;
	unicode2sjis( unicode, buf, 16 ) ;
}

BYTE	*Msg0[] = {
	"[待]", "[詳]", "特", "新", "他",
	"歩", "待", "乗", "計", "  ↓ ",
	"  ", "    ", "    ", "  ◇ ",
} ;
BYTE	*Msg1[] = {
	"You can't use any routes"
} ;

BYTE*
get_msg( int kind, int idx )
{
	static	BYTE	msg[ 128 ] ;

	if ( kind == 0 ) {
		return Msg0[ idx ] ;
	} else if ( kind == 1 ) {
		return Msg1[ idx ] ;
	}
	return msg ;
}
