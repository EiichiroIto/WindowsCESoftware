#include	"basewin.h"

#define		szTitleName		TEXT("CBSave")

class MainWindow ;

class MainApp {
private:
	HINSTANCE	m_hInst ;
	HACCEL		m_hAccl ;
	MainWindow	*m_pMainWnd ;

public:
	MainApp() ;
	~MainApp() ;

	BOOL	Create( HINSTANCE hInst ) ;
	int		Run() ;
	void	CopyParent() const ;
	BOOL	Copy( HWND hWnd ) ;
	BOOL	GetReg() ;
	void	SetReg() const ;
	void	DeleteReg() const ;

	HINSTANCE	get_inst() const {
		return m_hInst ;
	}
} ;

extern "C" {
	extern	TCHAR	MessageBuf[ 512 ] ;
	extern	DWORD	GetFontH( void ) ;
} ;
