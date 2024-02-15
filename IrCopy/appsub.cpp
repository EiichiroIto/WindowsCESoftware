#define	STRICT
#include	<windows.h>
#include	<windowsx.h>
#include	<commctrl.h>
#include	<tchar.h>
#include	"resource.h"
#include	"appmain.h"
#include	"appsub.h"

/*-------------------------------------------------------------------------*
 * ListArea
 *-------------------------------------------------------------------------*/
/*
 * ウィンドウを作成する
 */
HWND
ListArea::Create( HINSTANCE hInst, HWND hWndParent, DWORD x, DWORD y, DWORD w, DWORD h, WORD id )
{
	LV_COLUMN	lvc ;

	m_hWnd = CreateWindow( WC_LISTVIEW,
					TEXT(""),
					WS_VISIBLE|WS_CHILD|WS_VSCROLL|
					LVS_REPORT|LVS_SINGLESEL|WS_BORDER,
					x, y, w, h,
					hWndParent,
					(HMENU) id,
					hInst,
					NULL ) ;
	/* カラム情報を設定する */
	memset( &lvc, 0, sizeof lvc ) ;
	/* SourcePath */
	lvc.mask = LVCF_FMT|LVCF_TEXT|LVCF_WIDTH ;
	lvc.fmt = LVCFMT_LEFT ;
	lvc.cx = w / 2 - 5 ;
	lvc.pszText = TEXT("Source") ;
	lvc.iSubItem = 0 ;
	ListView_InsertColumn( m_hWnd, 0, &lvc ) ;
	/* DestinationPath */
	lvc.mask = LVCF_FMT|LVCF_TEXT|LVCF_WIDTH|LVCF_SUBITEM ;
	lvc.fmt = LVCFMT_LEFT ;
	lvc.cx = w / 2 - 5 ;
	lvc.pszText = TEXT("Destination") ;
	lvc.iSubItem = 1 ;
	ListView_InsertColumn( m_hWnd, 1, &lvc ) ;
	return m_hWnd ;
}

void
ListArea::AddItem( PTransInfo ti ) const
{
	LV_ITEM     lvi ;
	int			index ;

	memset( &lvi, 0, sizeof lvi ) ;
	/* SourcePath */
	lvi.iItem = ListView_GetItemCount( m_hWnd ) ;
	lvi.iSubItem = 0 ;
	lvi.mask = LVIF_TEXT ;
	lvi.pszText = ti->srcPath ;
	index = ListView_InsertItem( m_hWnd, &lvi ) ;
	if ( index == -1 ) {
		return ;
	}
	/* DestinationPath */
	lvi.iItem = index ;
	lvi.iSubItem = 1 ;
	lvi.mask = LVIF_TEXT ;
	lvi.pszText = ti->dstPath ;
	ListView_SetItem( m_hWnd, &lvi ) ;
}

void
ListArea::ModifyItem( int index, const PTransInfo ti ) const
{
	LV_ITEM     lvi ;

	memset( &lvi, 0, sizeof lvi ) ;
	/* SourcePath */
	lvi.iItem = index ;
	lvi.iSubItem = 0 ;
	lvi.mask = LVIF_TEXT ;
	lvi.pszText = ti->srcPath ;
	ListView_SetItem( m_hWnd, &lvi ) ;
	/* DestinationPath */
	lvi.iItem = index ;
	lvi.iSubItem = 1 ;
	lvi.mask = LVIF_TEXT ;
	lvi.pszText = ti->dstPath ;
	ListView_SetItem( m_hWnd, &lvi ) ;
}

void
ListArea::DeleteItem( int index ) const
{
	ListView_DeleteItem( m_hWnd, index ) ;
}

BOOL
ListArea::GetItem( int index, PTransInfo ti ) const
{
	LV_ITEM     lvi ;

	*ti->srcPath = *ti->dstPath = 0 ;
	memset( &lvi, 0, sizeof lvi ) ;
	/* SourcePath */
	lvi.iItem = index ;
	lvi.iSubItem = 0 ;
	lvi.mask = LVIF_TEXT ;
	lvi.pszText = ti->srcPath ;
	lvi.cchTextMax = MAX_PATH ;
	if ( !ListView_GetItem( m_hWnd, &lvi ) ) {
		return FALSE ;
	}
	/* DestinationPath */
	lvi.iItem = index ;
	lvi.iSubItem = 1 ;
	lvi.mask = LVIF_TEXT ;
	lvi.pszText = ti->dstPath ;
	lvi.cchTextMax = MAX_PATH ;
	return ListView_GetItem( m_hWnd, &lvi ) ;
}

int
ListArea::GetCurSel() const
{
	return ListView_GetNextItem( m_hWnd, -1, LVNI_FOCUSED ) ;
}

int
ListArea::GetCount() const
{
	return ListView_GetItemCount( m_hWnd ) ;
}

