
// ServerDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "ServerApp.h"
#include "ServerDlg.h"
#include "afxdialogex.h"

#include "MainFrm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CServerDlg*			g_pDlg;

// CServerDlg 대화 상자

CServerDlg::CServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SERVER_DIALOG, pParent)
	, m_strNotice(_T(""))
{
	g_pDlg = this;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LT_USERLIST, m_ctUserList);
	DDX_Control(pDX, IDC_LT_ROOMLIST, m_ctRoomList);
	DDX_Control(pDX, IDC_TR_SERVERLIST, m_ctServerList);
	DDX_Text(pDX, IDC_ED_NOTICE, m_strNotice);
}

BEGIN_MESSAGE_MAP(CServerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_MESSAGE(WM_USER_MESSAGE, &CServerDlg::OnMessage)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BT_NOTICE, &CServerDlg::OnBnClickedBtNotice)
	ON_BN_CLICKED(IDCANCEL, &CServerDlg::OnBnClickedCancel)
END_MESSAGE_MAP()

// CServerDlg 메시지 처리기

BOOL CServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.	
	LV_COLUMN lvColumn;

	// Setup User List
	m_ctUserList.SetExtendedStyle( LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT );	
	lvColumn.mask		= LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lvColumn.fmt		= LVCFMT_CENTER;

	lvColumn.pszText	= _T("UID");
	lvColumn.iSubItem	= 0;
	lvColumn.cx			= 50;
	m_ctUserList.InsertColumn( 0, &lvColumn );

	lvColumn.pszText	= _T("Nick");
	lvColumn.iSubItem	= 0;
	lvColumn.cx			= 150;
	m_ctUserList.InsertColumn( 1, &lvColumn );

	lvColumn.pszText	= _T("Level");
	lvColumn.iSubItem	= 1;
	lvColumn.cx			= 50;
	m_ctUserList.InsertColumn( 2, &lvColumn );

	lvColumn.pszText	= _T("State");
	lvColumn.iSubItem	= 2;
	lvColumn.cx			= 70;
	m_ctUserList.InsertColumn( 3, &lvColumn );

	// Setup Room List
	m_ctRoomList.SetExtendedStyle( LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT );

	lvColumn.pszText	= _T("Idx");
	lvColumn.iSubItem	= 0;
	lvColumn.cx			= 50;
	m_ctRoomList.InsertColumn( 0, &lvColumn );

	lvColumn.pszText	= _T("Name");
	lvColumn.iSubItem	= 1;
	lvColumn.cx			= 150;
	m_ctRoomList.InsertColumn( 1, &lvColumn );

	lvColumn.pszText	= _T("User");
	lvColumn.iSubItem	= 2;
	lvColumn.cx			= 50;
	m_ctRoomList.InsertColumn( 2, &lvColumn );

	lvColumn.pszText	= _T("State");
	lvColumn.iSubItem	= 3;
	lvColumn.cx			= 80;
	m_ctRoomList.InsertColumn( 3, &lvColumn );

	lvColumn.pszText	= _T("Time");
	lvColumn.iSubItem	= 4;
	lvColumn.cx			= 50;
	m_ctRoomList.InsertColumn( 4, &lvColumn );	

	TVINSERTSTRUCT tvi; 
	tvi.hParent         = TVI_ROOT; // 최상위
	tvi.hInsertAfter    = TVI_LAST;
	tvi.item.mask       = TVIF_TEXT | TVIF_PARAM | TVIF_CHILDREN;
	
	m_ctServerList.ModifyStyle(NULL , TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS);

	tvi.item.pszText    = _T("Auth");
	tvi.item.lParam		= SERVER_TYPE_AUTH;
	m_ctServerList.InsertItem( &tvi );
	tvi.item.pszText    = _T("DB");
	tvi.item.lParam		= SERVER_TYPE_DB;
	m_ctServerList.InsertItem( &tvi );
	tvi.item.pszText    = _T("Front");
	tvi.item.lParam		= SERVER_TYPE_FRONT;
	m_ctServerList.InsertItem( &tvi );

	m_pServer.StartServer( _T("Dump") );
	m_pServer.SetInit();

	SetTimer( 1, 1000, NULL );

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CServerDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_pServer.StopServer();
}

