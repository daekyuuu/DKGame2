#include "stdafx.h"
#include "DisconnectExCaller.h"

DisconnectExCaller* DisconnectExCaller::Instance = nullptr;

DisconnectExCaller* DisconnectExCaller::Get()
{
	if (!Instance)
	{
		Instance = new DisconnectExCaller();
	}
	return Instance;
}

DisconnectExCaller::DisconnectExCaller()
{
	mySocket = NULL;
	myDisconnectEx = NULL;
}

DisconnectExCaller::~DisconnectExCaller()
{
	if (mySocket)
	{
		closesocket(mySocket);
		mySocket = NULL;
	}

	if (myDisconnectEx)
	{
		myDisconnectEx = NULL;
	}
}

BOOL DisconnectExCaller::DisconnectEx(SOCKET s, LPOVERLAPPED lpOverlapped, DWORD dwFlags, DWORD dwReserved)
{
	if (mySocket == NULL)
	{
		mySocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (NULL == mySocket)
		{
			return false;
		}
	}

	if (myDisconnectEx == NULL)
	{
		DWORD dwBytes = 0;
		GUID GuidDisconnectEx = WSAID_DISCONNECTEX;
		if (SOCKET_ERROR == WSAIoctl(mySocket,
			SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidDisconnectEx, sizeof(GuidDisconnectEx),
			&myDisconnectEx, sizeof(myDisconnectEx), &dwBytes,
			NULL, NULL))
		{
			mySocket = NULL;
			myDisconnectEx = NULL;
			return false;
		}
	}

	return myDisconnectEx(s, lpOverlapped, dwFlags, dwReserved);
}
