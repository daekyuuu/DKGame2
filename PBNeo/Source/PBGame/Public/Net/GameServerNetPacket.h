#ifndef __GS_NET_PACKET_H__
#define __GS_NET_PACKET_H__

#include "S2MOPacket.h"
#include "GameServerNetDefine.h"
#include "GameServerNetResult.h"
#include "GameServerNetProtocol.h"

struct GsPacketConnectReq : public S2MOPacketBaseT<GS_PROTOCOL_BASE_CONNECT_REQ>
{
	S2MOStringW<GS_NET_IP_LENGTH> m_strIP;
	S2MOUINT16 m_ui16Port;

	GsPacketConnectReq()
	{
		_SetValue(m_strIP);
		_SetValue(m_ui16Port);
	};
};

struct GsPacketConnectAck : public S2MOPacketBaseResultT<GS_PROTOCOL_BASE_CONNECT_ACK>
{
};

struct GsPacketDisConnectReq : public S2MOPacketBaseT<GS_PROTOCOL_BASE_DISCONNECT_REQ>
{
};

struct GsPacketDisConnectAck : public S2MOPacketBaseResultT<GS_PROTOCOL_BASE_DISCONNECT_ACK>
{
};

struct GsPacketLoginReq : public S2MOPacketBaseT<GS_PROTOCOL_LOGIN_LOGIN_REQ>
{
	S2MOUINT16 m_ui16ListenPort;

	GsPacketLoginReq()
	{
		_SetValue(m_ui16ListenPort);
	};
};

struct GsPacketLoginAck : public S2MOPacketBaseResultT<GS_PROTOCOL_LOGIN_LOGIN_ACK>
{
	GsPacketLoginAck()
	{
	};
};

struct GsPacketLoadMapReq : public S2MOPacketBaseT<GS_PROTOCOL_CONTROL_LOADMAP_REQ>
{
	S2MOINT32 m_i32MapTableIndex;

	GsPacketLoadMapReq()
	{
		_SetValue(m_i32MapTableIndex);
	};
};

struct GsPacketLoadMapAck : public S2MOPacketBaseResultT<GS_PROTOCOL_CONTROL_LOADMAP_ACK>
{
	GsPacketLoadMapAck()
	{
	};
};

struct GsPacketShutdownReq : public S2MOPacketBaseT<GS_PROTOCOL_CONTROL_SHUTDOWN_REQ>
{
	GsPacketShutdownReq()
	{
	};
};

struct GsPacketShutdownAck : public S2MOPacketBaseResultT<GS_PROTOCOL_CONTROL_SHUTDOWN_ACK>
{
	GsPacketShutdownAck()
	{
	};
};

struct GsPacketKillReq : public S2MOPacketBaseT<GS_PROTOCOL_CONTROL_KILL_REQ>
{
	GsPacketKillReq()
	{
	};
};

#endif
