#ifndef __NET_PACKET_H__
#define __NET_PACKET_H__

#define USE_UNREAL4

#include "S2MOPacket.h"
#include "NetDefine.h"
#include "NetResult.h"
#include "NetProtocol.h"

struct PacketConnectReq : public S2MOPacketBaseT<PROTOCOL_BASE_CONNECT_REQ>
{
	S2MOStringW<NET_IP_LENGTH>										m_strIP;
	S2MOUINT16														m_ui16Port;

	PacketConnectReq()
	{
		_SetValue( m_strIP );
		_SetValue( m_ui16Port );
	};
};

struct PacketConnectAck : public S2MOPacketBaseResultT<PROTOCOL_BASE_CONNECT_ACK>
{
};

struct PacketHeartbitReq : public S2MOPacketBaseT<PROTOCOL_BASE_HEARTBIT_REQ>
{
};

struct PacketHeartbitAck : public S2MOPacketBaseT<PROTOCOL_BASE_HEARTBIT_ACK>
{
};

struct PacketNoticeAck : public S2MOPacketBaseT<PROTOCOL_BASE_NOTICE_ACK>
{
	S2MOINT8														m_i8Type;		// NET_NOTICE_TYPE
	S2MOStringW<NET_NOTICE_LENGTH>									m_strNotice;

	PacketNoticeAck()
	{
		_SetValue( m_i8Type );
		_SetValue( m_strNotice );
	};
};

struct PacketDisConnectReq : public S2MOPacketBaseT<PROTOCOL_BASE_DISCONNECT_REQ>
{
};

struct PacketDisConnectAck : public S2MOPacketBaseResultT<PROTOCOL_BASE_DISCONNECT_ACK>
{
};

struct PacketLoginReq : public S2MOPacketBaseT<PROTOCOL_LOGIN_INPUT_REQ>
{
	S2MOStringW<NET_ID_LENGTH>										m_strID;
	S2MOStringW<NET_PW_LENGTH>										m_strPW;

	PacketLoginReq()
	{
		_SetValue( m_strID );
		_SetValue( m_strPW );
	};
};

struct PacketLoginAck : public S2MOPacketBaseResultT<PROTOCOL_LOGIN_INPUT_ACK>
{
	S2MOVariable<T_UID>												m_TUID;

	PacketLoginAck()
	{
		_SetValue( m_TUID );
	};
};

struct PacketUserInfoLoadReq : public S2MOPacketBaseT<PROTOCOL_USER_INFO_LOAD_REQ>
{
};

// Result 
//		1. NET_RESULT_SUCCESS							- Success
//		2. NET_RESULT_SUCCESS_FIRST_CONNECT				- Success but first login. You have to create nickname. ( PacketUserChangeNickReq )
struct PacketUserInfoLoadAck : public S2MOPacketBaseResultT<PROTOCOL_USER_INFO_LOAD_ACK>
{
	NET_USER_BASE													m_Base;
	NET_USER_INFO													m_Info;
	NET_USER_RECORD													m_RecordTotal;
	NET_USER_RECORD													m_RecordSeason;
	NET_USER_EQUIPMENT												m_Eequipment;

	PacketUserInfoLoadAck()
	{
		_SetValue( m_Base );
		_SetValue( m_Info );
		_SetValue( m_RecordTotal );
		_SetValue( m_RecordSeason );
		_SetValue( m_Eequipment );
	};
};

struct PacketUserChangeNickReq : public S2MOPacketBaseT<PROTOCOL_USER_CHANGE_NICK_REQ>
{
	S2MOStringW<NET_NICK_LENGTH>									m_strNickname;

	PacketUserChangeNickReq()
	{
		_SetValue( m_strNickname );
	};
};

struct PacketUserChangeNickAck : public S2MOPacketBaseResultT<PROTOCOL_USER_CHANGE_NICK_ACK>
{
	S2MOStringW<NET_NICK_LENGTH>									m_strNickname;

	PacketUserChangeNickAck()
	{
		_SetValue( m_strNickname );
	};
};

