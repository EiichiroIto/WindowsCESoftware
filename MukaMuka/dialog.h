extern	TCHAR		szWinName[] ;
extern	TCHAR		Version[] ;
extern	HINSTANCE	hInst ;
extern	TCHAR		MukaPath[ MAX_PATH ] ;
extern	BOOL		F_skipstart ;
extern	BOOL		F_nowall ;
extern	BOOL		F_nopos ;

/* ���W�X�g���֌W */
BOOL	get_reg( void ) ;
BOOL	set_reg( void ) ;
void	delete_reg( void ) ;

/* �t�@�C���I�[�v���֌W */
BOOL	FileOpenDlg( HWND hWnd, LPTSTR buf, DWORD size ) ;
void	setup_init_dir( LPCTSTR buf ) ;

/* ABOUT�_�C�A���O�֌W */
BOOL CALLBACK	AboutDlgFunc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam ) ;

/* �����񏈗��֌W */
int		tcsicmp( LPCTSTR str1, LPCTSTR str2 ) ;
LPTSTR	tcsrchr( LPCTSTR str, TCHAR c ) ;
