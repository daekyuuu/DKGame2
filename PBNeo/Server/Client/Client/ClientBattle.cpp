// ClientLobby.cpp : 구현 파일입니다.
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


// CClientBattle 진단입니다.

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


// CClientBattle 메시지 처리기입니다.
void CClientBattle::OnInitialUpdate()
{
	CClientBase::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
}

void CClientBattle::OnBnClickedBattleLeave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	PacketBattleEndReq Packet;
	m_pTCP->SendPacket( &Packet );
}
