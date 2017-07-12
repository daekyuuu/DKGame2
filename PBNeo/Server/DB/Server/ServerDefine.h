#ifndef __SERVER_DEFINE_H__
#define __SERVER_DEFINE_H__

#define THIS_SERVER_TYPE			SERVER_TYPE_DB

enum DB_TYPE
{
	DB_TYPE_USERINFO_LOAD,
	DB_TYPE_USERINFO_SAVE,
};

struct RING_CENTER_POP
{
	char					m_pBuffer[ S2_PACKET_MAX_SIZE ]; 
};

struct RING_DB_PUSH
{
	DB_TYPE					m_DBType;

	INT32					m_i32SessionIdx;
	T_UID					m_TUID;

	USER_BASE				m_Base;
	USER_INFO				m_Info;
	USER_RECORD				m_Record;
	USER_EQUIPMENT			m_Eequipment;
};

struct RING_DB_POP
{
	DB_TYPE					m_DBType;

	T_RESULT				m_TResult;

	INT32					m_i32SessionIdx;
	T_UID					m_TUID;

	USER_BASE				m_Base;
	USER_INFO				m_Info;
	USER_RECORD				m_RecordTotal;
	USER_RECORD				m_RecordSeason;
	USER_EQUIPMENT			m_Eequipment;

};

#endif
