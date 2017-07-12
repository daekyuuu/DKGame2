#include "stdafx.h"
#include "MainFrm.h"
#include "ConnectorCenter.h"

void CMainFrm::_GetUserInfo( RING_DB_POP* pPop )
{
	PacketUserInfoLoadAck Packet;
	Packet.SetSessionID( pPop->m_i32SessionIdx );
	Packet.SetUID( pPop->m_TUID );

	Packet.m_TResult				= pPop->m_TResult;	
	pPop->m_Base.CopyTo( Packet.m_Base );
	pPop->m_Info.CopyTo( Packet.m_Info );
	pPop->m_RecordTotal.CopyTo( Packet.m_RecordTotal );
	pPop->m_RecordSeason.CopyTo( Packet.m_RecordSeason );
	pPop->m_Eequipment.CopyTo( Packet.m_Eequipment );
	
	m_pServerCenter->SendPacket( &Packet );
}