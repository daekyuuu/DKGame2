// ClientLogin.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Client.h"
#include "ClientLogin.h"


// CClientLogin

IMPLEMENT_DYNCREATE(CClientLogin, CClientBase)

CClientLogin::CClientLogin()
	: CClientBase(IDD_CLIENT_LOGIN)
	, m_strID(_T(""))
	, m_strPW(_T(""))
{

}

CClientLogin::~CClientLogin()
{
}

void CClientLogin::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_LOGIN_ID, m_strID);
	DDX_Text(pDX, IDC_LOGIN_PW, m_strPW);
}

BEGIN_MESSAGE_MAP(CClientLogin, CFormView)
	ON_BN_CLICKED(IDC_LOGIN_LOGIN, &CClientLogin::OnBnClickedLoginLogin)
END_MESSAGE_MAP()


// CClientLogin 진단입니다.

#ifdef _DEBUG
void CClientLogin::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CClientLogin::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CClientLogin 메시지 처리기입니다.
void CClientLogin::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
}

void CClientLogin::OnBnClickedLoginLogin()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData( TRUE );

	PacketLoginReq Packet;
	Packet.m_strID = m_strID.GetBuffer();
	Packet.m_strPW = m_strPW.GetBuffer();
	m_pTCP->SendPacket( &Packet );
}