void
ListArea::Clear() const
{
	ListView_DeleteAllItems( m_hWnd ) ;
}

/*-------------------------------------------------------------------------*
 * MainWindow
 *-------------------------------------------------------------------------*/
MainWindow::MainWindow( MainApp *pApp )
{
	m_pApp = pApp ;
	m_hIcon = 0 ;
}

MainWindow::~MainWindow()
{
	if ( m_hIcon ) {
		::DeleteObject( m_hIcon ) ;
	}
}

void
MainWindow::WMCreate( HWND hWnd )
{
	RECT		rect ;
	DWORD		height = 2, tw, h, th ;
	HINSTANCE	hInst = m_pApp->get_inst() ;

	m_hWnd = hWnd ;
	LONG	lStyle = GetWindowLong( hWnd, GWL_STYLE ) ;
#ifdef	_WIN32_WCE
	lStyle &= ~WS_CAPTION ;
#endif	/* _WIN32_WCE */
	SetWindowLong( hWnd, GWL_STYLE, lStyle ) ;
	/* コマンドバーを追加する */
	height += CreateCommandBar() ;
	/* 表示領域を求める */
	GetClientRect( m_hWnd, &rect ) ;
	rect.right -= 2 ;
	rect.top += height ;
	rect.bottom -= 2 ;
	/* アイコンを作成しタスクバーに登録する */
	m_hIcon = (HICON) LoadImage( m_pApp->get_inst(), MAKEINTRESOURCE(IDI_APPICON),
								 IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR ) ;
	if ( m_hIcon ) {
		SendMessage( hWnd, WM_SETICON, FALSE, (LPARAM)m_hIcon ) ;
	}
#ifdef	_WIN32_WCE
	tw = GetSystemMetrics( SM_CXSCREEN ) ;
	th = GetSystemMetrics( SM_CYSCREEN ) ;
#else	/* _WIN32_WCE */
	tw = 480 ;
	th = 240 ;
#endif	/* _WIN32_WCE */
	h = height + 2 ;
	/* 一覧表示領域を作成する */
	m_pApp->get_list()->Create( hInst, m_hWnd, 2, h, tw - 8, th - h - 35,
								IDC_LIST ) ;
}

void
MainWindow::WMDestroy()
{
	m_pApp->GetFromList() ;
	m_pApp->WriteToFile() ;
	PostQuitMessage( 0 ) ;
}

LRESULT
MainWindow::WMCommand( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	TransInfo	ti ;
	int			index ;
	WORD		idCtrl = LOWORD(wParam) ;
	WORD		notifyCode = HIWORD(wParam) ;
	BOOL		fQuit = FALSE, fDelete = FALSE ;

	switch ( idCtrl ) {
	case IDM_ABOUT:
		if ( AboutDialogExec( m_pApp->get_inst(), hWnd, &fDelete ) ) {
			if ( fDelete ) {
//				m_fDeleteReg = TRUE ;
				SendMessage( hWnd, WM_CLOSE, 0, 0 ) ;
			}
		}
		break ;
	case IDM_INIT:
		m_pApp->InitialRead() ;
		break ;
	case IDM_IMPORT:
		m_pApp->Import() ;
		break ;
	case IDM_EXPORT:
		m_pApp->Export() ;
		break ;
	case IDM_EXIT:
		SendMessage( hWnd, WM_CLOSE, 0, 0 ) ;
		break ;
	case IDM_ADD_FILE:
		memset( &ti, 0, sizeof ti ) ;
		if ( AddDialogExec( m_pApp->get_inst(), hWnd, FALSE, &ti ) ) {
			m_pApp->get_list()->AddItem( &ti ) ;
		}
		break ;
	case IDM_PROP_FILE:
		index = m_pApp->get_list()->GetCurSel() ;
		if ( index < 0 ) {
			break ;
		}
		memset( &ti, 0, sizeof ti ) ;
		m_pApp->get_list()->GetItem( index, &ti ) ;
		if ( AddDialogExec( m_pApp->get_inst(), hWnd, TRUE, &ti ) ) {
			m_pApp->get_list()->ModifyItem( index, &ti ) ;
		}
		break ;
	case IDM_DEL_FILE:
		index = m_pApp->get_list()->GetCurSel() ;
		if ( index < 0 ) {
			break ;
		}
		if ( MessageBox( hWnd, TEXT("Delete the transfer?"), TEXT("Delete Transfer"), MB_YESNO ) != IDYES ) {
			break ;
		}
		m_pApp->get_list()->DeleteItem( index ) ;
		break ;
	case IDM_EXECUTE:
		m_pApp->GetFromList() ;
		TransferDialogExec( m_pApp->get_inst(), hWnd, 0, m_pApp ) ;
		m_pApp->RefreshList() ;
		break ;
	case IDM_SEND:
		m_pApp->GetFromList() ;
		TransferDialogExec( m_pApp->get_inst(), hWnd, 1, m_pApp ) ;
		m_pApp->RefreshList() ;
		break ;
	case IDM_RECEIVE:
		m_pApp->GetFromList() ;
		TransferDialogExec( m_pApp->get_inst(), hWnd, 2, m_pApp ) ;
		m_pApp->RefreshList() ;
		break ;
	}
	return 0 ;
}

