#ifndef __SVR_PACKET_H__
#define __SVR_PACKET_H__


struct PacketConnectAuthReq : public S2MOPacketBaseT<PROTOCOL_BASE_CONNECT_AUTH_REQ>
{
	S2MOINT8												m_ui8Type;
	S2MOUINT32												m_ui32ServerIdx;

	PacketConnectAuthReq()
	{
		_SetValue( m_ui8Type );
		_SetValue( m_ui32ServerIdx );
	}
};

struct PacketDediStartReq : public S2MOPacketBaseT<PROTOCOL_DEDI_START_REQ>
{
	S2MOUINT32												m_ui32RoomIdx;
	S2MOVariable<T_MapID>									m_TMapID;

	PacketDediStartReq()
	{
		_SetValue( m_ui32RoomIdx );
		_SetValue( m_TMapID );
	};
};

struct PacketDediStartAck : public S2MOPacketBaseResultT<PROTOCOL_DEDI_START_ACK>
{
};

struct PacketDediLoadedAck : public S2MOPacketBaseResultT<PROTOCOL_DEDI_LOADED_ACK>
{
};

struct PacketDediEndAck : public S2MOPacketBaseT<PROTOCOL_DEDI_END_ACK>
{
};

struct PacketLogoutReq : public S2MOPacketBaseT<PROTOCOL_LOGOUT_REQ>
{
};

struct PacketUserInfoSaveReq : public S2MOPacketBaseT<PROTOCOL_USER_INFO_SAVE_REQ>
{
	S2MOVariable<T_UID>										m_TUID;
	NET_USER_BASE											m_Base;
	NET_USER_INFO											m_Info;
	NET_USER_RECORD											m_Record;
	NET_USER_EQUIPMENT										m_Eequipment;

	PacketUserInfoSaveReq()
	{
		_SetValue( m_TUID );
		_SetValue( m_Base );
		_SetValue( m_Info );
		_SetValue( m_Record );
		_SetValue( m_Eequipment );
	};
};

#endif
