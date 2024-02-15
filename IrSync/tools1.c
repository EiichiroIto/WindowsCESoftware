#define	STRICT
#include	<windows.h>
#include	<tchar.h>
#include	"tools.h"

HANDLE		g_hFile1 = INVALID_HANDLE_VALUE ;
BOOL		g_fEof1 = FALSE ;
BOOL		g_fBinary1 = FALSE ;
DWORD		g_dwRin1 = 0 ;
DWORD		g_dwRout1 = 0 ;
DWORD		g_dwWin1 = 0 ;
BYTE		g_rBuf1[ 256 ] ;
BYTE		g_sBuf1[ 256 ] ;

BOOL
Fopen( LPCTSTR path, LPCTSTR modestr )
{
	BOOL	f_trunc = FALSE, f_last = FALSE ;
	DWORD	access = GENERIC_READ ;
	DWORD	mode = FILE_SHARE_READ ;
	DWORD	dist = OPEN_EXISTING ;

	g_fBinary1 = FALSE ;
	if ( *modestr == TEXT('r') ) {
		access = GENERIC_READ ;
		dist = OPEN_EXISTING ;
		if ( modestr[1] == TEXT('+') ) {
			access = GENERIC_READ|GENERIC_WRITE ;
			dist = OPEN_ALWAYS ;
			if ( modestr[2] == TEXT('b') ) {
				g_fBinary1 = TRUE ;
			}
		} else if ( modestr[1] == TEXT('b') ) {
			g_fBinary1 = TRUE ;
		}
	} else if ( *modestr == TEXT('w') ) {
		access = GENERIC_WRITE ;
		dist = CREATE_ALWAYS ;
		f_trunc = TRUE ;
		if ( modestr[1] == TEXT('+') ) {
			access = GENERIC_READ|GENERIC_WRITE ;
			if ( modestr[2] == TEXT('b') ) {
				g_fBinary1 = TRUE ;
			}
		} else if ( modestr[1] == TEXT('b') ) {
			g_fBinary1 = TRUE ;
		}
	} else if ( *modestr == TEXT('a') ) {
		access = GENERIC_WRITE ;
		dist = CREATE_ALWAYS ;
		f_last = TRUE ;
		if ( modestr[1] == TEXT('+') ) {
			access = GENERIC_READ|GENERIC_WRITE ;
			if ( modestr[2] == TEXT('b') ) {
				g_fBinary1 = TRUE ;
			}
		} else if ( modestr[1] == TEXT('b') ) {
			g_fBinary1 = TRUE ;
		}
	} else {
		return FALSE ;
	}
	g_hFile1 = CreateFile( path, access, mode, NULL,
						 dist, FILE_ATTRIBUTE_NORMAL, 0 ) ;
	if ( g_hFile1 == INVALID_HANDLE_VALUE ) {
		return FALSE ;
	}
	if ( f_trunc ) {
		SetEndOfFile( g_hFile1 ) ;
	} else if ( f_last ) {
		SetFilePointer( g_hFile1, 0, NULL, FILE_END ) ;
	}
	g_fEof1 = FALSE ;
	g_dwRin1 = g_dwRout1 = g_dwWin1 = 0 ;
	return TRUE ;
}

void
Fclose( void )
{
	DWORD	nWrite ;

	if ( g_hFile1 != INVALID_HANDLE_VALUE ) {
		if ( g_dwWin1 ) {
			WriteFile( g_hFile1, g_sBuf1, g_dwWin1, &nWrite, NULL ) ;
		}
		CloseHandle( g_hFile1 ) ;
		g_hFile1 = INVALID_HANDLE_VALUE ;
	}
}

