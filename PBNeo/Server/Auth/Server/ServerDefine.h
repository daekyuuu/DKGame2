#ifndef __SERVER_DEFINE_H__
#define __SERVER_DEFINE_H__

#define THIS_SERVER_TYPE			SERVER_TYPE_AUTH


struct IOCP_RING_POP
{
	char					m_pBuffer[ S2_PACKET_MAX_SIZE ]; 
};

struct RING_LOGIN_PUSH
{
	INT32					m_i32SessionIdx;
	TCHAR					m_strID[ NET_ID_LENGTH ];
	TCHAR					m_strPW[ NET_PW_LENGTH ];
};

struct RING_LOGIN_POP
{
	T_RESULT				m_TResult;
	INT32					m_i32SessionIdx;
	T_UID					m_TUID;
	//TCHAR					m_strQuery[ S2_STRING_COUNT ];
};

#endif
