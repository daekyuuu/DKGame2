#ifndef __S2MO_PACKET_H__
#define __S2MO_PACKET_H__

#include <iostream>
using namespace std;

#include "S2MODataType.h"

#define S2MO_PACKET_CONTENT_SIZE			8192
#define S2MO_PACKET_HEAD_SIZE				6
#define S2MO_PACKET_SERVER_DATA_SIZE		sizeof(T_UID)+sizeof(T_SESSIONID)

#pragma pack( push, 1)

struct S2MO_EXPORT_BASE S2MOPacketBase
{
protected:
	T_PACKET_SIZE			m_TPacketSize;
	T_PACKET_PROTOCOL		m_TProtocol;

	T_PACKET_RANDSEED		m_TPacketRandSeed;

	T_UID					m_TUID;
	T_SESSIONID				m_TSessionID;

public :
	S2MOPacketBase()	
	{	
		m_TPacketSize	= 0;
	}
	S2MOPacketBase( T_PACKET_PROTOCOL TProtocol )	
	{	
		m_TPacketSize	= 0;
		m_TProtocol		= TProtocol;	
	}

	T_PACKET_SIZE			GetPacketSize()											{	return m_TPacketSize;							}
	T_PACKET_PROTOCOL		GetProtocol()											{	return m_TProtocol;								}
	T_UID					GetUID()												{	return m_TUID;									}
	T_SESSIONID				GetSessionID()											{	return m_TSessionID;							}

	void					SetPacketRandSeed( T_PACKET_RANDSEED i16RandSeed )		{	m_TPacketRandSeed	= i16RandSeed;				}
	void					SetProtocol( T_PACKET_PROTOCOL TProtocol )				{	m_TProtocol			= TProtocol;				}
	void					SetUID( T_UID TUID )									{	m_TUID				= TUID;						}
	void					SetSessionID( T_SESSIONID TSessionID )					{	m_TSessionID		= TSessionID;				}

	inline void				Reset()
	{
		m_TPacketSize = 0;
	}

	virtual bool			UnPacking_Head( char* pBuffer )
	{
		m_TPacketSize		= *((T_PACKET_SIZE*)pBuffer);		pBuffer += sizeof(T_PACKET_SIZE);
		m_TProtocol			= *((T_PACKET_PROTOCOL*)pBuffer);	pBuffer += sizeof(T_PACKET_PROTOCOL);
		m_TPacketRandSeed	= *((T_PACKET_RANDSEED*)pBuffer);
	
		return true;
	}

	virtual bool			Packing_T( char* pBuffer, bool bServer = false )
	{
		int i32HeadCopySize = S2MO_PACKET_HEAD_SIZE;
		if( bServer )
		{
			i32HeadCopySize += S2MO_PACKET_SERVER_DATA_SIZE;
		}

		m_TPacketSize = 0;

		if( false == Packing_C( pBuffer+i32HeadCopySize ) )					return false;
		
		m_TPacketSize += i32HeadCopySize;
		memcpy( pBuffer, &m_TPacketSize, i32HeadCopySize );

		return true;
	}
	virtual bool			UnPacking_T( char* pBuffer, bool bServer = false )
	{
		int i32HeadCopySize = S2MO_PACKET_HEAD_SIZE;
		if( bServer )
		{
			i32HeadCopySize += S2MO_PACKET_SERVER_DATA_SIZE;
		}
		memcpy( &m_TPacketSize, pBuffer, i32HeadCopySize );
		if( false == UnPacking_C( pBuffer+i32HeadCopySize ) )					return false;
		return true;
	}
	virtual bool			Packing_C( char* pBuffer )					{	return true;	}
	virtual bool			UnPacking_C( char* pBuffer )				{	return true;	}

	void					Copy( const S2MOPacketBase& pBase )
	{
		m_TPacketSize			= pBase.m_TPacketSize;
		m_TProtocol				= pBase.m_TProtocol;
		m_TPacketRandSeed		= pBase.m_TPacketRandSeed;
		m_TUID					= pBase.m_TUID;
		m_TSessionID			= pBase.m_TSessionID;
	}

