#ifndef __S2MO_DATA_TYPE_H__
#define __S2MO_DATA_TYPE_H__

#include "S2MO.h"

class S2MO_EXPORT_BASE S2MOPackable 
{
protected:
	S2MOPackable*			m_pNext;

protected:

	bool					_PackingSize( char* pBuffer, int i32Count, int i32MaxSize, int& i32Size )
	{
		if( i32MaxSize < 0xFF )
		{
			unsigned char* pTemp = (unsigned char*)pBuffer;
			*pTemp = (unsigned char)i32Count;
		
			i32Size = sizeof(char);
		}
		else if( i32MaxSize < 0xFFFF )
		{
			unsigned short* pTemp = (unsigned short*)pBuffer;
			*pTemp = (unsigned short)i32Count;

			i32Size = sizeof(short);
		}
		else
		{
			return false;
		}
		return true;
	}
	bool					_UnPackingSize( char* pBuffer, int& i32Count, int i32MaxSize, int& i32Size )
	{
		if( i32MaxSize < 0xFF )
		{
			unsigned char ui8Size = *((unsigned char*)pBuffer);
			i32Count = ui8Size;

			i32Size = sizeof(char);
		
		}
		else if( i32MaxSize < 0xFFFF )
		{
			unsigned short ui16Size = *((unsigned short*)pBuffer);
			i32Count = ui16Size;

			i32Size = sizeof(short);
		}
		else
		{
			return false;
		}
		return true;
	}
	
public:
	S2MOPackable()																{	m_pNext = NULL;					}
	virtual ~S2MOPackable()														{}

	virtual void			Copy( const S2MOPackable* pOrg )					{}

	S2MOPackable*			GetNext()											{	return m_pNext;					}
	void					SetNext( S2MOPackable* pPackable )					{	m_pNext = pPackable;			}

	virtual bool			Packing( char* pBuffer, int& i32Size )				{	return true;					}
	virtual bool			UnPacking( char* pBuffer, int& i32Size )			{	return true;					}
};

template <typename T, int N=1>
class S2MOVariable : public S2MOPackable 
{
protected:
	int						m_i32Count;
	T						m_TData[ N ];

public:
	S2MOVariable()				{	m_i32Count = 0; }
	virtual ~S2MOVariable()		{}
	
	virtual bool			Packing( char* pBuffer, int& i32Size )
	{
		if( N > 1 )
		{
			if( false == _PackingSize( pBuffer, m_i32Count, N, i32Size ) )		return false;
			if( 0 < m_i32Count )
			{
				int i32FullSize = sizeof( T )*m_i32Count;
				memcpy( &pBuffer[ i32Size ], m_TData, i32FullSize );
				i32Size += i32FullSize;
			}
		}
		else
		{
			T* pValue	= (T*)pBuffer;
			*pValue		= m_TData[0];
			i32Size		= sizeof(T);
		}
		return true;
	}
	virtual bool			UnPacking( char* pBuffer, int& i32Size )
	{
		if( N > 1 )
		{
			if( false == _UnPackingSize( pBuffer, m_i32Count, N, i32Size ) )	return false;
			if( N < m_i32Count )												return false;
			if( 0 < m_i32Count )
			{
				int i32FullSize = sizeof( T )*m_i32Count;
				memcpy( m_TData, &pBuffer[ i32Size ], i32FullSize );
				i32Size += i32FullSize;
			}
		}
		else
		{
			T* pValue	= (T*)pBuffer;
			m_TData[0]	= *pValue;
			i32Size		= sizeof(T);
			m_i32Count	= 1;
		}
		return true;
	}

	virtual void			Copy( const S2MOPackable* pOrg )
	{
		const S2MOVariable* pValue = (S2MOVariable*)pOrg;
		m_i32Count = pValue->m_i32Count;
		for( int i = 0 ; i < m_i32Count ; i++ )
		{
			m_TData[i] = pValue->m_TData[i];
		}
	}

	virtual void			GetValue( T* pTData, const int i32CountofBuffer, int* pi32Count = NULL )
	{
		int i32CopyCount = m_i32Count;
		if( i32CountofBuffer < m_i32Count )
		{
#ifdef _DEBUG
			_ASSERT( false );
#else
			i32CopyCount = i32CountofBuffer;
#endif
		}
		if( pi32Count )
		{
			*pi32Count = i32CopyCount;
		}
		if( 0 < i32CopyCount )
		{
			for( int i = 0 ; i < i32CopyCount ; i++ )
			{
				pTData[ i ] = m_TData[ i ];
			}
		}
	}