void
MainWindow::WMNotify( HWND hWnd, int idCtrl, LPNMHDR pnmh )
{
	switch ( pnmh->idFrom ) {
	case IDC_LIST:
		if ( pnmh->code == NM_DBLCLK ) {
			PostMessage( hWnd, WM_COMMAND, IDM_PROP_FILE, 0 ) ;
		} else if ( pnmh->code == LVN_KEYDOWN ) {
			switch ( ((LV_KEYDOWN*) pnmh)->wVKey ) {
			case VK_RETURN:
				PostMessage( hWnd, WM_COMMAND, IDM_PROP_FILE, 0 ) ;
				break ;
			case VK_DELETE:
				PostMessage( hWnd, WM_COMMAND, IDM_DEL_FILE, 0 ) ;
				break ;
			}
		}
		break ;
	}
}

void
MainWindow::WMSetFocus()
{
	m_pApp->get_list()->SetFocus() ;
}

BOOL
MainWindow::WMClose()
{
	return FALSE ;
}

BOOL
MainWindow::Create()
{
	HWND	hWnd = CreateWindowEx( 0,
						GetClassName(),
						szTitleName,
						WS_VISIBLE,
#ifdef	_WIN32_WCE
						0, 0, CW_USEDEFAULT, CW_USEDEFAULT,
#else	/* _WIN32_WCE */
						0, 0, 480, 240,
#endif	/* _WIN32_WCE */
						NULL,
						NULL,
						m_pApp->get_inst(),
						this ) ;
	if ( hWnd == 0 ) {
		return FALSE ;
	}
    ShowWindow( hWnd, SW_SHOW ) ;
    UpdateWindow( hWnd ) ;
    return TRUE ;
}

DWORD
MainWindow::CreateCommandBar()
{
	HINSTANCE	hInst = m_pApp->get_inst() ;

#ifdef	_WIN32_WCE
	static TBBUTTON	tbButton[] = {
		{0,              0, TBSTATE_ENABLED, TBSTYLE_SEP,    0, 0, 0, -1},
		{2,   IDM_ADD_FILE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, -1},
		{0,              0, TBSTATE_ENABLED, TBSTYLE_SEP,    0, 0, 0, -1},
		{0,     IDM_IMPORT, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, -1},
		{1,     IDM_EXPORT, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, -1},
		{0,              0, TBSTATE_ENABLED, TBSTYLE_SEP,    0, 0, 0, -1},
		{7,   IDM_DEL_FILE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, -1},
		{6,  IDM_PROP_FILE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, -1},
		{0,              0, TBSTATE_ENABLED, TBSTYLE_SEP,    0, 0, 0, -1},
		{4,    IDM_RECEIVE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, -1},
		{3,       IDM_SEND, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, -1},
		{5,    IDM_EXECUTE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, -1},
	} ;

	m_hWndCB = CommandBar_Create( hInst, m_hWnd, 0 ) ;
	CommandBar_InsertMenubar( m_hWndCB, hInst, IDR_MAIN_MENU, 0 ) ;
    CommandBar_AddBitmap( m_hWndCB, hInst, IDR_TOOLBAR1, 8, 0, 0 ) ;
    CommandBar_AddButtons( m_hWndCB, sizeof(tbButton) / sizeof(*tbButton), tbButton ) ;
	CommandBar_AddAdornments( m_hWndCB, 0, 0 ) ;
	return CommandBar_Height( m_hWndCB ) ;
#else	/* _WIN32_WCE */
	return 0 ;
#endif	/* _WIN32_WCE */
}

BOOL
MainWindow::RegisterClass( HINSTANCE hInst )
{
	WNDCLASS	wc ;

	wc.style         = NULL ;
	wc.lpfnWndProc   = (WNDPROC) WndProc ;
	wc.cbClsExtra    = 0 ;
	wc.cbWndExtra    = 4 ;
	wc.hInstance     = hInst ;
	wc.hIcon         = NULL ;
	wc.hCursor       = NULL ;
	wc.hbrBackground = (HBRUSH) GetStockObject( WHITE_BRUSH ) ;
#ifdef	_WIN32_WCE
	wc.lpszMenuName  = 0 ;
#else	/* _WIN32_WCE */
	wc.lpszMenuName  = MAKEINTRESOURCE(IDR_MAIN_MENU) ;
#endif	/* _WIN32_WCE */
	wc.lpszClassName = 	GetClassName() ;
	return ::RegisterClass( &wc ) ;
}
