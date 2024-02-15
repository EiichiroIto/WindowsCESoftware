#ifdef	__cplusplus
extern "C" {
#endif

BOOL	GetOpenFileName2( OPENFILENAME *pOfn, LPCTSTR pAppName ) ;
BOOL	GetSaveFileName2( OPENFILENAME *pOfn ) ;
void	AddRecent( LPCTSTR pAppName, LPCTSTR pPath ) ;

#ifdef	__cplusplus
} ;
#endif