	void					SetValue( const T* pTData, const int i32StartIdx, const int i32CountofBuffer )
	{
		int i32CopyCount = i32CountofBuffer;
		if( 0 >= i32CountofBuffer )											return;
		int i32Idx = i32StartIdx + i32CountofBuffer;
		if( N < i32Idx )
		{
#ifdef _DEBUG
			_ASSERT( false );
#else
			i32CopyCount = i32CountofBuffer - (i32Idx-N);
#endif
		}
				
		for( int i = 0 ; i < i32CopyCount ; i++ )
		{
			m_TData[ i32StartIdx+i ] = pTData[ i ];
		}
		if( m_i32Count < i32Idx ) m_i32Count = i32Idx;
	}

	INT32&					GetCount()
	{
		return m_i32Count;
	}

	void					SetCount( INT32 i32Count )
	{
		m_i32Count = i32Count;
	}

	operator T() const
	{
		return *m_TData;
	}
	T*						operator&()
	{
		return m_TData;
	}

	virtual S2MOVariable&		operator=( const S2MOVariable& Org )
	{
		Copy( &Org );
		return *this;
	}
	virtual void			operator=( const T& TData )
	{
		if( m_i32Count < 1 )			m_i32Count = 1;
		*m_TData = TData;
	}
	virtual void			operator=( const T* TData )
	{
		for( int i  = 0 ; i < N ; i++ )
		{
			m_TData[i] = TData[i];
		}
		m_i32Count = N;
	}
	T&						operator[]( const int i32Idx )
	{
		if( N <= i32Idx )					_ASSERT( false );
		if( m_i32Count < ( i32Idx + 1 ) )	m_i32Count = i32Idx + 1;
		return m_TData[ i32Idx ];
	}
};


template <typename T, int N=1>
class S2MOStruct : public S2MOVariable<T, N>
{
public:
	S2MOStruct()			{}
	virtual ~S2MOStruct()	{}
	
	virtual bool			Packing( char* pBuffer, int& i32Size )
	{
		if( N > 1 )
		{
			if( false == _PackingSize( pBuffer, m_i32Count, N, i32Size ) )		return false;
		}
		int i32WriteSize;
		for( int i = 0 ; i < m_i32Count ; i++ )
		{
			m_TData[i].Packing( &pBuffer[i32Size], i32WriteSize );
			i32Size += i32WriteSize;
		}
		return true;
	}
	virtual bool			UnPacking( char* pBuffer, int& i32Size )
	{
		if( N > 1 )
		{
			if( false == _UnPackingSize( pBuffer, m_i32Count, N, i32Size ) )	return false;
			if( N < m_i32Count )												return false;
		}
		else
		{
			m_i32Count	= 1;
		}
		int i32WriteSize;
		for( int i = 0 ; i < m_i32Count ; i++ )
		{
			m_TData[i].UnPacking( &pBuffer[i32Size], i32WriteSize );
			i32Size += i32WriteSize;
		}
		return true;
	}
};

template <typename T, int N>
class S2MOString : public S2MOVariable<T,N>
{

public:
	S2MOString()			{}
	virtual ~S2MOString()	{}

	virtual void			Copy( const S2MOPackable* pOrg )
	{
		S2MOVariable::Copy( pOrg );
		m_TData[ m_i32Count ] = '\0';
	}
	
	virtual bool			UnPacking( char* pBuffer, int& i32Size )
	{
		if( false == S2MOVariable::UnPacking( pBuffer, i32Size ) )						return false;
		m_TData[ m_i32Count ] = '\0';

		return true;
	}

	virtual void			GetValue( T* strData, const int i32CountofBuffer, int* pi32Count = NULL )
	{
		int i32CopyCount = m_i32Count;
		if( i32CountofBuffer < (m_i32Count+1) )
		{
#ifdef _DEBUG
			_ASSERT( false );
#else
			i32CopyCount = i32CountofBuffer-1;
#endif
		}
		if( pi32Count )
		{
			*pi32Count = i32CopyCount;
		}
		if( 0 < i32CopyCount )
		{
			memcpy( strData, m_TData, sizeof(T)*i32CopyCount );
		}
		strData[ i32CopyCount ] = '\0';
	}

