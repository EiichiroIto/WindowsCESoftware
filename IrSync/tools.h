#ifndef	APP_TOOLS1
#define	APP_TOOLS1

#ifdef	__cplusplus
extern "C" {
#endif

BOOL	Fopen( LPCTSTR path, LPCTSTR modestr ) ;
void	Fclose( void ) ;
int		Fgetc( void ) ;
char*	Fgets( char *buf, int size ) ;
int		Fputc( int c ) ;
int		Fputs( const char *buf ) ;
void	Fwrite( const char *buf, int size ) ;

void	slash2backslash( LPTSTR str ) ;
void	catdir( LPTSTR body, LPCTSTR adddir, LPTSTR pPrevDir ) ;

extern	HANDLE		g_hFile1 ;

#ifdef	__cplusplus
} ;
#endif

#endif	/* APP_TOOLS1 */
