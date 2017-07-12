#include "stdafx.h"
#include "S2MOPacket.h"
//
//void S2MOPacketBase::Reset()
//{
//	m_TPacketSize = 0;
//}
//
//bool S2MOPacketBase::UnPacking_Head( char* pBuffer )
//{
//	m_TPacketSize		= *((T_PACKET_SIZE*)pBuffer);		pBuffer += sizeof(T_PACKET_SIZE);
//	m_TProtocol			= *((T_PACKET_PROTOCOL*)pBuffer);	pBuffer += sizeof(T_PACKET_PROTOCOL);
//	m_i16PacketRandSeed = *((INT16*)pBuffer);
//	
//	return true;
//}
//
//bool S2MOPacketBase::Packing_T( char* pBuffer )
//{
//	if( false == Packing_C( pBuffer+S2MO_PACKET_HEAD_SIZE ) )					return false;
//	memcpy( pBuffer, &m_TPacketSize, S2MO_PACKET_HEAD_SIZE );
//	return true;
//}
//
//bool S2MOPacketBase::UnPacking_T( char* pBuffer )
//{	
//	memcpy( &m_TPacketSize, pBuffer, S2MO_PACKET_HEAD_SIZE );
//	if( false == UnPacking_C( pBuffer+S2MO_PACKET_HEAD_SIZE ) )					return false;
//	return true;
//}
//
//bool S2MOPacketBase::Packing_C(	char* pBuffer )
//{	pBuffer;
//	return true;
//}
//
//bool S2MOPacketBase::UnPacking_C( char* pBuffer )
//{
//	pBuffer;
//	return true;
//}