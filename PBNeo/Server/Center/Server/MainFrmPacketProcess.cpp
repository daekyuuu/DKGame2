#include "stdafx.h"
#include "MainFrm.h"

#include "RoomMgr.h"
#include "UserMgr.h"

T_RESULT CMainFrm::_LoginReq( RING_IOCP_POP* pRing )
{
	CSession* pSession = m_pSessionMgr->GetServerType( SERVER_TYPE_AUTH );
	if( NULL == pSession )	return NET_RESULT_ERROR;
		
	if( 0 == pSession->SendPacket( pRing->m_pBuffer ) )
	{
		return NET_RESULT_ERROR;
	}

	return NET_RESULT_SUCCESS;
}

T_RESULT CMainFrm::_LoginInputAck( RING_IOCP_POP* pRing )
{
	PacketLoginAck Recv;
	Recv.UnPacking_T( pRing->m_pBuffer, true );

	CSession* pSession = m_pSessionMgr->GetServerType( SERVER_TYPE_FRONT, 0 );
	if( NULL == pSession )	return NET_RESULT_ERROR;

	if( S2MO_IS_SUCCESS( Recv.m_TResult ) )
	{
		USER_LIST UserList;
		UserList.m_TUID					= Recv.GetUID();

		T_RESULT TResult;
		USER_LIST* pUser = m_pUserMgr->InsertUser( &UserList, TResult );
		if( pUser )
		{
			pUser->m_eUserState			= USER_STATE_LOGIN;
			pUser->m_i32SessionIdx		= Recv.GetSessionID();
			pUser->m_pFrontSession		= pSession;

			DLG_SENDMESSAGE( DLG_MESSAGE_USER_INSERT, (LPARAM)pUser );
		}
		else
		{
			Recv.m_TResult				= TResult;
		}
	}
	
	pSession->SendPacket( &Recv );

	return NET_RESULT_SUCCESS;
}

T_RESULT CMainFrm::_LogoutReq( RING_IOCP_POP* pRing )
{
	PacketLogoutReq Recv;
	Recv.UnPacking_T( pRing->m_pBuffer, true );

	T_UID TUID = Recv.GetUID();
	USER_LIST* pUser = m_pUserMgr->FindUser( TUID );
	if( pUser )
	{
		// 유저가 방에 있다면 방에서 나가줍니다.
		if( USER_STATE_ROOM <= pUser->m_eUserState )
		{
			m_pRoomMgr->RoomUserLeave( pUser );
		}

		// DB 저장을 위하여 데이터를 전달 합니다.
		PacketUserInfoSaveReq Packet;
		Packet.m_TUID			= pUser->m_TUID;
		pUser->m_UserBase.CopyTo( Packet.m_Base );
		pUser->m_UserInfo.CopyTo( Packet.m_Info );
		pUser->m_UserRecordSeason.CopyTo( Packet.m_Record );
		pUser->m_UserEquipment.CopyTo( Packet.m_Eequipment );
		CSession* pSession = m_pSessionMgr->GetServerType( SERVER_TYPE_DB, 0 );
		if( pSession )
		{
			pSession->SendPacket( &Packet );
		}

		m_pUserMgr->DeleteUser( pUser );
		DLG_SENDMESSAGE( DLG_MESSAGE_USER_DELETE, (LPARAM)pUser );
	}

	return NET_RESULT_SUCCESS;
}

T_RESULT CMainFrm::_UserInfoLoadReq( RING_IOCP_POP* pRing )
{
	CSession* pSession = m_pSessionMgr->GetServerType( SERVER_TYPE_DB, 0 );
	if( NULL == pSession )	return NET_RESULT_ERROR;

	pSession->SendPacket( pRing->m_pBuffer );

	return NET_RESULT_SUCCESS;
}

T_RESULT CMainFrm::_UserInfoLoadAck( RING_IOCP_POP* pRing )
{
	PacketUserInfoLoadAck Recv;
	Recv.UnPacking_T( pRing->m_pBuffer, true );

	T_UID TUID = Recv.GetUID();
	USER_LIST* pUser = m_pUserMgr->FindUser( TUID );
	if( pUser )
	{
		pUser->m_eUserState			= USER_STATE_WAITING;
		pUser->m_UserBase			= Recv.m_Base;
		pUser->m_UserInfo			= Recv.m_Info;
		//pUser->m_UserMoney		= Recv.m_Money;
		pUser->m_UserRecordTotal	= Recv.m_RecordTotal;
		pUser->m_UserRecordSeason	= Recv.m_RecordSeason;
		pUser->m_UserEquipment		= Recv.m_Eequipment;

		DLG_SENDMESSAGE( DLG_MESSAGE_USER_UPDATE, (LPARAM)pUser );
	}
	else
	{
		return NET_RESULT_ERROR;
	}

	CSession* pSession = m_pSessionMgr->GetServerType( SERVER_TYPE_FRONT, 0 );
	if( NULL == pSession )	return NET_RESULT_ERROR;
	
	pSession->SendPacket( &Recv );

	return NET_RESULT_SUCCESS;
}

