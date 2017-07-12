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

#ifdef S2MO_EXPORTS
#include "S2MOPacket.h"
bool 	S2MO_EXPORT_BASE		S2MOCreate();
void 	S2MO_EXPORT_BASE		S2MODestory();

bool 	S2MO_EXPORT_BASE		S2MOMakePacket( S2MOPacketBase* pPacket, char* pBuffer, bool bServer );
bool 	S2MO_EXPORT_BASE		S2MORecvPacket( S2MOPacketBase* pPacket, char* pBuffer, bool bServer );

void	S2MO_EXPORT_BASE		S2MOSetPacketRandSeed( INT16 i16RandSeed );
INT16	S2MO_EXPORT_BASE		S2MOGetPacketRandSeed();
#endif
#endif