LRESULT CServerDlg::OnMessage( WPARAM wParam, LPARAM lParam )
{
	CString strTemp;
	switch( (DLG_MESSAGE)wParam )
	{
	case DLG_MESSAGE_SERVER_INSERT:
		{
			_InsertServer( (SERVER_TYPE_INFO*)lParam );
		}
		break;
	case DLG_MESSAGE_SERVER_DESTROY:
		{
			_DestroyServer( (SERVER_TYPE_INFO*)lParam );
		}
		break;
	case DLG_MESSAGE_USER_INSERT:
		{
			_UserInsert( (USER_LIST*)lParam );
		}
		break;
	case DLG_MESSAGE_USER_DELETE:
		{
			_UserDelete( (USER_LIST*)lParam );
		}
		break;
	case DLG_MESSAGE_USER_UPDATE:
		{
			_UserUpdate( (USER_LIST*)lParam );			
		}
		break;
	case DLG_MESSAGE_ROOM_CREATE:
		{
			_RoomCreate( (CRoom*)lParam );
		}
		break;
	case DLG_MESSAGE_ROOM_DESTROY:
		{
			_RoomDestroy( (CRoom*)lParam );
		}
		break;
	case DLG_MESSAGE_ROOM_UPDATE:
		{
			_RoomUpdate( (CRoom*)lParam );
		}
		break;
	}
	
	return (LRESULT)1;
}

void CServerDlg::_InsertServer( SERVER_TYPE_INFO* pInfo )
{
	TVINSERTSTRUCT tvi; 
	tvi.hInsertAfter    = TVI_LAST;
	tvi.item.mask       = TVIF_TEXT | TVIF_PARAM;

	HTREEITEM hTreeItem;

	hTreeItem = m_ctServerList.GetRootItem();

	CString strTemp;
	DWORD lParam;
	
	while( hTreeItem )
	{
		lParam = m_ctServerList.GetItemData( hTreeItem );
		if( pInfo->m_eType == lParam )
		{
			strTemp.Format( _T("%03d"), pInfo->m_ui32Idx+1 );

			tvi.hParent			= hTreeItem;
			tvi.item.pszText    = strTemp.GetBuffer();
			tvi.item.lParam		= ((pInfo->m_ui32Idx+1) << 16) | pInfo->m_eType;
			m_ctServerList.InsertItem( &tvi );
			
			break;
		}
		else
		{
			hTreeItem = m_ctServerList.GetNextItem( hTreeItem, TVGN_NEXT );
		}
	}
	m_ctServerList.Expand( hTreeItem, TVE_EXPAND );
}

void CServerDlg::_DestroyServer( SERVER_TYPE_INFO* pInfo )
{
	TVINSERTSTRUCT tvi; 
	tvi.hInsertAfter    = TVI_LAST;
	tvi.item.mask       = TVIF_TEXT | TVIF_PARAM;

	HTREEITEM hTreeItem;

	hTreeItem = m_ctServerList.GetRootItem();

	CString strTemp;
	DWORD lParam;
	DWORD dwParam = ((pInfo->m_ui32Idx+1) << 16) | pInfo->m_eType;
	
	while( hTreeItem )
	{
		lParam = m_ctServerList.GetItemData( hTreeItem );
		if( pInfo->m_eType == lParam )
		{
			HTREEITEM hChildItem;
			hChildItem = m_ctServerList.GetChildItem( hTreeItem );
			while( hChildItem )
			{
				lParam = m_ctServerList.GetItemData( hChildItem );
				if( dwParam == lParam )
				{
					m_ctServerList.DeleteItem( hChildItem );

					hTreeItem	= NULL;
					hChildItem	= NULL;
				}
				else
				{
					hChildItem = m_ctServerList.GetNextItem( hChildItem, TVGN_NEXT );
				}
			}
		}
		else
		{
			hTreeItem = m_ctServerList.GetNextItem( hTreeItem, TVGN_NEXT );
		}
	}
}