T_RESULT CMainFrm::_UserChangeNick( RING_IOCP_POP* pRing )
{
	PacketUserChangeNickReq Recv;
	Recv.UnPacking_T( pRing->m_pBuffer, true );

	T_UID TUID = Recv.GetUID();
	USER_LIST* pUser = m_pUserMgr->FindUser( TUID );
	if( pUser )
	{
		Recv.m_strNickname.GetValue( pUser->m_UserBase.m_strNickname, NET_NICK_LENGTH );
	}
	else
	{
		return NET_RESULT_ERROR;
	}

	PacketUserChangeNickAck Send;
	Send.SetSessionID( Recv.GetSessionID() );
	Send.SetUID( Recv.GetUID() );
	Send.m_TResult			= S2_RESULT_SUCCESS;
	Send.m_strNickname		= pUser->m_UserBase.m_strNickname;

	CSession* pSession = m_pSessionMgr->GetServerType( SERVER_TYPE_FRONT, 0 );
	if( NULL == pSession )	return NET_RESULT_ERROR;
	
	pSession->SendPacket( &Send );

	return NET_RESULT_SUCCESS;
}

T_RESULT CMainFrm::_RoomCreateReq( RING_IOCP_POP* pRing )
{
	PacketRoomCreateReq Recv;
	Recv.UnPacking_T( pRing->m_pBuffer, true );
		
	T_UID TUID = Recv.GetUID();
	USER_LIST* pUser = m_pUserMgr->FindUser( TUID );
	if( NULL == pUser )
	{
		S2ASSERT(FALSE);
	}

	if( S2MO_IS_SUCCESS( m_pRoomMgr->RoomCreate( &Recv, pUser ) ) )
	{
		pUser->m_eUserState		= USER_STATE_ROOM;
		DLG_SENDMESSAGE( DLG_MESSAGE_USER_UPDATE, (LPARAM)pUser );
	}
	else
	{
		PacketRoomCreateAck Send;
		Send.m_TResult			= NET_RESULT_ERROR;
		Send.SetSessionID( Recv.GetSessionID() );
		Send.SetUID( Recv.GetUID() );
	
		pRing->m_pSession->SendPacket( &Send );
	}

	return NET_RESULT_SUCCESS;
}

T_RESULT CMainFrm::_RoomEnterReq( RING_IOCP_POP* pRing )
{
	PacketRoomEnterReq Recv;
	Recv.UnPacking_T( pRing->m_pBuffer, true );
		
	T_UID TUID = Recv.GetUID();
	USER_LIST* pUser = m_pUserMgr->FindUser( TUID );
	if( NULL == pUser )
	{
		S2ASSERT(FALSE);
	}	

	if( S2MO_IS_SUCCESS( m_pRoomMgr->RoomUserEnter( &Recv, pUser ) ) )
	{
		pUser->m_eUserState		= USER_STATE_ROOM;
		DLG_SENDMESSAGE( DLG_MESSAGE_USER_UPDATE, (LPARAM)pUser );
	}
	else
	{
		PacketRoomEnterAck Send;
		Send.m_TResult			= NET_RESULT_ERROR;
		Send.SetSessionID( Recv.GetSessionID() );
		Send.SetUID( Recv.GetUID() );
	
		pRing->m_pSession->SendPacket( &Send );
	}

	return NET_RESULT_SUCCESS;
}

T_RESULT CMainFrm::_RoomLeaveReq( RING_IOCP_POP* pRing )
{
	PacketRoomLeaveReq Recv;
	Recv.UnPacking_T( pRing->m_pBuffer, true );
		
	T_UID TUID = Recv.GetUID();
	USER_LIST* pUser = m_pUserMgr->FindUser( TUID );
	if( NULL == pUser )
	{
		S2ASSERT(FALSE);
	}

	if( S2MO_IS_SUCCESS( m_pRoomMgr->RoomUserLeave( pUser ) ) )
	{
		pUser->m_eUserState = USER_STATE_WAITING;
		DLG_SENDMESSAGE( DLG_MESSAGE_USER_UPDATE, (LPARAM)pUser );
	}
	else
	{
		PacketRoomEnterAck Send;
		Send.m_TResult			= NET_RESULT_ERROR;
		Send.SetSessionID( Recv.GetSessionID() );
		Send.SetUID( Recv.GetUID() );
	
		pRing->m_pSession->SendPacket( &Send );
	}

	return NET_RESULT_SUCCESS;
}

T_RESULT CMainFrm::_RoomQuickMatch( RING_IOCP_POP* pRing )
{
	PacketRoomQuickMatchReq Recv;
	Recv.UnPacking_T( pRing->m_pBuffer, true );

	T_UID TUID = Recv.GetUID();
	USER_LIST* pUser = m_pUserMgr->FindUser( TUID );
	if( NULL == pUser )
	{
		S2ASSERT(FALSE);
	}

	if( m_pRoomMgr->QuickMatch( &Recv, pUser ) )
	{
	}
	else
	{
		PacketRoomQuickMatchAck Send;
		Send.m_TResult			= NET_RESULT_ERROR;
		Send.SetSessionID( Recv.GetSessionID() );
		Send.SetUID( Recv.GetUID() );
	
		pRing->m_pSession->SendPacket( &Send );
	}

	return NET_RESULT_SUCCESS;
}

