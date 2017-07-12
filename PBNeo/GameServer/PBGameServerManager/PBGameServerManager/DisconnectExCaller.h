#pragma once

// Strangely, Win32 does not less us directly call DisconnectEx.
// This is a singleton class with the sole purpose to call DisconnectEx.
class DisconnectExCaller
{
public:

	static DisconnectExCaller* Instance;
	static DisconnectExCaller* Get();

	DisconnectExCaller();
	~DisconnectExCaller();

	BOOL DisconnectEx(SOCKET s, LPOVERLAPPED lpOverlapped, DWORD  dwFlags, DWORD  dwReserved);

private:

	SOCKET mySocket;
	LPFN_DISCONNECTEX myDisconnectEx;

};