TCHAR* CServerDlg::_GetUserState( USER_STATE eState )
{
	switch( eState )
	{
	case USER_STATE_NONE:		return _T("None");
	case USER_STATE_LOGIN:		return _T("Login");
	case USER_STATE_WAITING:	return _T("Waiting");
	case USER_STATE_ROOM:		return _T("Room");
	case USER_STATE_BATTLE:		return _T("Battle");
	}
	return _T("None");
}

void CServerDlg::_UserInsert( USER_LIST* pUser )
{
	CString strTemp;
	LPARAM Param = ((pUser->m_TUID % USER_NODE_COUNT) * 10000000) + pUser->m_ui32Idx;
	strTemp.Format( _T("%I64d"), pUser->m_TUID );
	INT32 i32Idx = m_ctUserList.InsertItem( LVIF_TEXT|LVIF_PARAM, 0, strTemp, 0, 0, 0, Param );			
	m_ctUserList.SetItem( i32Idx, 1, LVIF_TEXT, _T(""), 0, 0, 0, 0 );
	m_ctUserList.SetItem( i32Idx, 2, LVIF_TEXT, _T(""), 0, 0, 0, 0 );
	m_ctUserList.SetItem( i32Idx, 3, LVIF_TEXT, _GetUserState(pUser->m_eUserState), 0, 0, 0, 0 );
}

void CServerDlg::_UserDelete( USER_LIST* pUser )
{
	DWORD dwUserData = ((pUser->m_TUID % USER_NODE_COUNT) * 10000000) + pUser->m_ui32Idx;
	for( INT32 i = 0 ; i < m_ctUserList.GetItemCount() ; i++ )
	{
		if( dwUserData == m_ctUserList.GetItemData(i) )
		{
			m_ctUserList.DeleteItem(i);
			break;
		}
	}
}

void CServerDlg::_UserUpdate(USER_LIST* pUser)
{
	CString strTemp;
	DWORD dwUserData = ((pUser->m_TUID % USER_NODE_COUNT) * 10000000) + pUser->m_ui32Idx;
	for( INT32 i = 0 ; i < m_ctUserList.GetItemCount() ; i++ )
	{
		if( dwUserData == m_ctUserList.GetItemData(i) )
		{
			m_ctUserList.SetItem(i, 1, LVIF_TEXT, pUser->m_UserBase.m_strNickname, 0, 0, 0, 0);
			strTemp.Format(_T("%d"), pUser->m_UserBase.m_ui16Level);
			m_ctUserList.SetItem(i, 2, LVIF_TEXT, strTemp, 0, 0, 0, 0);
			m_ctUserList.SetItem(i, 3, LVIF_TEXT, _GetUserState(pUser->m_eUserState), 0, 0, 0, 0);

			break;
		}
	}
}

TCHAR* CServerDlg::_GetRoomState( NET_ROOM_STATE eState )
{
	switch( eState )
	{
	case NET_ROOM_STATE_CLOSE:		return _T("Close");
	case NET_ROOM_STATE_OPEN:		return _T("Waiting");
	case NET_ROOM_STATE_READY:		return _T("Ready");
	case NET_ROOM_STATE_COUNTDOWN:	return _T("CountDown");
	case NET_ROOM_STATE_PLAYING:	return _T("Playing");
	}
	return _T("None");
}

