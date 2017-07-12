
// Client.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
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
	// 표준 파일을 기초로 하는 문서 명령입니다.
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
END_MESSAGE_MAP()


// CClientApp 생성

CClientApp::CClientApp()
{
	m_bHiColorIcons = TRUE;

	// 다시 시작 관리자 지원
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// 응용 프로그램을 공용 언어 런타임 지원을 사용하여 빌드한 경우(/clr):
	//     1) 이 추가 설정은 다시 시작 관리자 지원이 제대로 작동하는 데 필요합니다.
	//     2) 프로젝트에서 빌드하려면 System.Windows.Forms에 대한 참조를 추가해야 합니다.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: 아래 응용 프로그램 ID 문자열을 고유 ID 문자열로 바꾸십시오(권장).
	// 문자열에 대한 서식: CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("Client.AppID.NoVersion"));

	// TODO: 여기에 생성 코드를 추가합니다.
	m_pTCP = NULL;
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}

CClientApp::~CClientApp()
{
	SAFE_DELETE( m_pTCP );
}
// 유일한 CClientApp 개체입니다.

CClientApp theApp;


// CClientApp 초기화

BOOL CClientApp::InitInstance()
{
	// 응용 프로그램 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControlsEx()가 필요합니다. 
	// InitCommonControlsEx()를 사용하지 않으면 창을 만들 수 없습니다.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 응용 프로그램에서 사용할 모든 공용 컨트롤 클래스를 포함하도록
	// 이 항목을 설정하십시오.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();


	// OLE 라이브러리를 초기화합니다.
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// RichEdit 컨트롤을 사용하려면  AfxInitRichEdit2()가 있어야 합니다.	
	// AfxInitRichEdit2();

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화
	// 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(_T("로컬 응용 프로그램 마법사에서 생성된 응용 프로그램"));
	LoadStdProfileSettings(4);  // MRU를 포함하여 표준 INI 파일 옵션을 로드합니다.


	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// 응용 프로그램의 문서 템플릿을 등록합니다.  문서 템플릿은
	//  문서, 프레임 창 및 뷰 사이의 연결 역할을 합니다.
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CClientDoc),
		RUNTIME_CLASS(CMainFrame),       // 주 SDI 프레임 창입니다.
		RUNTIME_CLASS(CClientConnect));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// 표준 셸 명령, DDE, 파일 열기에 대한 명령줄을 구문 분석합니다.
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// 명령줄에 지정된 명령을 디스패치합니다.
	// 응용 프로그램이 /RegServer, /Register, /Unregserver 또는 /Unregister로 시작된 경우 FALSE를 반환합니다.
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
	
	// 창 하나만 초기화되었으므로 이를 표시하고 업데이트합니다.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	return TRUE;
}

int CClientApp::ExitInstance()
{
	//TODO: 추가한 추가 리소스를 처리합니다.
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
// CClientApp 메시지 처리기


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
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

// 대화 상자를 실행하기 위한 응용 프로그램 명령입니다.
void CClientApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CClientApp 사용자 지정 로드/저장 메서드

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

// CClientApp 메시지 처리기


BOOL CClientApp::OnIdle(LONG lCount)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
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
