#include "stdafx.h"
#include "S2MO.h"
#include "S2MOBase.h"

#if defined(S2MO_EXPORTS) || defined(S2MO_IMPORTS)
S2MOBase* g_pS2MOBase = NULL;

bool S2MOCreate()
{
	g_pS2MOBase = new S2MOBase;
	if( NULL == g_pS2MOBase )					return false;
	if( false == g_pS2MOBase->Create() )		return false;

	return true;
}

void S2MODestory()
{
	if( g_pS2MOBase )
	{
		delete g_pS2MOBase;
		g_pS2MOBase = NULL;
	}
}

bool S2MOMakePacket( S2MOPacketBase* pPacket, char* pBuffer, bool bServer )
{
	pPacket->Reset();
	if( false == pPacket->Packing_T( pBuffer, bServer ) )			return false;

	return true;
}

bool S2MORecvPacket( S2MOPacketBase* pPacket, char* pBuffer, bool bServer )
{
	pPacket->Reset();
	if( false == pPacket->UnPacking_T( pBuffer, bServer ) )			return false;
	
	return true;
}

void S2MOSetPacketRandSeed( INT16 i16RandSeed )
{
	g_pS2MOBase->SetPacketRandSeed( i16RandSeed );
}

INT16 S2MOGetPacketRandSeed()
{
	return g_pS2MOBase->GetPacketRandSeed();
}

T_PACKET_SIZE S2MOChangePacketCS( char* pClientBuffer, char* pServerBuffer, T_UID &TUID, T_SESSIONID &TSessionID )
{
	S2MOPacketBase PacketBase;
	PacketBase.UnPacking_Head( pClientBuffer );

	T_PACKET_SIZE TSize = PacketBase.GetPacketSize() + S2MO_PACKET_SERVER_DATA_SIZE;
	if( S2MO_PACKET_CONTENT_SIZE < TSize )	return 0;

	memcpy( pServerBuffer, pClientBuffer,	S2MO_PACKET_HEAD_SIZE );
	memcpy( pServerBuffer, &TSize,			sizeof(T_PACKET_SIZE) );
	pServerBuffer += S2MO_PACKET_HEAD_SIZE;
	pClientBuffer += S2MO_PACKET_HEAD_SIZE;
	memcpy( pServerBuffer, &TUID,			sizeof(T_UID) );				pServerBuffer += sizeof(T_UID);
	memcpy( pServerBuffer, &TSessionID,		sizeof(T_SESSIONID) );			pServerBuffer += sizeof(T_SESSIONID);
	memcpy( pServerBuffer, pClientBuffer,	PacketBase.GetPacketSize()-S2MO_PACKET_HEAD_SIZE );

	return TSize;
}

T_PACKET_SIZE S2MOChangePacketSC( char* pServerBuffer, char* pClientBuffer, T_UID &TUID, T_SESSIONID &TSessionID )
{
	S2MOPacketBase PacketBase;
	PacketBase.UnPacking_Head( pServerBuffer );

	T_PACKET_SIZE TSize = PacketBase.GetPacketSize() - S2MO_PACKET_SERVER_DATA_SIZE;

	memcpy( pClientBuffer,	pServerBuffer,	S2MO_PACKET_HEAD_SIZE );
	memcpy( pClientBuffer,	&TSize,			sizeof(T_PACKET_SIZE) );
	pServerBuffer += S2MO_PACKET_HEAD_SIZE;
	pClientBuffer += S2MO_PACKET_HEAD_SIZE;
	memcpy( &TUID,			pServerBuffer,	sizeof(T_UID) );				pServerBuffer += sizeof(T_UID);
	memcpy( &TSessionID,	pServerBuffer,	sizeof(T_SESSIONID) );			pServerBuffer += sizeof(T_SESSIONID);
	memcpy( pClientBuffer,	pServerBuffer,	TSize-S2MO_PACKET_HEAD_SIZE );

	return TSize;
}

#endif