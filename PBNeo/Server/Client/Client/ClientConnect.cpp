// ClientConnect.cpp : ���� �����Դϴ�.
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


// CClientConnect �����Դϴ�.

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


// CClientConnect �޽��� ó�����Դϴ�.
void CClientConnect::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	m_ctIP.SetAddress( 127, 0, 0, 1 );
}

void CClientConnect::OnBnClickedConnectConnect()
{
	UpdateData( TRUE );

	BYTE bIP[ 4 ] ;
	m_ctIP.GetAddress( bIP[0], bIP[1], bIP[2], bIP[3] );
	UINT32 ui32IP = bIP[0] | (bIP[1] << 8) | (bIP[2] << 16) | (bIP[3] << 24);

	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CClientApp *pApp = (CClientApp *)AfxGetApp();

	pApp->ConnectServerReq( ui32IP, (UINT16)m_ui32Port );

	//pApp->SwitchView( VIEW_PAGE_LOGIN );
}