	void					SetValue( const T* pTData )
	{
		int i32Count = N-2;
		m_i32Count = 0;
		while( '\0' != pTData[ m_i32Count ] )
		{
			m_TData[ m_i32Count ] = pTData[ m_i32Count ];
			m_i32Count++;
			if( i32Count < m_i32Count ) break;
		}
		m_TData[ m_i32Count ] = '\0';
	}

	virtual void			operator=( const T* strData )
	{
		SetValue( strData );
	}
};

template <int N>
class S2MOStringA : public S2MOString<char,N>
{

public:
	S2MOStringA()			{}
	virtual ~S2MOStringA()	{}

	virtual S2MOStringA&	operator=( const S2MOStringA& Org )
	{
		S2MOString::Copy( &Org );
		return *this;
	}
	virtual void			operator=( const char* strData )
	{
		S2MOString::operator=( strData );
	}
	virtual void			operator=( const TCHAR* strData )
	{
		S2MOString::operator=( strData );
	}
};

template <int N>
class S2MOStringW : public S2MOString<CHAR16,N>
{

public:
	S2MOStringW()			{}
	virtual ~S2MOStringW()	{}

	virtual S2MOStringW&	operator=( const S2MOStringW& Org )
	{
		S2MOString::Copy( &Org );
		return *this;
	}
	virtual void			operator=( const CHAR16* strData )
	{
		S2MOString::operator=( strData );
	}
};

class S2MOStructBase : public S2MOPackable
{
	S2MOPackable*			m_pS2Packable;

protected:
	void					_SetValue( S2MOPackable& pData )
	{
		if( m_pNext )
		{
			m_pNext->SetNext( &pData );
			m_pNext			= m_pNext->GetNext();
		}
		else
		{
			m_pS2Packable	= &pData;
			m_pNext			= m_pS2Packable;
		}
	}

public:
	S2MOStructBase()
	{
		m_pNext = NULL;
	}	
	virtual ~S2MOStructBase()	{}

	virtual void			Copy( const S2MOPackable* pOrg )
	{
		S2MOPackable* pS2Dis	= m_pS2Packable;
		S2MOPackable* pS2Src	= ((S2MOStructBase*)pOrg)->m_pS2Packable;
		while( pS2Src && pS2Dis )
		{
			pS2Dis->Copy( pS2Src );

			pS2Src = pS2Src->GetNext();
			pS2Dis = pS2Dis->GetNext();
		}
	}

	virtual bool			Packing( char* pBuffer, int& i32Size )
	{
		S2MOPackable* pS2Packable = m_pS2Packable;
		i32Size = 0;
		int i32ReturnSize;
		while( pS2Packable )
		{
			if( false == pS2Packable->Packing( &pBuffer[ i32Size ], i32ReturnSize ) )	return false;			
			i32Size += i32ReturnSize;

			pS2Packable = pS2Packable->GetNext();
		}
		return true;
	}
	virtual bool			UnPacking( char* pBuffer, int& i32Size )
	{
		S2MOPackable* pS2Packable = m_pS2Packable;
		i32Size = 0;
		int i32ReturnSize;
		while( pS2Packable )
		{
			if( false == pS2Packable->UnPacking( &pBuffer[ i32Size ], i32ReturnSize ) )	return false;			
			i32Size += i32ReturnSize;

			pS2Packable = pS2Packable->GetNext();
		}
		return true;
	}
	virtual S2MOStructBase&		operator=( const S2MOStructBase& Org )
	{
		Copy( &Org );
		return *this;
	}
};

typedef		S2MOVariable<INT8>			S2MOINT8;
typedef		S2MOVariable<UINT8>			S2MOUINT8;
typedef		S2MOVariable<INT16>			S2MOINT16;
typedef		S2MOVariable<UINT16>		S2MOUINT16;
typedef		S2MOVariable<INT32>			S2MOINT32;
typedef		S2MOVariable<UINT32>		S2MOUINT32;
typedef		S2MOVariable<INT64>			S2MOINT64;
typedef		S2MOVariable<UINT64>		S2MOUINT64;

#endif