#include	"basewin.h"
#include	"dialog.h"

#ifndef	_H_APPSUB
#define	_H_APPSUB

class MainApp ;

/*
 * メインウィンドウ
 */
class MainWindow : public Window {
private:
	MainApp			*m_pApp ;		/* アプリケーションへのポインタ */
	HWND			m_hwndCB ;		/* CommandBarのハンドル */
	HICON			m_hIcon ;		/* アプリアイコンのハンドル */
	HFONT			m_hFont ;		/* フォントのハンドル */
	BOOL			m_bDeleteReg ;	/* レジストリを削除する場合*真* */
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
