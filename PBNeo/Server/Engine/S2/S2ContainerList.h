#ifndef __S2_CONTAINER_LIST_H__
#define __S2_CONTAINER_LIST_H__

enum S2_EXPORT_BASE S2_CONTAINER_TYPE
{
	S2_CONTAINER_TYPE_USE,					// Use List 로 사용
	S2_CONTAINER_TYPE_FREE,					// Free List 로 사용
};

enum S2_CONTAINER_STATE
{
	S2_CONTAINER_STATE_NONE,
	S2_CONTAINER_STATE_8,
	S2_CONTAINER_STATE_16,
	S2_CONTAINER_STATE_32,
};

class S2_EXPORT_BASE S2ContainerList
{
	S2_CONTAINER_STATE		m_eState;
	S2_CONTAINER_TYPE		m_eType;

	UINT32					m_ui32Count;
	UINT32					m_ui32StartIdx;
	UINT8*					m_ui8Buffer;
	UINT16*					m_ui16Buffer;
	UINT32*					m_ui32Buffer;

	BOOL					_EmptyBuffer();
	BOOL					_FillBuffer();

public:
	S2ContainerList();
	virtual ~S2ContainerList();

	BOOL					Create( UINT32 ui32Count, S2_CONTAINER_TYPE eType );
	void					Destroy();

	BOOL					ResetBuffer();

	BOOL					PopContainerList( UINT32* pui32Idx );
	void					PushContainerList( UINT32 ui32Idx );

	UINT32					GetStartIdx()					{	return m_ui32StartIdx;		}
	UINT8*					GetBuffer8()					{	return m_ui8Buffer;			}
	UINT16*					GetBuffer16()					{	return m_ui16Buffer;		}
	UINT32*					GetBuffer32()					{	return m_ui32Buffer;		}
};

#endif