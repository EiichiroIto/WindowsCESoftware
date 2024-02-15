#include	"basewin.h"
#include	"dialog.h"

#ifndef	_H_APPSUB
#define	_H_APPSUB

class MainApp ;

/*
 * ���C���E�B���h�E
 */
class MainWindow : public Window {
private:
	MainApp			*m_pApp ;		/* �A�v���P�[�V�����ւ̃|�C���^ */
	HWND			m_hwndCB ;		/* CommandBar�̃n���h�� */
	HICON			m_hIcon ;		/* �A�v���A�C�R���̃n���h�� */
	HFONT			m_hFont ;		/* �t�H���g�̃n���h�� */
	BOOL			m_bDeleteReg ;	/* ���W�X�g�����폜����ꍇ*�^* */
	HWND			m_hwndDirEdit ;
	HWND			m_hwndDirList ;
	TCHAR			m_szCopyDir[ MAX_PATH ] ;
	TCHAR			m_szCopyFile[ MAX_PATH ] ;
	BOOL			m_bCopy ;

	DWORD	CreateCommandBar() ;

public:
	MainWindow( MainApp * ) ;
	~MainWindow() ;

	BOOL	Create() ;
	void	WMCreate( HWND hWnd ) ;
	void	WMDestroy() ;
	LRESULT	WMCommand( HWND hWnd, UINT msg, WPARAM wp, LPARAM lp ) ;
	void	WMSetFocus() ;
	BOOL	WMClose() ;

	BOOL	CutCopySelectedFile( BOOL bCopy ) ;
	BOOL	PasteSelectedFile() ;
	BOOL	DeleteSelectedFile() ;

	static	BOOL	RegisterClass( HINSTANCE hInst ) ;
	static	LPCTSTR	GetClassName() { return szClassName ; }
} ;

#endif	/* _H_APPSUB */
