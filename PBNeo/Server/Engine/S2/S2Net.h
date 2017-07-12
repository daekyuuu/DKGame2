#ifndef __S2_NET_H__
#define __S2_NET_H__

typedef UINT16					PACKET_SIZE;
typedef UINT16					PACKET_PROTOCOL;

#define S2_PACKET_MAX_SIZE		8192

#define S2_PACKET_HEAD_SIZE		(sizeof(PACKET_SIZE) + sizeof(PACKET_PROTOCOL) + sizeof(INT16))	// sizeof(PACKET_SIZE) + sizeof(PACKET_PROTOCOL)
#define S2_PACKET_BUFFER_SIZE	8192 * 3

namespace S2Net
{
	S2_EXPORT_BASE BOOL			Create(void);
	S2_EXPORT_BASE void			Destroy(void);			

	//S2_EXPORT_BASE BOOL			GetMyAddress( TCHAR * pAdress );

	S2_EXPORT_BASE void			DestroySocket( SOCKET hSocket );

	//S2_EXPORT_BASE char*		GetMyAddressList( INT32 * pOutCount);
	//S2_EXPORT_BASE void			GetName( INT32 Socket, SOCKADDR_IN * plocaladdr ); 
	//I3_EXPORT_BASE INT32		GetMACAddress(char* pMAC,INT32 nNICCount = 1);
	//S2_EXPORT_BASE INT32		GetMACAddress(UINT64 * pMAC,INT32 nNICCount = 1);
	//S2_EXPORT_BASE INT32		GetError( char * pString );
	S2_EXPORT_BASE UINT32		GetIPToLong( char* strIP );
	S2_EXPORT_BASE UINT32		GetIPToLong( wchar_t* wstrIP );
	S2_EXPORT_BASE void			GetIPToLongA( UINT32 ui32IP, char* strIP );
	S2_EXPORT_BASE void			GetIPToLongW( UINT32 ui32IP, wchar_t* wstrIP );
};

#endif