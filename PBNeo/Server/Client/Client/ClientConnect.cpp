// ClientConnect.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Client.h"
#include "ClientConnect.h"


// CClientConnect

IMPLEMENT_DYNCREATE(CClientConnect, CClientBase)

CClientConnect::CClientConnect()
	: CClientBase(IDD_CLIENT_CONNECT)
	, m_ui32Port(36100)
{

}

CClientConnect::~CClientConnect()
{
}

void CClientConnect::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CONNECT_IP, m_ctIP);
	DDX_Text(pDX, IDC_CONNECT_PORT, m_ui32Port);
	DDV_MinMaxUInt(pDX, m_ui32Port, 0, 65535);
}

BEGIN_MESSAGE_MAP(CClientConnect, CFormView)
	ON_BN_CLICKED(IDC_CONNECT_CONNECT, &CClientConnect::OnBnClickedConnectConnect)
END_MESSAGE_MAP()


// CClientConnect 진단입니다.

#ifdef _DEBUG
void CClientConnect::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CClientConnect::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CClientConnect 메시지 처리기입니다.
void CClientConnect::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	m_ctIP.SetAddress( 127, 0, 0, 1 );
}

void CClientConnect::OnBnClickedConnectConnect()
{
	UpdateData( TRUE );

	BYTE bIP[ 4 ] ;
	m_ctIP.GetAddress( bIP[0], bIP[1], bIP[2], bIP[3] );
	UINT32 ui32IP = bIP[0] | (bIP[1] << 8) | (bIP[2] << 16) | (bIP[3] << 24);

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CClientApp *pApp = (CClientApp *)AfxGetApp();

	pApp->ConnectServerReq( ui32IP, (UINT16)m_ui32Port );

	//pApp->SwitchView( VIEW_PAGE_LOGIN );
}