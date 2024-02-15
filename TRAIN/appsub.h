#include	"basewin.h"

#ifndef	_H_APPSUB
#define	_H_APPSUB

class TrainApp ;
class ResultArea ;

class StationArea {
private:
	HWND		m_hWnd ;		/* �Ή�����E�B���h�E�n���h�� */
	HWND		m_hWndLine ;	/* �o��/���������p�H���̃n���h�� */
	LINEID		*m_pTermLine ;	/* �o��/���������p�H���̃|�C���^ */
	BOOL		m_fValid ;		/* �C���f�b�N�X���L���ȏꍇ*�^* */
	DWORD		m_dIndex ;		/* ���͂����w�̃C���f�b�N�X */
	ResultArea	*m_pResult ;	/* ���ʕ\���̈�ւ̃|�C���^ */

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
	HWND		m_hWnd ;		/* �Ή�����E�B���h�E�n���h�� */

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
 * ���ԕ\��
 */
class TimeArea {
private:
	HWND		m_hWnd ;		/* �Ή�����E�B���h�E�n���h�� */
	int			*m_pTime ;		/* ������ێ����Ă���ϐ��ւ̃|�C���^ */

public:
	HWND	get_hwnd() const { return m_hWnd ; }
	DWORD	Create( HINSTANCE hInst, HWND hWndParent, DWORD x, DWORD y, WORD id, WORD bmpID ) ;
	void	Refresh() const ;
	void	Clear() ;
	DWORD	GetHeight() const ;

	TimeArea( int *time ) ;
} ;

/*
 * ���}�E�V�����E���̑��I��
 */
class KindArea {
private:
	HWND		m_hWnd ;		/* �Ή�����E�B���h�E�n���h�� */
	int			*m_pFlag ;		/* �t���O��ێ����Ă���ϐ��ւ̃|�C���^ */
	int			m_dMask ;		/* �t���O�̃}�X�N */

public:
	HWND	get_hwnd() const { return m_hWnd ; }
	DWORD	Create( HINSTANCE hInst, HWND hWndParent, DWORD x, DWORD y, WORD id, LPCSTR text ) ;
	void	Refresh() const ;
	void	Toggle() ;
	DWORD	GetHeight() const ;

	KindArea( int *data, int mask ) ;
} ;

/*
 * �k�����ԁE�҂����ԁE�抷��
 */
class MaxArea {
private:
	HWND		m_hWnd ;		/* �Ή�����E�B���h�E�n���h�� */
	int			*m_pData ;		/* �f�[�^��ێ����Ă���ϐ��ւ̃|�C���^ */
	int			m_dMaxValue ;	/* ���w��Ƃ݂Ȃ��l */

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
 * ���C���E�B���h�E
 */
class MainWindow : public Window {
private:
	TrainApp		*m_pApp ;		/* �A�v���P�[�V�����ւ̃|�C���^ */
	HWND			m_hWndCB ;		/* CommandBar�̃n���h�� */
	HICON			m_hIcon ;		/* �A�v���A�C�R���̃n���h�� */
	BOOL			m_fDeleteReg ;	/* ���W�X�g�����폜����ꍇ*�^* */

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
