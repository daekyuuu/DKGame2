#ifndef __USER_CONTAINER_H__
#define __USER_CONTAINER_H__

class CSession;

struct ROOM_LIST
{
	UINT32					m_ui32Idx;

	UINT32					m_ui32RoomIdx;
	T_MapID					m_TMapID;

	// InsertUser 시 사용되는 Copy 리스트
	void Reset()
	{
		m_ui32RoomIdx		= 0;
		m_TMapID			= 0;
	}
	void InsertCopy( ROOM_LIST* pUser )
	{
		m_ui32RoomIdx		= pUser->m_ui32RoomIdx;
	}
};

class CRoomContainer
{
	UINT32					m_ui32Count;
	// 한번에 너무 많은 배열을 생성하면 메모리가 부족 할 수 있어 2중 포인터로 선언 합니다.
	ROOM_LIST**				m_ppRoomList;
	S2ContainerList*		m_pContainer;

public:
	CRoomContainer();
	~CRoomContainer();

	BOOL					Create( UINT32 ui32Count );
	void					Destroy();

	ROOM_LIST*				PopEmptyUser();
	void					PushEmptyUser( ROOM_LIST* pUser );
};

#endif