int
Fgetc( void )
{
	int		c ;
	BOOL	ret ;
	DWORD	nRead ;

	do {
		if ( g_fEof1 ) {
			return -1 ;
		}
		if ( g_dwRin1 == g_dwRout1 ) {
			g_dwRin1 = g_dwRout1 = 0 ;
			ret = ReadFile( g_hFile1, g_rBuf1, sizeof g_rBuf1, &nRead, NULL ) ;
			if ( !ret || !nRead ) {
				g_fEof1 = TRUE ;
				return -1 ;
			}
			g_dwRin1 = nRead ;
			g_dwRin1 %= sizeof g_rBuf1 ;
		}
		c = (int) g_rBuf1[ g_dwRout1 ] ;
		g_dwRout1 ++ ;
		g_dwRout1 %= sizeof g_rBuf1 ;
	} while ( !g_fBinary1 && c == '\r' ) ;
	return c ;
}

char *
Fgets( char *buf, int size )
{
	int		c ;
	LPBYTE	ptr = buf ;

	size -- ;
	while ( (c = Fgetc()) != -1 ) {
		if ( !size ) {
			break ;
		}
		*ptr++ = c ;
		if ( c == '\n' ) {
			break ;
		}
	}
	*ptr = 0 ;
	return ptr == buf ? NULL : buf ;
}

static	int
Fputc_sub( int c )
{
	DWORD	tmp, nWrite ;

	g_sBuf1[ g_dwWin1 ] = c ;
	g_dwWin1 ++ ;
	g_dwWin1 %= sizeof g_sBuf1 ;
	if ( !g_dwWin1 || c == '\n' ) {
		tmp = g_dwWin1 ? g_dwWin1 : sizeof g_sBuf1 ;
		g_dwWin1 = 0 ;
		if ( !WriteFile( g_hFile1, g_sBuf1, tmp, &nWrite, NULL ) ) {
			return FALSE ;
		} else if ( nWrite == 0 ) {
			return FALSE ;
		}
	}
	return TRUE ;
}

int
Fputc( int c )
{
	if ( !g_fBinary1 && c == '\n' ) {
		if ( !Fputc_sub( '\r' ) ) {
			return -1 ;
		}
	}
	return Fputc_sub( c ) ? c : -1 ;
}

int
Fputs( const char *buf )
{
	BYTE	c ;
	int		last = -1 ;

	while ( c = *buf++ ) {
		last = (int) c ;
		if ( Fputc( last ) == -1 ) {
			return -1 ;
		}
	}
	return last ;
}

void
Fwrite( const char *buf, int size )
{
	if ( !size ) {
		return ;
	}
	do {
		Fputc( *buf++ ) ;
	} while ( -- size ) ;
}

void
slash2backslash( LPTSTR str )
{
	LPTSTR	ptr ;

	/* '/'Ç'\'Ç…ïœä∑Ç∑ÇÈ */
	while ( ptr = _tcschr( str, TEXT('/') ) ) {
		*ptr = TEXT('\\') ;
	}
}

void
catdir( LPTSTR body, LPCTSTR adddir, LPTSTR pPrevDir )
{
	DWORD	len ;
	LPTSTR	ptr ;

	slash2backslash( body ) ;
	if ( pPrevDir ) {
		_tcscpy( pPrevDir, TEXT("..") ) ;
	}
	while ( *adddir ) {
		if ( _tcschr( TEXT("/\\"), *adddir ) ) {
			/* êÊì™Ç™"/"Ç©"\"ÇÃèÍçáÇÕì«Ç›îÚÇŒÇ∑ */
			adddir ++ ;
			continue ;
		}
		if ( adddir[0] == TEXT('.') && adddir[1] == TEXT('.') ) {
			ptr = _tcsrchr( body, TEXT('\\') ) ;
			if ( ptr && pPrevDir ) {
				_tcscpy( pPrevDir, ptr + 1 ) ;
			}
			if ( ptr && ptr != body ) {
				*ptr = 0 ;
			} else {
				_tcscpy( body, TEXT("\\") ) ;
			}
			adddir += 2 ;
		} else if ( adddir[0] == TEXT('.') ) {
			adddir ++ ;
		} else {
			break ;
		}
	}
	if ( !*adddir ) {
		return ;
	}
	len = _tcslen( body ) ;
	if ( !len || body[ len - 1 ] != TEXT('\\') ) {
		body[ len ++ ] = TEXT('\\') ;
	}
	_tcscpy( &body[ len ], adddir ) ;
	slash2backslash( body ) ;
}
