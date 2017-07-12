#ifndef __USER_CONTAINER_H__
#define __USER_CONTAINER_H__

class CSession;

struct ROOM_LIST
{
	UINT32					m_ui32Idx;

	UINT32					m_ui32RoomIdx;
	T_MapID					m_TMapID;

	// InsertUser �� ���Ǵ� Copy ����Ʈ
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
	// �ѹ��� �ʹ� ���� �迭�� �����ϸ� �޸𸮰� ���� �� �� �־� 2�� �����ͷ� ���� �մϴ�.
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