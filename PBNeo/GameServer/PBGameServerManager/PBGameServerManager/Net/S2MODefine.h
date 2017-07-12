#ifndef __S2MO_DEFINE_H__
#define __S2MO_DEFINE_H__

typedef		signed char					INT8;
typedef		unsigned char				UINT8;
typedef		short int					INT16;
typedef		unsigned short int			UINT16;
typedef		signed int					INT32;
typedef		unsigned int				UINT32;
typedef		signed __int64				INT64;
typedef		unsigned __int64			UINT64;
typedef		char						CHAR8;
#ifndef USE_UNREAL4
typedef		wchar_t						CHAR16;
#else
typedef		char16_t					CHAR16;
#endif

typedef		short int					T_PACKET_SIZE;
typedef		short int					T_PACKET_PROTOCOL;
typedef		short int					T_PACKET_RANDSEED;

typedef		int							T_RESULT;

typedef		unsigned __int64			T_UID;
typedef		int							T_SESSIONID;

#define S2MO_PACKET_CONTENT_SIZE		8192
#define S2MO_IS_SUCCESS(result)			(0<=result)
#define S2MO_IS_FAIL(result)			(0>result)

#endif