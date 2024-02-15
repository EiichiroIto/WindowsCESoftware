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

#ifdef	__cplusplus
} ;
#endif

#endif	/* APP_TOOLS1 */
