#include "stdafx.h"
#include "Connection.h"
#include "Assert.h"

ConnectionManager::ConnectionManager(SOCKET Listener, HANDLE IoPort)
{
	myConn = new Connection(Listener, IoPort, this);
}

ConnectionManager::~ConnectionManager()
{
	delete myConn;
}

void ConnectionManager::SendMsg(S2MOPacketBase* pPacket)
{
	char pBuffer[S2MO_PACKET_CONTENT_SIZE];
	pPacket->Packing_T(pBuffer);
	myConn->SendMsg(pBuffer, pPacket->GetPacketSize());
}

void ConnectionManager::SpawnGameServer()
{
	// initialize window flags
	UINT32 dwFlags = 0;
	UINT16 ShowWindowFlags = SW_HIDE;

	// initialize startup info
	STARTUPINFO StartupInfo = {
		sizeof(STARTUPINFO),
		NULL, NULL, NULL,
		(::DWORD)CW_USEDEFAULT,
		(::DWORD)CW_USEDEFAULT,
		(::DWORD)CW_USEDEFAULT,
		(::DWORD)CW_USEDEFAULT,
		(::DWORD)0, (::DWORD)0, (::DWORD)0,
		(::DWORD)dwFlags,
		ShowWindowFlags,
		0, NULL,
		HANDLE(nullptr),
		HANDLE(nullptr),
		HANDLE(nullptr)
	};

	// ProcessInfo
	PROCESS_INFORMATION ProcessInfo;
	ZeroMemory(&ProcessInfo, sizeof(ProcessInfo));

	TCHAR SpawnProcessCommand[MAX_PATH] = TEXT("PBNeoServer.exe ?game=BOMB ?SpawnedByGameServerManager -log");

	// Start the child process. 
	CreateProcess(NULL,						// No module name (use command line)
		SpawnProcessCommand,			// Command line
		NULL,											// Process handle not inheritable
		NULL,											// Thread handle not inheritable
		FALSE,											// Set handle inheritance to FALSE
		0,													// No creation flags
		NULL,											// Use parent's environment block
		NULL,											// Use parent's starting directory 
		&StartupInfo,								// Pointer to STARTUPINFO structure
		&ProcessInfo);								// Pointer to PROCESS_INFORMATION structure	

	// We will not communicate with the spawned process via its process/thread handle
	CloseHandle(ProcessInfo.hProcess);
	CloseHandle(ProcessInfo.hThread);
}

void ConnectionManager::OnMsgReceived(char* Buffer, UINT32 BufferSize)
{
	S2MOPacketBase PacketBase;
	PacketBase.UnPacking_Head(Buffer);
	OnMsgReceived(PacketBase.GetProtocol(), Buffer, BufferSize);
}

IoKey::IoKey()
{
	Internal = 0;
	InternalHigh = 0;
	Offset = 0;
	OffsetHigh = 0;
	hEvent = 0;

	myOwningConnection = nullptr;
	myKeyType = IoKeyType::None;
}

IoKey::~IoKey()
{

}

Connection::Connection(SOCKET Listener, HANDLE IoPort, ConnectionManager* Manager)
{
	myIoPort = IoPort;
	myListener = Listener;
	myManager = Manager;

	mySock = WSASocket(PF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0,
		WSA_FLAG_OVERLAPPED);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(mySock), IoPort,
		(ULONG_PTR)CompletionKeyCode::COMPLETION_KEY_IO, 0);

	myAcceptKey.myOwningConnection = this;
	myAcceptKey.myKeyType = IoKeyType::Accept;

	myRecvKey.myOwningConnection = this;
	myRecvKey.myKeyType = IoKeyType::Recv;

	mySendKey.myOwningConnection = this;
	mySendKey.myKeyType = IoKeyType::Send;

	myDisconnectKey.myOwningConnection = this;
	myDisconnectKey.myKeyType = IoKeyType::Reset;

	myState = State::None;

	ClearBuffers();
	StartAccept();
}

Connection::~Connection()
{
	myState = State::None;

	shutdown(mySock, SD_BOTH);
	closesocket(mySock);
}

void Connection::StartAccept()
{
	myState = State::Accepting;

	DWORD ReceiveLen = 0; // This gets thrown away, but must be passed.
	AcceptEx(myListener, mySock, myAddrBlock, 0, ADDRSIZE,
		ADDRSIZE, &ReceiveLen, &myAcceptKey);
}

