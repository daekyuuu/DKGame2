#ifndef __USER_CONTAINER_H__
#define __USER_CONTAINER_H__

class CSession;

struct USER_LIST
{
	UINT32					m_ui32Idx;

	CSession*				m_pFrontSession;
	INT32					m_i32SessionIdx;

	T_UID					m_TUID;

	USER_BASE				m_UserBase;
	USER_INFO				m_UserInfo;
	USER_MONEY				m_UserMoney;
	USER_RECORD				m_UserRecordTotal;
	USER_RECORD				m_UserRecordSeason;
	USER_EQUIPMENT			m_UserEquipment;

	USER_STATE				m_eUserState;
	INT32					m_i32RoomIdx;
	INT32					m_i32SlotIdx;

	// InsertUser �� ���Ǵ� Copy ����Ʈ
	void Reset()
	{
		m_eUserState		= USER_STATE_NONE;
		m_i32RoomIdx		= 0;
		m_i32SlotIdx		= 0;
		m_UserBase.Reset();
		m_UserInfo.Reset();
		m_UserMoney.Reset();
		m_UserRecordTotal.Reset();
		m_UserRecordSeason.Reset();
		m_UserEquipment.Reset();
	}
	void InsertCopy( USER_LIST* pUser )
	{
		m_TUID				= pUser->m_TUID;
	}
};

class CUserContainer
{
	UINT32					m_ui32Count;
	// �ѹ��� �ʹ� ���� �迭�� �����ϸ� �޸𸮰� ���� �� �� �־� 2�� �����ͷ� ���� �մϴ�.
	USER_LIST**				m_ppUserList;
	S2ContainerList*		m_pContainer;

public:
	CUserContainer();
	~CUserContainer();

	BOOL					Create( UINT32 ui32Count );
	void					Destroy();

	USER_LIST*				PopEmptyUser();
	void					PushEmptyUser( USER_LIST* pUser );
};

#endif