T_RESULT CMainFrm::_RoomMapVote( RING_IOCP_POP* pRing )
{
	PacketRoomMapVoteReq Recv;
	Recv.UnPacking_T( pRing->m_pBuffer, true );

	T_UID TUID = Recv.GetUID();
	USER_LIST* pUser = m_pUserMgr->FindUser( TUID );
	if( NULL == pUser )
	{
		S2ASSERT(FALSE);
	}

	if( m_pRoomMgr->MapVote( &Recv, pUser ) )
	{
	}
	else
	{
		PacketRoomMapVoteAck Send;
		Send.m_TResult			= NET_RESULT_ERROR;
		Send.SetSessionID( Recv.GetSessionID() );
		Send.SetUID( Recv.GetUID() );
	
		pRing->m_pSession->SendPacket( &Send );
	}

	return NET_RESULT_SUCCESS;
}

T_RESULT CMainFrm::_RoomSlotReady( RING_IOCP_POP* pRing )
{
	PacketRoomSlotReadyReq Recv;
	Recv.UnPacking_T( pRing->m_pBuffer, true );
		
	T_UID TUID = Recv.GetUID();
	USER_LIST* pUser = m_pUserMgr->FindUser( TUID );
	if( NULL == pUser )
	{
		S2ASSERT(FALSE);
	}

	if( S2MO_IS_SUCCESS( m_pRoomMgr->RoomUserReady( pUser ) ) )
	{
		DLG_SENDMESSAGE( DLG_MESSAGE_USER_UPDATE, (LPARAM)pUser );
	}
	else
	{
		PacketRoomSlotReadyAck Send;
		Send.m_TResult			= NET_RESULT_ERROR;
		Send.SetSessionID( Recv.GetSessionID() );
		Send.SetUID( Recv.GetUID() );
	
		pRing->m_pSession->SendPacket( &Send );
	}

	return NET_RESULT_SUCCESS;
}

T_RESULT CMainFrm::_RoomBattleReady( RING_IOCP_POP* pRing )
{
	PacketBattleReadyReq Recv;
	Recv.UnPacking_T( pRing->m_pBuffer, true );

	T_UID TUID = Recv.GetUID();
	USER_LIST* pUser = m_pUserMgr->FindUser( TUID );
	if( NULL == pUser )
	{
		S2ASSERT(FALSE);
	}

	if( S2MO_IS_SUCCESS( m_pRoomMgr->RoomBattleReady( pUser ) ) )
	{
		DLG_SENDMESSAGE( DLG_MESSAGE_USER_UPDATE, (LPARAM)pUser );
	}
	else
	{
		PacketBattleReadyAck Send;
		Send.m_TResult			= NET_RESULT_ERROR;
		Send.SetSessionID( Recv.GetSessionID() );
		Send.SetUID( Recv.GetUID() );
	
		pRing->m_pSession->SendPacket( &Send );
	}

	return NET_RESULT_SUCCESS;
}

T_RESULT CMainFrm::_RoomBattleEnd( RING_IOCP_POP* pRing )
{
	PacketBattleEndReq Recv;
	Recv.UnPacking_T( pRing->m_pBuffer, true );
		
	T_UID TUID = Recv.GetUID();
	USER_LIST* pUser = m_pUserMgr->FindUser( TUID );
	if( NULL == pUser )
	{
		S2ASSERT(FALSE);
	}

	if( S2MO_IS_SUCCESS( m_pRoomMgr->RoomBattleEnd( pUser ) ) )
	{
		DLG_SENDMESSAGE( DLG_MESSAGE_USER_UPDATE, (LPARAM)pUser );
	}
	else
	{
		PacketBattleEndAck Send;
		Send.m_TResult			= NET_RESULT_ERROR;
		Send.SetSessionID( Recv.GetSessionID() );
		Send.SetUID( Recv.GetUID() );
	
		pRing->m_pSession->SendPacket( &Send );
	}

	return NET_RESULT_SUCCESS;
}

T_RESULT CMainFrm::_RoomList( RING_IOCP_POP* pRing )
{
	PacketRoomListReq Recv;
	Recv.UnPacking_T( pRing->m_pBuffer, true );
		
	T_UID TUID = Recv.GetUID();
	USER_LIST* pUser = m_pUserMgr->FindUser( TUID );
	if( NULL == pUser )
	{
		S2ASSERT(FALSE);
	}

	if( S2MO_IS_SUCCESS( m_pRoomMgr->RoomList( pUser ) ) )
	{
		DLG_SENDMESSAGE( DLG_MESSAGE_USER_UPDATE, (LPARAM)pUser );
	}
	else
	{
		PacketRoomListAck Send;
		Send.m_TResult			= NET_RESULT_ERROR;
		Send.SetSessionID( Recv.GetSessionID() );
		Send.SetUID( Recv.GetUID() );
	
		pRing->m_pSession->SendPacket( &Send );
	}

	return NET_RESULT_SUCCESS;
}