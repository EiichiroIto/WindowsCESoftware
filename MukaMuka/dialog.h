extern	TCHAR		szWinName[] ;
extern	TCHAR		Version[] ;
extern	HINSTANCE	hInst ;
extern	TCHAR		MukaPath[ MAX_PATH ] ;
extern	BOOL		F_skipstart ;
extern	BOOL		F_nowall ;
extern	BOOL		F_nopos ;

/* レジストリ関係 */
BOOL	get_reg( void ) ;
BOOL	set_reg( void ) ;
void	delete_reg( void ) ;

/* ファイルオープン関係 */
BOOL	FileOpenDlg( HWND hWnd, LPTSTR buf, DWORD size ) ;
void	setup_init_dir( LPCTSTR buf ) ;

/* ABOUTダイアログ関係 */
BOOL CALLBACK	AboutDlgFunc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam ) ;

/* 文字列処理関係 */
int		tcsicmp( LPCTSTR str1, LPCTSTR str2 ) ;
LPTSTR	tcsrchr( LPCTSTR str, TCHAR c ) ;
