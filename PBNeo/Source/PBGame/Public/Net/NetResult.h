#ifndef __NET_RESULT_H__
#define __NET_RESULT_H__

enum NET_RESULT
{
	NET_RESULT_SUCCESS								= 0,

	NET_RESULT_SUCCESS_FIRST_CONNECT,




	// Error Message
	NET_RESULT_ERROR								= 0x80000000,
	NET_RESULT_ERROR1								= 0x80000001,

	NET_RESULT_ERROR_DB,
	NET_RESULT_ERROR_SYSTEM,						// 시스템 적인 에러
	NET_RESULT_ERROR_DUPLICATION,					// 중복처리
	NET_RESULT_ERROR_PACKET,						// 받은 패킷의 에러

	NET_RESULT_ERROR_CONNECT_FAIL,
	NET_RESULT_ERROR_DISCONNECT,

	NET_RESULT_ERROR_LOGIN_DUPLICATION,				// 중복 로그인
		
};

#endif