void Connection::OnAcceptDone()
{
	setsockopt(mySock, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT,
		(char*)&myListener, sizeof(SOCKET));

	// Notify manager we have a new connection
	myState = State::Ready;
	myManager->OnConnAccepted();

	// Try to recv more message
	StartRecvMsg();

	// Try to send more message
	StartSendMsg();
}

void Connection::StartRecvMsg()
{
	if (myState != State::Ready)
		return;

	myValidRecvBufferSize = 0;

	ReadFile((HANDLE)mySock, myRecvBuffer, sizeof(T_PACKET_SIZE), NULL, &myRecvKey);
	return;
}

void Connection::OnRecvDone(DWORD NumTransfered)
{
	if (myState != State::Ready)
		return;

	// More data is received
	myValidRecvBufferSize += NumTransfered;

	// Try to first receive the message size variable
	if (myValidRecvBufferSize < sizeof(T_PACKET_SIZE))
	{
		ReadFile((HANDLE)mySock, &myRecvBuffer[myValidRecvBufferSize], sizeof(T_PACKET_SIZE) - myValidRecvBufferSize, NULL, &myRecvKey);
		return;
	}

	// If we have not got a complete message yet, receive the remaining to-be-received bytes
	T_PACKET_SIZE MessageSize = *((T_PACKET_SIZE*)myRecvBuffer);
	T_PACKET_SIZE BytesToRecv = min(MessageSize, S2MO_PACKET_CONTENT_SIZE);
	if (myValidRecvBufferSize < (UINT32)(BytesToRecv))
	{
		ReadFile((HANDLE)mySock, &myRecvBuffer[myValidRecvBufferSize], BytesToRecv - myValidRecvBufferSize, NULL, &myRecvKey);
		return;
	}

	// We have a complete message! Yeah!
	assert(myValidRecvBufferSize == BytesToRecv);

	// Notify manager we have a complete message
	myManager->OnMsgReceived(myRecvBuffer, myValidRecvBufferSize);

	// Try to recv more message
	StartRecvMsg();
}

void Connection::StartSendMsg()
{
	if (myState != State::Ready)
		return;

	if (mySendBufferQueue.empty())
		return;

	if (myIsSending)
		return;

	myIsSending = true;
	mySentByteNum = 0;
	WriteFile((HANDLE)mySock, mySendBufferQueue.front().data(), mySendBufferQueue.front().size(), NULL, &mySendKey);
}

void Connection::OnSendDone(DWORD NumTransfered)
{
	if (myState != State::Ready)
		return;

	mySentByteNum += NumTransfered;

	if (mySendBufferQueue.front().size() > mySentByteNum)
	{
		WriteFile((HANDLE)mySock, &mySendBufferQueue.front().data()[mySentByteNum], mySendBufferQueue.front().size() - mySentByteNum, NULL, &mySendKey);
		return;
	}

	// We have sent all the bytes in the message
	assert(mySendBufferQueue.front().size() == mySentByteNum);
	mySendBufferQueue.pop();
	mySentByteNum = 0;
	myIsSending = false;

	// Try to send more message
	StartSendMsg();
}

void Connection::StartReset()
{
	myState = State::Resetting;

	DisconnectExCaller::Get()->DisconnectEx(mySock, &myDisconnectKey, TF_REUSE_SOCKET, 0);
}

void Connection::OnResetDone()
{
	myManager->OnConnReset();

	ClearBuffers();
	StartAccept();
}

void Connection::OnIoError()
{
	StartReset();
}

void Connection::OnAcceptCompleted()
{
	OnAcceptDone();
}

void Connection::OnSendCompleted(DWORD NumTransfered)
{
	OnSendDone(NumTransfered);
}

void Connection::OnRecvCompleted(DWORD NumTransfered)
{
	OnRecvDone(NumTransfered);
}

void Connection::OnResetCompleted()
{
	OnResetDone();
}

void Connection::SendMsg(char* Buffer, UINT32 BufferSize)
{
	if (BufferSize == 0) // Ignore empty messages
		return;

	mySendBufferQueue.emplace(Buffer, Buffer + BufferSize);

	StartSendMsg();
}

void Connection::ClearBuffers()
{
	ZeroMemory(myAddrBlock, ADDRSIZE * 2);

	myValidRecvBufferSize = 0;
	ZeroMemory(myRecvBuffer, S2MO_PACKET_CONTENT_SIZE);

	myIsSending = false;
	mySentByteNum = 0;
	mySendBufferQueue.empty();
}
