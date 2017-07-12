#include "stdafx.h"
#include "S2Packet.h"

S2Packet::S2Packet()
{
	Clear();
}

S2Packet::S2Packet( PACKET_PROTOCOL Protocol )
{
	Clear();
	SetProtocol( Protocol );
}

S2Packet::~S2Packet()
{
}

void	S2Packet::SetSize( PACKET_SIZE Size )
{
	PACKET_SIZE* pPacketSize	= (PACKET_SIZE*)(m_pPacketBuffer);
	*pPacketSize = Size;
}

void	S2Packet::SetProtocol( PACKET_PROTOCOL Protocol )
{
	PACKET_PROTOCOL* pPacketProtocol	= (PACKET_PROTOCOL*)(m_pPacketBuffer+sizeof(PACKET_SIZE));
	*pPacketProtocol = Protocol;
}

BOOL	S2Packet::ReadData( void* pData, INT32 i32Size )
{
	if( S2_PACKET_BUFFER_SIZE < (m_i32ReadPosition+i32Size) )	return FALSE;

	S2Memory::Copy( pData, &m_pPacketBuffer[ m_i32ReadPosition ], i32Size );

	m_i32ReadPosition	+= i32Size;

	return TRUE;
}

BOOL	S2Packet::WriteData( void* pData, INT32 i32Size )
{
	if( S2_PACKET_BUFFER_SIZE < (m_i32WritePosition+i32Size) )	return FALSE;

	S2Memory::Copy( &m_pPacketBuffer[ m_i32WritePosition ], pData, i32Size );

	m_i32WritePosition	+= i32Size;
	_AddPacketSize( i32Size );

	return TRUE;
}

BOOL	S2Packet::WriteData( void* pData, INT32 i32Size, INT32 i32Position )
{
	if( S2_PACKET_BUFFER_SIZE < (i32Position+i32Size) )		return FALSE;

	S2Memory::Copy( &m_pPacketBuffer[ i32Position ], pData, i32Size );

	return TRUE;
}

void	S2Packet::Clear()
{
	m_i32ReadPosition	= S2_PACKET_HEAD_SIZE;
	m_i32WritePosition	= S2_PACKET_HEAD_SIZE;

	S2Memory::FillZero( m_pPacketBuffer, S2_PACKET_HEAD_SIZE );
}

void	S2Packet::_AddPacketSize( INT32 i32Size )
{
	PACKET_SIZE* pPacketSize = (PACKET_SIZE*)m_pPacketBuffer;
	*pPacketSize += (PACKET_SIZE)i32Size;
}