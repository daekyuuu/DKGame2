// ClientLobby.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Client.h"
#include "ClientBattle.h"

// CClientBattle

IMPLEMENT_DYNCREATE(CClientBattle, CClientBase)

CClientBattle::CClientBattle()
	: CClientBase(IDD_CLIENT_BATTLE)
{

}

CClientBattle::~CClientBattle()
{
}

void CClientBattle::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CClientBattle, CFormView)
	ON_BN_CLICKED(IDC_BATTLE_LEAVE, &CClientBattle::OnBnClickedBattleLeave)
END_MESSAGE_MAP()


// CClientBattle �����Դϴ�.

#ifdef _DEBUG
void CClientBattle::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CClientBattle::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CClientBattle �޽��� ó�����Դϴ�.
void CClientBattle::OnInitialUpdate()
{
	CClientBase::OnInitialUpdate();

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
}

void CClientBattle::OnBnClickedBattleLeave()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	PacketBattleEndReq Packet;
	m_pTCP->SendPacket( &Packet );
}
