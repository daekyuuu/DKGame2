// ClientLogin.cpp : ���� �����Դϴ�.
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


// CClientLogin �����Դϴ�.

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


// CClientLogin �޽��� ó�����Դϴ�.
void CClientLogin::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
}

void CClientLogin::OnBnClickedLoginLogin()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData( TRUE );

	PacketLoginReq Packet;
	Packet.m_strID = m_strID.GetBuffer();
	Packet.m_strPW = m_strPW.GetBuffer();
	m_pTCP->SendPacket( &Packet );
}
