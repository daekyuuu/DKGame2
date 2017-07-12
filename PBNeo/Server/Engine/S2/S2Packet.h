#ifndef __S2_PACKET_H__
#define __S2_PACKET_H__

#include "S2Memory.h"

class S2_EXPORT_BASE S2Packet
{
	// Buffer 는 꼭 맨 생단에 있어야 하며 virtual 을 절대 사용하면 안됩니다.
	char					m_pPacketBuffer[ S2_PACKET_BUFFER_SIZE ];

	INT32					m_i32ReadPosition;
	INT32					m_i32WritePosition;

protected:

	void					_AddPacketSize( INT32 i32Size );

public:
	S2Packet();
	S2Packet( PACKET_PROTOCOL Protocol );
	~S2Packet();							// Virtual을 사용할 수 없습니다.

	void					SetSize( PACKET_SIZE Size );
	void					SetProtocol( PACKET_PROTOCOL Protocol );
	BOOL					ReadData( void* pData, INT32 i32Size );
	BOOL					WriteData( void* pData, INT32 i32Size );
	BOOL					WriteData( void* pData, INT32 i32Size, INT32 i32Position );

	PACKET_SIZE				GetPacketTotalSize()							{	return GetPacketSize()+S2_PACKET_HEAD_SIZE;	}
	PACKET_SIZE				GetPacketSize()									{	return (*((PACKET_SIZE*)m_pPacketBuffer));	}

	PACKET_PROTOCOL			GetPacketProtocol()								{	return (*((PACKET_PROTOCOL*)(m_pPacketBuffer+sizeof(PACKET_SIZE))));	}
	
	char*					GetPacketBuffer()								{	return m_pPacketBuffer;						}
	char*					GetPacketDataBuffer()							{	return (m_pPacketBuffer+S2_PACKET_HEAD_SIZE);	}
	
	void					SetPacketBuffer( char* pBuffer )				{	S2Memory::Copy( m_pPacketBuffer, pBuffer, S2_PACKET_BUFFER_SIZE );	}
	void					SetPacketBuffer( char* pBuffer, INT32 i32Size )	{	S2Memory::Copy( m_pPacketBuffer, pBuffer, i32Size );			}

	void					Clear();
};

#endif