struct PacketRoomCreateReq : public S2MOPacketBaseT<PROTOCOL_ROOM_CREATE_REQ>
{
	NET_ROOM_CREATE_INFO											m_RoomCreateInfo;

	PacketRoomCreateReq()
	{
		_SetValue( m_RoomCreateInfo );
	};
};

struct PacketRoomCreateAck : public S2MOPacketBaseResultT<PROTOCOL_ROOM_CREATE_ACK>
{
	S2MOUINT32														m_ui32RoomIdx;

	PacketRoomCreateAck()
	{
		_SetValue( m_ui32RoomIdx );
	};
};

struct PacketRoomEnterReq : public S2MOPacketBaseT<PROTOCOL_ROOM_ENTER_REQ>
{
	S2MOUINT32														m_ui32RoomIdx;

	PacketRoomEnterReq()
	{
		_SetValue( m_ui32RoomIdx );
	}
};

struct PacketRoomEnterAck : public S2MOPacketBaseResultT<PROTOCOL_ROOM_ENTER_ACK>
{
	NET_ROOM_INFO													m_RoomInfo;
	NET_ROOM_SLOT_INFO												m_SlotInfo;
	S2MOUINT8														m_ui8ReminTimeSec;
	S2MOStruct<NET_ROOM_MAP_INFO, NET_ROOM_MAP_LIST_COUNT>			m_MapInfo;

	PacketRoomEnterAck()
	{
		_SetValue( m_RoomInfo );
		_SetValue( m_SlotInfo );
		_SetValue( m_ui8ReminTimeSec );
		_SetValue( m_MapInfo );
	};
};

struct PacketRoomEnterTransAck : public S2MOPacketBaseT<PROTOCOL_ROOM_ENTER_TRANS_ACK>
{
	NET_ROOM_SLOT_INFO												m_SlotInfo;

	PacketRoomEnterTransAck()
	{
		_SetValue( m_SlotInfo );
	};
};

struct PacketRoomLeaveReq : public S2MOPacketBaseT<PROTOCOL_ROOM_LEAVE_REQ>
{
};

struct PacketRoomLeaveAck : public S2MOPacketBaseResultT<PROTOCOL_ROOM_LEAVE_ACK>
{
};

struct PacketRoomLeaveTransAck : public S2MOPacketBaseT<PROTOCOL_ROOM_LEAVE_TRANS_ACK>
{
	NET_ROOM_SLOT_BASE												m_SlotBase;	

	PacketRoomLeaveTransAck()
	{
		_SetValue( m_SlotBase );
	};
};

struct PacketRoomQuickMatchReq : public S2MOPacketBaseT<PROTOCOL_ROOM_QUICK_MATCH_REQ>
{
	S2MOStruct<NET_ROOM_ENTER_INFO, NET_ROOM_MODE_MAX>				m_EnterInfo;

	PacketRoomQuickMatchReq()
	{
		_SetValue( m_EnterInfo );
	};
};

struct PacketRoomQuickMatchAck : public S2MOPacketBaseResultT<PROTOCOL_ROOM_QUICK_MATCH_ACK>
{
};

struct PacketRoomSlotInfoReq : public S2MOPacketBaseT<PROTOCOL_ROOM_SLOT_INFO_REQ>
{
	S2MOVariable<INT8, NET_ROOM_USER_COUNT>							m_i8SlotIdx;

	PacketRoomSlotInfoReq()
	{
		_SetValue( m_i8SlotIdx );
	};
};

struct PacketRoomSlotInfoAck : public S2MOPacketBaseResultT<PROTOCOL_ROOM_SLOT_INFO_ACK>
{
	S2MOStruct<NET_ROOM_SLOT_INFO, NET_ROOM_USER_COUNT>				m_SlotInfo;

	PacketRoomSlotInfoAck()
	{
		_SetValue( m_SlotInfo );
	}
};

struct PacketRoomOptionChangeInfoReq : public S2MOPacketBaseT<PROTOCOL_ROOM_OPTION_CHANGE_REQ>
{
	NET_ROOM_INFO													m_RoomInfo;

