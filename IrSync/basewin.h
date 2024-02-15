#ifndef	APP_BASEWIN
#define	APP_BASEWIN

class Window {
protected:
	HWND m_hwnd ;

public:
	Window() {}
	~Window() {}

	virtual BOOL	Create() { return FALSE ; }

	virtual void	WMCreate( HWND hWnd ) {}
	virtual void	WMDestroy() {}
	virtual void	WMPaint() {}
	virtual	void	WMSetFocus() {}
	virtual	void	WMKillFocus( HWND hWndGetFocus ) {}
	virtual	void	WMChar( TCHAR c, LONG keydata ) {}
	virtual	void	WMKeyDown( int nVirtKey, LONG lKeyData ) {}

	virtual LRESULT	WMCommand( HWND hWnd, UINT msg, WPARAM wp, LPARAM lp ) {
		return 0 ;
	}
	virtual void	WMNotify( HWND hWnd, int idCtrl, LPNMHDR pnmh ) {}
	virtual void	WMDrawItem( HWND hWndChild, LPDRAWITEMSTRUCT dis ) {}
	virtual BOOL	WMClose() { return FALSE ; }
	virtual void	WMActivate( WORD fActive, BOOL fMinimize, HWND hWnd ) {}
	virtual BOOL	WMCopydata( PCOPYDATASTRUCT cds ) { return FALSE ; }
	virtual LRESULT	WMGetDlgCode() { return 0 ; }
	virtual void	WMTimer( int idCtrl ) {}

	HWND	get_hwnd() const { return m_hwnd ; }
} ;

typedef	Window	*PWindow ;

extern "C" {
	BOOL APIENTRY		DlgProc( HWND hDlg, UINT msg, WPARAM wp, LPARAM lp ) ;
	LRESULT CALLBACK	WndProc( HWND hWnd,  UINT msg, WPARAM wParam, LPARAM lParam ) ;
} ;

#endif	/* APP_BASEWIN */
