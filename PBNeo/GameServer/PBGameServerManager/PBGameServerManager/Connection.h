#pragma once
#include "Net\S2MOPacket.h"
#include "DisconnectExCaller.h"

class Connection;

// Create subclass of this class and specify how each connection behave
class ConnectionManager
{
public:
	ConnectionManager(SOCKET Listener, HANDLE IoPort);
	~ConnectionManager();

protected: // Interface to child class

	// Methods to be called
	void SendMsg(S2MOPacketBase* pPacket);
	void SpawnGameServer();

	// Non-public Event
	virtual void OnMsgReceived(T_PACKET_PROTOCOL	Protocol, char* Buffer, UINT32 BufferSize) = 0;

public: // Interface to Connection

	virtual void OnConnAccepted() = 0;
	void OnMsgReceived(char* Buffer, UINT32 BufferSize);
	virtual void OnConnReset() = 0;

private:

	Connection* myConn;

};

enum class IoKeyType
{
	None,
	Accept,
	Recv,
	Send,
	Reset,
};

class IoKey :
	public OVERLAPPED
{
public:
	IoKey();
	~IoKey();

	Connection* myOwningConnection;
	IoKeyType myKeyType;
};

class Connection
{
public:
	Connection(SOCKET Listener, HANDLE IoPort, ConnectionManager* Manager);
	~Connection();

private:
	void StartAccept();
	void OnAcceptDone();
	void StartRecvMsg();
	void OnRecvDone(DWORD NumTransfered);
	void StartSendMsg();
	void OnSendDone(DWORD NumTransfered);
	void StartReset();
	void OnResetDone();

public: // Interface to worker thread
	void OnIoError();
	void OnAcceptCompleted();
	void OnSendCompleted(DWORD NumTransfered);
	void OnRecvCompleted(DWORD NumTransfered);
	void OnResetCompleted();

public: // Interface to ConnectionManager
	void SendMsg(char* Buffer, UINT32 BufferSize);

private: // External references

	// Persistent across program lifetime
	HANDLE myIoPort;
	SOCKET myListener;

	// Owner of this connection
	ConnectionManager* myManager;

private: // Owned resource

	SOCKET mySock;

	IoKey myAcceptKey;
	IoKey myRecvKey;
	IoKey mySendKey;
	IoKey myDisconnectKey;

private: // State

	enum class State
	{
		None,
		Accepting,
		Ready,
		Resetting,
	}; 
	State myState;

private:  // Owned buffers

	void ClearBuffers();

	static const UINT32 ADDRSIZE = (sizeof(struct sockaddr_in) + 16);
	BYTE myAddrBlock[ADDRSIZE * 2];

	UINT32 myValidRecvBufferSize;
	char myRecvBuffer[S2MO_PACKET_CONTENT_SIZE];
	
	bool myIsSending;
	UINT32 mySentByteNum;
	std::queue<std::vector<char>> mySendBufferQueue;
};