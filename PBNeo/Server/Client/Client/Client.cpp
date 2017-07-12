
// Client.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "Client.h"
#include "MainFrm.h"

#include "ClientDoc.h"
#include "ClientConnect.h"
#include "ClientLogin.h"
#include "ClientLobby.h"
#include "ClientRoom.h"
#include "ClientBattle.h"
#include "ClientLoadout.h"

#include <AFXPRIV.H>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CClientApp

BEGIN_MESSAGE_MAP(CClientApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CClientApp::OnAppAbout)
	// ǥ�� ������ ���ʷ� �ϴ� ���� ����Դϴ�.
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
END_MESSAGE_MAP()


// CClientApp ����

CClientApp::CClientApp()
{
	m_bHiColorIcons = TRUE;

	// �ٽ� ���� ������ ����
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// ���� ���α׷��� ���� ��� ��Ÿ�� ������ ����Ͽ� ������ ���(/clr):
	//     1) �� �߰� ������ �ٽ� ���� ������ ������ ����� �۵��ϴ� �� �ʿ��մϴ�.
	//     2) ������Ʈ���� �����Ϸ��� System.Windows.Forms�� ���� ������ �߰��ؾ� �մϴ�.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: �Ʒ� ���� ���α׷� ID ���ڿ��� ���� ID ���ڿ��� �ٲٽʽÿ�(����).
	// ���ڿ��� ���� ����: CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("Client.AppID.NoVersion"));

	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	m_pTCP = NULL;
	// InitInstance�� ��� �߿��� �ʱ�ȭ �۾��� ��ġ�մϴ�.
}

CClientApp::~CClientApp()
{
	SAFE_DELETE( m_pTCP );
}
// ������ CClientApp ��ü�Դϴ�.

CClientApp theApp;


// CClientApp �ʱ�ȭ

BOOL CClientApp::InitInstance()
{
	// ���� ���α׷� �Ŵ��佺Ʈ�� ComCtl32.dll ���� 6 �̻��� ����Ͽ� ���־� ��Ÿ����
	// ����ϵ��� �����ϴ� ���, Windows XP �󿡼� �ݵ�� InitCommonControlsEx()�� �ʿ��մϴ�. 
	// InitCommonControlsEx()�� ������� ������ â�� ���� �� �����ϴ�.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ���� ���α׷����� ����� ��� ���� ��Ʈ�� Ŭ������ �����ϵ���
	// �� �׸��� �����Ͻʽÿ�.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();


	// OLE ���̺귯���� �ʱ�ȭ�մϴ�.
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// RichEdit ��Ʈ���� ����Ϸ���  AfxInitRichEdit2()�� �־�� �մϴ�.	
	// AfxInitRichEdit2();

	// ǥ�� �ʱ�ȭ
	// �̵� ����� ������� �ʰ� ���� ���� ������ ũ�⸦ ���̷���
	// �Ʒ����� �ʿ� ���� Ư�� �ʱ�ȭ
	// ��ƾ�� �����ؾ� �մϴ�.
	// �ش� ������ ����� ������Ʈ�� Ű�� �����Ͻʽÿ�.
	// TODO: �� ���ڿ��� ȸ�� �Ǵ� ������ �̸��� ����
	// ������ �������� �����ؾ� �մϴ�.
	SetRegistryKey(_T("���� ���� ���α׷� �����翡�� ������ ���� ���α׷�"));
	LoadStdProfileSettings(4);  // MRU�� �����Ͽ� ǥ�� INI ���� �ɼ��� �ε��մϴ�.


	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// ���� ���α׷��� ���� ���ø��� ����մϴ�.  ���� ���ø���
	//  ����, ������ â �� �� ������ ���� ������ �մϴ�.
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CClientDoc),
		RUNTIME_CLASS(CMainFrame),       // �� SDI ������ â�Դϴ�.
		RUNTIME_CLASS(CClientConnect));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// ǥ�� �� ���, DDE, ���� ���⿡ ���� ������� ���� �м��մϴ�.
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// ����ٿ� ������ ����� ����ġ�մϴ�.
	// ���� ���α׷��� /RegServer, /Register, /Unregserver �Ǵ� /Unregister�� ���۵� ��� FALSE�� ��ȯ�մϴ�.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	m_pTCP = new CNetworkTCP ( this );
	if( NULL == m_pTCP )
	{
		return FALSE;
	}

	if( FALSE == _CreatePage() )
	{
		return FALSE;
	}

	SwitchView( VIEW_PAGE_CONNECT );
	
	// â �ϳ��� �ʱ�ȭ�Ǿ����Ƿ� �̸� ǥ���ϰ� ������Ʈ�մϴ�.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	return TRUE;
}