void CServerDlg::_RoomCreate( CRoom* pRoom )
{
	CString strTemp;
	
	strTemp.Format( _T("%03d"), pRoom->GetIdx() );
	INT32 i32Idx = m_ctRoomList.InsertItem( LVIF_TEXT|LVIF_PARAM, 0, strTemp, 0, 0, 0, pRoom->GetIdx() );			
	m_ctRoomList.SetItem( i32Idx, 1, LVIF_TEXT, pRoom->GetRoomInfo()->m_strTitle, 0, 0, 0, 0 );
	strTemp.Format( _T("%02d/%02d"), pRoom->GetRoomInfo()->m_i8MemberCount, pRoom->GetRoomInfo()->m_i8MemberMaxCount );	
	m_ctRoomList.SetItem( i32Idx, 2, LVIF_TEXT, strTemp, 0, 0, 0, 0 );
	m_ctRoomList.SetItem( i32Idx, 3, LVIF_TEXT, _GetRoomState( pRoom->GetRoomState() ), 0, 0, 0, 0 );
	strTemp.Format( _T("%02d"), pRoom->GetReminTime() );
	m_ctRoomList.SetItem( i32Idx, 4, LVIF_TEXT, strTemp, 0, 0, 0, 0 );
}

void CServerDlg::_RoomDestroy( CRoom* pRoom )
{
	for( INT32 i = 0; i < m_ctRoomList.GetItemCount(); i++ )
	{
		if( pRoom->GetIdx() == m_ctRoomList.GetItemData(i) )
		{
			m_ctRoomList.DeleteItem( i );
			break;
		}
	}
}

void CServerDlg::_RoomUpdate( CRoom* pRoom )
{
	for( INT32 i = 0; i < m_ctRoomList.GetItemCount(); i++ )
	{
		if( pRoom->GetIdx() == m_ctRoomList.GetItemData(i) )
		{
			CString strTemp;
			m_ctRoomList.SetItem( i, 1, LVIF_TEXT, pRoom->GetRoomInfo()->m_strTitle, 0, 0, 0, 0 );
			strTemp.Format( _T("%02d/%02d"), pRoom->GetRoomInfo()->m_i8MemberCount, pRoom->GetRoomInfo()->m_i8MemberMaxCount );	
			m_ctRoomList.SetItem( i, 2, LVIF_TEXT, strTemp, 0, 0, 0, 0 );
			m_ctRoomList.SetItem( i, 3, LVIF_TEXT, _GetRoomState( pRoom->GetRoomState() ), 0, 0, 0, 0 );
			strTemp.Format( _T("%02d"), pRoom->GetReminTime() );
			m_ctRoomList.SetItem( i, 4, LVIF_TEXT, strTemp, 0, 0, 0, 0 );
			break;
		}
	}
}


void CServerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CString strTemp;
	INT32 i32Time;
	for( INT32 i = 0; i < m_ctRoomList.GetItemCount(); i++ )
	{
		strTemp = m_ctRoomList.GetItemText( i, 3 );
		if( 0 == strTemp.Compare( _T("Playing") ) )	continue;

		strTemp = m_ctRoomList.GetItemText( i, 4 );
		i32Time = _wtoi( strTemp.GetBuffer() );
		i32Time--;
		strTemp.Format( _T("%02d"), i32Time );
		m_ctRoomList.SetItem( i, 4, LVIF_TEXT, strTemp, 0, 0, 0, 0 );
	}

	CDialogEx::OnTimer(nIDEvent);
}

void CServerDlg::OnBnClickedBtNotice()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData( TRUE );

	PacketNoticeAck Packet;
	Packet.m_i8Type		= NET_NOTICE_TYPE_NORMAL;
	Packet.m_strNotice	= m_strNotice.GetBuffer();
	m_pServer.GetMainFrm()->SendNotice( &Packet );

	m_strNotice = _T("");
	UpdateData( FALSE );
}


BOOL CServerDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if( pMsg->wParam == VK_RETURN )
	{
		OnBnClickedBtNotice();
		pMsg->wParam = NULL;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CServerDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( IDCANCEL == MessageBox( _T("정말 종료?"), _T("경고"), MB_OKCANCEL ) )
	{
		return;
	}

	CDialogEx::OnCancel();
}