	PacketRoomOptionChangeInfoReq()
	{
		_SetValue( m_RoomInfo );
	};
};

struct PacketRoomOptionChangeInfoAck : public S2MOPacketBaseResultT<PROTOCOL_ROOM_OPTION_CHANGE_ACK>
{
	NET_ROOM_INFO													m_RoomInfo;

	PacketRoomOptionChangeInfoAck()
	{
		_SetValue( m_RoomInfo );
	};
};

struct PacketRoomMapVoteReq : public S2MOPacketBaseT<PROTOCOL_ROOM_MAP_VOTE_REQ>
{
	S2MOUINT32														m_ui32MapIdx;

	PacketRoomMapVoteReq()
	{
		_SetValue( m_ui32MapIdx );
	};
};

struct PacketRoomMapVoteAck : public S2MOPacketBaseResultT<PROTOCOL_ROOM_MAP_VOTE_ACK>
{
};

struct PacketRoomMapVoteTransAck : public S2MOPacketBaseT<PROTOCOL_ROOM_MAP_VOTE_TRANS_ACK>
{
	S2MOStruct<NET_ROOM_MAP_INFO, NET_ROOM_MAP_LIST_COUNT>			m_MapInfo;

	PacketRoomMapVoteTransAck()
	{
		_SetValue( m_MapInfo );
	};
};

struct PacketRoomSlotReadyReq : public S2MOPacketBaseT<PROTOCOL_ROOM_SLOT_READY_REQ>
{
};

struct PacketRoomSlotReadyAck : public S2MOPacketBaseResultT<PROTOCOL_ROOM_SLOT_READY_ACK>
{
};

struct PacketRoomStateChangeAck : public S2MOPacketBaseT<PROTOCOL_ROOM_STATE_CHANGE_ACK>
{
	S2MOINT8														m_i8State;						//NET_ROOM_STATE
	S2MOUINT8														m_ui8ReminTimeSec;

	PacketRoomStateChangeAck()
	{
		_SetValue( m_i8State );
		_SetValue( m_ui8ReminTimeSec );
	};
};

struct PacketRoomSlotStateChangeAck : public S2MOPacketBaseT<PROTOCOL_ROOM_SLOT_STATE_CHANGE_ACK>
{
	S2MOStruct<NET_ROOM_SLOT_BASE, NET_ROOM_USER_COUNT>				m_SlotBase;

	PacketRoomSlotStateChangeAck()
	{
		_SetValue( m_SlotBase );
	};
};

struct PacketBattleStartAck : public S2MOPacketBaseT<PROTOCOL_ROOM_BATTLE_START_ACK>
{
	S2MOUINT32														m_ui32MapIdx;
	S2MOStruct<NET_ROOM_SLOT_BASE, NET_ROOM_PLAYER_USER_COUNT>		m_StartSlot;

	PacketBattleStartAck()
	{
		_SetValue( m_ui32MapIdx );
		_SetValue( m_StartSlot );
	};
};

struct PacketBattleEndReq : public S2MOPacketBaseT<PROTOCOL_ROOM_BATTLE_END_REQ>
{
};

struct PacketBattleEndAck : public S2MOPacketBaseResultT<PROTOCOL_ROOM_BATTLE_END_ACK>
{
};

struct PacketRoomListReq : public S2MOPacketBaseT<PROTOCOL_ROOM_LIST_REQ>
{
};

struct PacketRoomListAck : public S2MOPacketBaseResultT<PROTOCOL_ROOM_LIST_ACK>
{
	S2MOStruct<NET_ROOM_INFO, NET_ROOM_LIST_PAGE_COUNT>				m_RoomList;

	PacketRoomListAck()
	{
		_SetValue( m_RoomList );
	};
};

struct PacketBattleReadyReq : public S2MOPacketBaseT<PROTOCOL_ROOM_BATTLE_READY_REQ>
{
};

struct PacketBattleReadyAck : public S2MOPacketBaseResultT<PROTOCOL_ROOM_BATTLE_READY_ACK>
{
};

#endif
