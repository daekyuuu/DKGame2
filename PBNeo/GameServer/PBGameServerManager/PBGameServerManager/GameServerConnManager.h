#pragma once
#include "Connection.h"

class GameServerConnManager
	: public ConnectionManager
{
public:

	GameServerConnManager(SOCKET Listener, HANDLE IoPort);
	~GameServerConnManager();

	void OnConnAccepted() override;
	void OnMsgReceived(T_PACKET_PROTOCOL Protocol, char* Buffer, UINT32 BufferSize) override;
	void OnConnReset() override;

public:  // Interface to owner of instance

	void LoadMap(INT32 MapTableIndex);
	void Shutdown();
	bool ReadyToBeDestroyed();

public:  // State of the connection

	enum class State
	{
		WaitingForConnection,
		WaitingForLogin,
		Ready,
		WaitingForLoadMapAck,
		WaitingForShutdownAck,
		Closed,
	};
	State myState;

	UINT16 ConnectedServerListenPort;

};