int CClientApp::ExitInstance()
{
	//TODO: �߰��� �߰� ���ҽ��� ó���մϴ�.
	AfxOleTerm(FALSE);

	return CWinAppEx::ExitInstance();
}

BOOL CClientApp::_CreatePage()
{
	CDocument* pCurrentDoc = ((CFrameWnd*)m_pMainWnd)->GetActiveDocument();

	CCreateContext newContext;
	newContext.m_pNewViewClass		= NULL;
	newContext.m_pNewDocTemplate	= NULL;
	newContext.m_pLastView			= NULL;
	newContext.m_pCurrentFrame		= NULL;
	newContext.m_pCurrentDoc		= pCurrentDoc;

	CRect	rcRect( 0, 0, 0, 0);
	UINT32	ui32ViewID				= AFX_IDW_PANE_FIRST;

	for( INT32 i = 0 ; i < VIEW_PAGE_COUNT ; i++ )
	{
		m_pPage[ i ] = _GetPage( i );
		if( NULL == m_pPage[ i ] )		return FALSE;
		m_pPage[ i ]->SetTCP( m_pTCP );

		ui32ViewID++;
		((CView*)m_pPage[ i ])->Create( NULL, _T("AnyWindowName"), WS_CHILD, rcRect, m_pMainWnd, ui32ViewID, &newContext );
		m_pPage[ i ]->SendMessage( WM_INITIALUPDATE, 0, 0 );
	}
	
	return TRUE;
}

CClientBase* CClientApp::_GetPage( INT32 i32Number )
{
	switch( i32Number )
	{
	case VIEW_PAGE_CONNECT:		return (CClientBase*)(new CClientConnect);
	case VIEW_PAGE_LOGIN:		return (CClientBase*)(new CClientLogin);
	case VIEW_PAGE_LOBBY:		return (CClientBase*)(new CClientLobby);
	case VIEW_PAGE_ROOM:		return (CClientBase*)(new CClientRoom);
	case VIEW_PAGE_BATTLE:		return (CClientBase*)(new CClientBattle);
	case VIEW_PAGE_LOADOUT:		return (CClientBase*)(new CClientLoadout);
	}
	
	return NULL;
}

CView* CClientApp::SwitchView( VIEW_PAGE ePage )
{
	if( VIEW_PAGE_COUNT <= ePage )	return NULL;

	CView* pActiveView = ((CFrameWnd*)m_pMainWnd)->GetActiveView();

	CView* pNewView = m_pPage[ ePage ];

	// Exchange view window IDs so RecalcLayout() works.
#ifndef _WIN32
	UINT32 ui32Temp = ::GetWindowWord(pActiveView->m_hWnd, GWW_ID );
	::SetWindowWord( pActiveView->m_hWnd, GWW_ID, ::GetWindowWord(pNewView->m_hWnd, GWW_ID) );
	::SetWindowWord( pNewView->m_hWnd, GWW_ID, ui32Temp );
#else
	UINT32 ui32Temp = ::GetWindowLong( pActiveView->m_hWnd, GWL_ID );
	::SetWindowLong( pActiveView->m_hWnd, GWL_ID, ::GetWindowLong(pNewView->m_hWnd, GWL_ID) );
	::SetWindowLong( pNewView->m_hWnd, GWL_ID, ui32Temp );
#endif

	pActiveView->ShowWindow(SW_HIDE);
	pNewView->ShowWindow(SW_SHOW);
	((CFrameWnd*)m_pMainWnd)->SetActiveView(pNewView);
	((CFrameWnd*)m_pMainWnd)->RecalcLayout();
	pNewView->Invalidate();
	return pActiveView;
} 
// CClientApp �޽��� ó����


// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// ��ȭ ���ڸ� �����ϱ� ���� ���� ���α׷� ����Դϴ�.
void CClientApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CClientApp ����� ���� �ε�/���� �޼���

void CClientApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
}

void CClientApp::LoadCustomState()
{
}

void CClientApp::SaveCustomState()
{
}

// CClientApp �޽��� ó����


BOOL CClientApp::OnIdle(LONG lCount)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	m_pTCP->SelectEvent();

	return 1;//CWinAppEx::OnIdle(lCount);
}

BOOL CClientApp::ConnectServerReq( UINT32 ui32IP, UINT16 ui16Port )
{
	if( FALSE == m_pTCP->Create( ui32IP, ui16Port ) )
	{
		return FALSE;
	}

	return TRUE;
}

void CClientApp::ConnectServerAck( char* pBuffer )
{
	if( FALSE == SwitchView( VIEW_PAGE_LOGIN ) )
	{		
	}
}

void CClientApp::LoginAck( char* pBuffer )
{
	PacketLoginAck Packet;
	Packet.UnPacking_C( pBuffer );

	if( S2MO_IS_SUCCESS( Packet.m_TResult ) )
	{
		SwitchView( VIEW_PAGE_LOBBY );
	}
}

