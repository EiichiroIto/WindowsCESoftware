#include	"basewin.h"

#ifndef	_H_APPSUB
#define	_H_APPSUB

class TrainApp ;
class ResultArea ;

class StationArea {
private:
	HWND		m_hWnd ;		/* 対応するウィンドウハンドル */
	HWND		m_hWndLine ;	/* 出発/到着時利用路線のハンドル */
	LINEID		*m_pTermLine ;	/* 出発/到着時利用路線のポインタ */
	BOOL		m_fValid ;		/* インデックスが有効な場合*真* */
	DWORD		m_dIndex ;		/* 入力した駅のインデックス */
	ResultArea	*m_pResult ;	/* 結果表示領域へのポインタ */

public:
	HWND	get_hwnd() const { return m_hWnd ; }
	DWORD	get_index() const { return m_dIndex ; }
	BOOL	is_valid() const { return m_fValid ; }
	DWORD	Create( HINSTANCE hInst, HWND hWndParent, DWORD x, DWORD y, DWORD w, WORD id1, WORD id2 ) ;
	BOOL	confirm() ;
	void	set_station_index( DWORD station_index ) ;
	void	set_sort_index( DWORD sort_index ) ;
	void	GetSjis( LPSTR sjis, DWORD size ) const ;
	void	SetFocus() const { ::SetFocus( m_hWnd ) ; }
	void	set_term_line( LINEID lineID ) const ;
	void	Clear() ;
	BOOL	is_empty() const ;
	DWORD	GetHeight() const ;

	StationArea( LINEID *pLineID ) ;
} ;

class ResultArea {
private:
	HWND		m_hWnd ;		/* 対応するウィンドウハンドル */

public:
	HWND	get_hwnd() const { return m_hWnd ; }
	HWND	Create( HINSTANCE hInst, HWND hWndParent, DWORD x, DWORD y, DWORD w, DWORD h, WORD id ) ;
	void	SetProc( int (*proc)( int ln, const BYTE **buf, int *len ) ) ;
	DWORD	GetCurSel() const ;
	void	SetCurSel( DWORD index ) ;
	void	SendKey( DWORD nVirtKey ) const ;
	void	SetFocus() const { ::SetFocus( m_hWnd ) ; }
	void	Clear() ;
} ;

/*
 * 時間表示
 */
class TimeArea {
private:
	HWND		m_hWnd ;		/* 対応するウィンドウハンドル */
	int			*m_pTime ;		/* 時刻を保持している変数へのポインタ */

public:
	HWND	get_hwnd() const { return m_hWnd ; }
	DWORD	Create( HINSTANCE hInst, HWND hWndParent, DWORD x, DWORD y, WORD id, WORD bmpID ) ;
	void	Refresh() const ;
	void	Clear() ;
	DWORD	GetHeight() const ;

	TimeArea( int *time ) ;
} ;

/*
 * 特急・新幹線・その他選択
 */
class KindArea {
private:
	HWND		m_hWnd ;		/* 対応するウィンドウハンドル */
	int			*m_pFlag ;		/* フラグを保持している変数へのポインタ */
	int			m_dMask ;		/* フラグのマスク */

public:
	HWND	get_hwnd() const { return m_hWnd ; }
	DWORD	Create( HINSTANCE hInst, HWND hWndParent, DWORD x, DWORD y, WORD id, LPCSTR text ) ;
	void	Refresh() const ;
	void	Toggle() ;
	DWORD	GetHeight() const ;

	KindArea( int *data, int mask ) ;
} ;

/*
 * 徒歩時間・待ち時間・乗換回数
 */
class MaxArea {
private:
	HWND		m_hWnd ;		/* 対応するウィンドウハンドル */
	int			*m_pData ;		/* データを保持している変数へのポインタ */
	int			m_dMaxValue ;	/* 無指定とみなす値 */

public:
	HWND	get_hwnd() const { return m_hWnd ; }
	DWORD	Create( HINSTANCE hInst, HWND hWndParent, DWORD x, DWORD y, WORD idCtrl, WORD bmpID ) ;
	void	SetFocus() const { ::SetFocus( m_hWnd ) ; }
	void	Refresh() const ;
	void	Update() const ;
	DWORD	GetHeight() const ;

	MaxArea( int *data, int max ) ;

	static DWORD	GetWidth() { return GetFontHW() * 2 + 16 + 6 ; }
} ;

/*
 * メインウィンドウ
 */
class MainWindow : public Window {
private:
	TrainApp		*m_pApp ;		/* アプリケーションへのポインタ */
	HWND			m_hWndCB ;		/* CommandBarのハンドル */
	HICON			m_hIcon ;		/* アプリアイコンのハンドル */
	BOOL			m_fDeleteReg ;	/* レジストリを削除する場合*真* */

	DWORD	CreateCommandBar() ;

public:
	MainWindow( TrainApp * ) ;
	~MainWindow() ;

	BOOL	Create() ;
	void	WMCreate( HWND hWnd ) ;
	void	WMDestroy() ;
	void	WMSetFocus() ;
	LRESULT	WMCommand( HWND hWnd, UINT msg, WPARAM wp, LPARAM lp ) ;
	void	WMNotify( HWND hWnd, int idCtrl, LPNMHDR pnmh ) ;
	BOOL	WMClose() ;
	void	SetCBState( DWORD idCtrl, BOOL fState ) ;

	static	BOOL	RegisterClass( HINSTANCE hInst ) ;
	static	LPCTSTR	GetClassName() { return TEXT("TRAIN") ; }
} ;

extern	BOOL	g_bNarrow ;

#endif	/* _H_APPSUB */