	S2MOPacketBase& operator=( const S2MOPacketBase& Org )
	{
		Copy( Org );
		return *this;
	}
};

#pragma pack( pop )

template <T_PACKET_PROTOCOL P>
struct S2MOPacketBaseT : public S2MOPacketBase
{
	S2MOPackable*			m_pS2Packable;
	S2MOPackable*			m_pS2InsertPoint;

protected:
	void					_SetValue( S2MOPackable& pData )
	{
		if( m_pS2InsertPoint )
		{
			m_pS2InsertPoint->SetNext( &pData );
			m_pS2InsertPoint	= m_pS2InsertPoint->GetNext();
		}
		else
		{
			m_pS2Packable		= &pData;
			m_pS2InsertPoint	= m_pS2Packable;
		}
	}

public:	
	
	S2MOPacketBaseT()
		: S2MOPacketBase( P )
	{
		m_pS2Packable		= NULL;
		m_pS2InsertPoint	= NULL;
	}
	
	bool					Packing_C( char* pBuffer )
	{
		S2MOPackable* pS2Packable = m_pS2Packable;
		int i32Size = 0;
		while( pS2Packable )
		{
			if( false == pS2Packable->Packing( &pBuffer[ m_TPacketSize ], i32Size ) )				return false;
			m_TPacketSize	+= (T_PACKET_SIZE)i32Size;

			pS2Packable = pS2Packable->GetNext();
		}
		
		return true;
	}
	bool					UnPacking_C( char* pBuffer )
	{
		int i32Size = 0;
		int i32PacketSize = 0;
		S2MOPackable* pS2Packable = m_pS2Packable;
		while( pS2Packable )
		{
			if( false == pS2Packable->UnPacking( &pBuffer[ i32PacketSize ], i32Size ) )				return false;
			i32PacketSize += i32Size;

			pS2Packable = pS2Packable->GetNext();
		}
		return true;
	}

	void					Copy( const S2MOPacketBaseT& pBase )
	{
		S2MOPacketBase::Copy( pBase );

		S2MOPackable* pS2Dis	= m_pS2Packable;
		S2MOPackable* pS2Src	= pBase.m_pS2Packable;
		while( pS2Src && pS2Dis )
		{
			pS2Dis->Copy( pS2Src );

			pS2Src = pS2Src->GetNext();
			pS2Dis = pS2Dis->GetNext();
		}
	}

	S2MOPacketBaseT& operator=( const S2MOPacketBaseT& Org )
	{
		Copy( Org );
		return *this;
	}
};

template <T_PACKET_PROTOCOL P>
struct S2MOPacketBaseResultT : public S2MOPacketBaseT<P>
{
public:

	T_RESULT				m_TResult;

	virtual bool			Packing_C( char* pBuffer )
	{
		memcpy( pBuffer, &m_TResult, sizeof(T_RESULT) );
		m_TPacketSize += sizeof(T_RESULT);

		if( S2MO_IS_SUCCESS( m_TResult ) )
		{
			if( false == S2MOPacketBaseT::Packing_C( pBuffer ) )									return false;
		}
		return true;
	}
	virtual bool			UnPacking_C( char* pBuffer )
	{
		memcpy( &m_TResult, pBuffer, sizeof(T_RESULT) );

		if( S2MO_IS_SUCCESS( m_TResult ) )
		{
			if( false == S2MOPacketBaseT::UnPacking_C( &pBuffer[sizeof(T_RESULT)] ) )				return false;
		}
		return true;
	}

	void					Copy( const S2MOPacketBaseResultT& pBase )
	{
		S2MOPacketBaseT::Copy( pBase );
		m_TResult = pBase.m_TResult;
	}

	virtual S2MOPacketBaseResultT&		operator=( const S2MOPacketBaseResultT& Org )
	{
		Copy( Org );
		return *this;
	}
};

#endif