void CClientApp::UserInfoLoadAck( char* pBuffer )
{
	PacketUserInfoLoadAck Packet;
	Packet.UnPacking_C( pBuffer );

	if( S2MO_IS_SUCCESS( Packet.m_TResult ) )
	{
		CClientLobby* pPage = (CClientLobby*)m_pPage[ VIEW_PAGE_LOBBY ];
		pPage->SetUserInfo( &Packet );
	}
}

void CClientApp::UserChangeNickAck( char* pBuffer )
{
	PacketUserChangeNickAck Packet;
	Packet.UnPacking_C( pBuffer );

	if( S2MO_IS_SUCCESS( Packet.m_TResult ) )
	{
		CClientLobby* pPage = (CClientLobby*)m_pPage[ VIEW_PAGE_LOBBY ];
		pPage->SetUserNick( &Packet );
	}
}

void CClientApp::RoomCreateAck( char* pBuffer )
{
	PacketRoomCreateAck Packet;
	Packet.UnPacking_C( pBuffer );
}

void CClientApp::RoomEnterAck( char* pBuffer )
{
	PacketRoomEnterAck Packet;
	Packet.UnPacking_C( pBuffer );

	if( S2MO_IS_SUCCESS( Packet.m_TResult ) )
	{
		SwitchView( VIEW_PAGE_ROOM );

		CClientRoom* pPage = (CClientRoom*)m_pPage[ VIEW_PAGE_ROOM ];
		pPage->EnterUser( &Packet );
	}
}

void CClientApp::RoomLeaveAck( char* pBuffer )
{
	PacketRoomLeaveAck Packet;
	Packet.UnPacking_C( pBuffer );

	if( S2MO_IS_SUCCESS( Packet.m_TResult ) )
	{
		SwitchView( VIEW_PAGE_LOBBY );
	}
}

void CClientApp::RoomSlotInfoAck( char* pBuffer )
{
	PacketRoomSlotInfoAck Packet;
	Packet.UnPacking_C( pBuffer );

	CClientRoom* pPage = (CClientRoom*)m_pPage[ VIEW_PAGE_ROOM ];
	pPage->SetRoomSlotInfo( &Packet );
}

void CClientApp::RoomEnterTransAck( char* pBuffer )
{
	PacketRoomEnterTransAck Packet;
	Packet.UnPacking_C( pBuffer );

	CClientRoom* pPage = (CClientRoom*)m_pPage[ VIEW_PAGE_ROOM ];
	pPage->EnterTransUser( &Packet );
}

void CClientApp::RoomLeaveTransAck( char* pBuffer )
{
	PacketRoomLeaveTransAck Packet;
	Packet.UnPacking_C( pBuffer );

	CClientRoom* pPage = (CClientRoom*)m_pPage[ VIEW_PAGE_ROOM ];
	pPage->LeaveTransUser( &Packet );
}

void CClientApp::RoomMapVoteAck( char* pBuffer )
{
	PacketRoomMapVoteTransAck Packet;
	Packet.UnPacking_C( pBuffer );

	CClientRoom* pPage = (CClientRoom*)m_pPage[ VIEW_PAGE_ROOM ];
	pPage->RoomMapVote( &Packet );
}

void CClientApp::RoomSlotStateChangeAck( char* pBuffer )
{
	PacketRoomSlotStateChangeAck Packet;
	Packet.UnPacking_C( pBuffer );

	CClientRoom* pPage = (CClientRoom*)m_pPage[ VIEW_PAGE_ROOM ];
	pPage->RoomSlotStateChange( &Packet );
}

void CClientApp::RoomStateChangeAck( char* pBuffer )
{
	PacketRoomStateChangeAck Packet;
	Packet.UnPacking_C( pBuffer );

	CClientRoom* pPage = (CClientRoom*)m_pPage[ VIEW_PAGE_ROOM ];
	pPage->RoomStateChange( &Packet );
}

void CClientApp::RoomBattleStartAck( char* pBuffer )
{
	PacketBattleStartAck Packet;
	Packet.UnPacking_C( pBuffer );

	SwitchView( VIEW_PAGE_BATTLE );
}

void CClientApp::RoomBattleEndAck( char* pBuffer )
{
	PacketBattleEndAck Packet;
	Packet.UnPacking_C( pBuffer );

	if( S2MO_IS_SUCCESS( Packet.m_TResult ) )
	{
		SwitchView( VIEW_PAGE_ROOM );
	}
}

void CClientApp::RoomList( char* pBuffer )
{
	PacketRoomListAck Packet;
	Packet.UnPacking_C( pBuffer );

	CClientLobby* pPage = (CClientLobby*)m_pPage[ VIEW_PAGE_LOBBY ];
	pPage->RoomList( &Packet );
}
