#ifndef __S2MO_H__
#define __S2MO_H__

#define S2MO_VER_MAJOR			0
#define S2MO_VER_MINOR			1

#ifdef S2MO_EXPORTS
	#define	S2MO_EXPORT_BASE	__declspec( dllexport)
#elif S2MO_IMPORTS
	#define	S2MO_EXPORT_BASE	__declspec( dllimport)
#else
	#define	S2MO_EXPORT_BASE	
#endif

#include "S2MODefine.h"

#if defined(S2MO_EXPORTS) || defined(S2MO_IMPORTS)
#include "S2MOPacket.h"
bool 			S2MO_EXPORT_BASE		S2MOCreate();
void 			S2MO_EXPORT_BASE		S2MODestory();

bool 			S2MO_EXPORT_BASE		S2MOMakePacket( S2MOPacketBase* pPacket, char* pBuffer, bool bServer );
bool 			S2MO_EXPORT_BASE		S2MORecvPacket( S2MOPacketBase* pPacket, char* pBuffer, bool bServer );

void			S2MO_EXPORT_BASE		S2MOSetPacketRandSeed( INT16 i16RandSeed );
INT16			S2MO_EXPORT_BASE		S2MOGetPacketRandSeed();

T_PACKET_SIZE	S2MO_EXPORT_BASE		S2MOChangePacketCS( char* pClientBuffer, char* pServerBuffer, T_UID &TUID, T_SESSIONID &TSessionID );
T_PACKET_SIZE	S2MO_EXPORT_BASE		S2MOChangePacketSC( char* pServerBuffer, char* pClientBuffer, T_UID &TUID, T_SESSIONID &TSessionID );
#endif
#endif