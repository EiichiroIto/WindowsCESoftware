#include	"basewin.h"
#include	"dialog.h"

#ifndef	_H_APPSUB
#define	_H_APPSUB

class MainApp ;

/*
 * 一覧表示
 */
class ListArea {
private:
	HWND			m_hWnd ;		/* 対応するウィンドウハンドル */

public:
	HWND	get_hwnd() const { return m_hWnd ; }
	HWND	Create( HINSTANCE hInst, HWND hWndParent, DWORD x, DWORD y, DWORD w, DWORD h, WORD id ) ;

	void	AddItem( PTransInfo fi ) const ;
	void	ModifyItem( int index, const PTransInfo fi ) const ;
	void	DeleteItem( int index ) const ;
	BOOL	GetItem( int index, PTransInfo fi ) const ;
	int		GetCurSel() const ;
	int		GetCount() const ;
	void	Clear() const ;

	void	SetProc( int (*proc)( int ln, const BYTE **buf, int *len ) ) ;
	void	SetCurSel( DWORD index ) ;
	void	SendKey( DWORD nVirtKey ) const ;
	void	SetFocus() const { ::SetFocus( m_hWnd ) ; }
	void	SetKeyMask( BOOL fTitle ) ;
} ;

/*
 * メインウィンドウ
 */
class MainWindow : public Window {
private:
	MainApp			*m_pApp ;		/* アプリケーションへのポインタ */
	HWND			m_hWndCB ;		/* CommandBarのハンドル */
	HICON			m_hIcon ;		/* アプリアイコンのハンドル */
	BOOL			m_fDeleteReg ;	/* レジストリを削除する場合*真* */

	DWORD	CreateCommandBar() ;

public:
	MainWindow( MainApp * ) ;
	~MainWindow() ;

	BOOL	Create() ;
	void	WMCreate( HWND hWnd ) ;
	void	WMDestroy() ;
	void	WMSetFocus() ;
	LRESULT	WMCommand( HWND hWnd, UINT msg, WPARAM wp, LPARAM lp ) ;
	void	WMNotify( HWND hWnd, int idCtrl, LPNMHDR pnmh ) ;

	BOOL	WMClose() ;

	static	BOOL	RegisterClass( HINSTANCE hInst ) ;
	static	LPCTSTR	GetClassName() { return TEXT("IRCOPY") ; }
} ;

#endif	/* _H_APPSUB */
