#include "stdafx.h"
#include "Connection.h"
#include "GameServerConnManager.h"

// Signaled by ConsoleCtrlHandler on CTRL-C.
HANDLE StopEvent = 0;
BOOL WINAPI ConsoleCtrlHandler(DWORD Ctrl)
{
	switch (Ctrl)
	{
	case CTRL_C_EVENT:
	case CTRL_CLOSE_EVENT:
		SetEvent(StopEvent);
		return TRUE;
	default:
		return FALSE;
	}
}

// Worker thread procedure.
unsigned int __stdcall HandleIoCompletions(void* Iocp)
{
	for (;;)
	{
		BOOL Status = 0;
		DWORD NumTransferred = 0;
		ULONG_PTR CompKey = (ULONG_PTR)CompletionKeyCode::COMPLETION_KEY_NONE;
		LPOVERLAPPED pOverlapped = 0;

		Status = GetQueuedCompletionStatus(reinterpret_cast<HANDLE>(Iocp),
			&NumTransferred, &CompKey, &pOverlapped, INFINITE);

		IoKey* pIoKey = reinterpret_cast<IoKey*>(pOverlapped);
		if (FALSE == Status)
		{
			pIoKey->myOwningConnection->OnIoError();
		}
		else if ((ULONG_PTR)CompletionKeyCode::COMPLETION_KEY_IO == CompKey)
		{
			switch (pIoKey->myKeyType)
			{

			case IoKeyType::Accept:
			{
				pIoKey->myOwningConnection->OnAcceptCompleted();
				break;
			}

			case IoKeyType::Recv:
			{
				pIoKey->myOwningConnection->OnRecvCompleted(NumTransferred);
				break;
			}

			case IoKeyType::Send:
			{
				pIoKey->myOwningConnection->OnSendCompleted(NumTransferred);
				break;
			}

			case IoKeyType::Reset:
			{
				pIoKey->myOwningConnection->OnResetCompleted();
				break;
			}

			default:
			{
				break;
			}

			}
		}
		else if ((ULONG_PTR)CompletionKeyCode::COMPLETION_KEY_SHUTDOWN == CompKey)
		{
			break;
		}
	}
	return 0;
}

int main()
{
	WSADATA Wsa = { 0 };
	WSAStartup(MAKEWORD(2, 2), &Wsa);

	HANDLE IoPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, WORKER_THREAD_COUNT);

	// Associate a Listener socket with the I/O Completion Port and start listening
	SOCKET GameServerListener = WSASocket(PF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	struct sockaddr_in Addr = { 0 };
	Addr.sin_family = AF_INET;
	Addr.sin_addr.S_un.S_addr = INADDR_ANY;
	Addr.sin_port = htons(DEFAULT_LISTEN_PORT);
	bind(GameServerListener, (struct sockaddr*)&Addr, sizeof(struct sockaddr_in));
	CreateIoCompletionPort((HANDLE)GameServerListener, IoPort, (ULONG_PTR)CompletionKeyCode::COMPLETION_KEY_IO, 0);
	listen(GameServerListener, DEFAULT_LISTEN_QUEUE_SIZE);

	// Each connection manager will spawn and manage a game server
	GameServerConnManager* GameServerConnections[MAX_CONCURRENT_GAMESERVER_CONNECTIONS] = { 0 };
	for (size_t i = 0; i < MAX_CONCURRENT_GAMESERVER_CONNECTIONS; i++)
	{
		GameServerConnections[i] = new GameServerConnManager(GameServerListener, IoPort);
	}

	HANDLE Workers[WORKER_THREAD_COUNT] = { 0 };
	unsigned int WorkerIds[WORKER_THREAD_COUNT] = { 0 };
	for (size_t i = 0; i < WORKER_THREAD_COUNT; i++)
	{
		Workers[i] = (HANDLE)_beginthreadex(0, 0, HandleIoCompletions, IoPort, 0, WorkerIds + i);
	}

	// Temp test code : to remove very soon
	{
		Sleep(5000);
		for (int round = 1; round <= 3; ++round)
		{
			for (size_t i = 0; i < MAX_CONCURRENT_GAMESERVER_CONNECTIONS; i++)
			{
				GameServerConnections[i]->LoadMap(110000 + 10 * round);
			}

			Sleep(15000);
		}
	}

	// Wait until Ctrl-C is pressed to shut down process
	StopEvent = CreateEvent(0, FALSE, FALSE, 0);
	SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE);
	printf("PBGameServerManager: Press CTRL-C To graciously shut down. \n");
	WaitForSingleObject(StopEvent, INFINITE);
	SetConsoleCtrlHandler(NULL, FALSE);
	CloseHandle(StopEvent);

	// Ask all game server to shut down and wait until they are all done
	for (;;)
	{
		for (size_t i = 0; i < MAX_CONCURRENT_GAMESERVER_CONNECTIONS; i++)
		{
			GameServerConnections[i]->Shutdown();
		}

		bool SomeGameServerNotYetShutdown = false;
		for (size_t i = 0; i < MAX_CONCURRENT_GAMESERVER_CONNECTIONS; i++)
		{
			if (!GameServerConnections[i]->ReadyToBeDestroyed())
			{
				SomeGameServerNotYetShutdown = true;
			}
		}

		if (SomeGameServerNotYetShutdown)
		{
			// Retry every 1 sec
			Sleep(1000);
		}
		else
		{
			break;
		}
	}

	for (size_t i = 0; i < WORKER_THREAD_COUNT; i++)
	{
		PostQueuedCompletionStatus(IoPort, 0, (ULONG_PTR)CompletionKeyCode::COMPLETION_KEY_SHUTDOWN, 0);
	}
	WaitForMultipleObjects(WORKER_THREAD_COUNT, Workers, TRUE, INFINITE);
	for (size_t i = 0; i < WORKER_THREAD_COUNT; i++)
	{
		CloseHandle(Workers[i]);
	}

	for (size_t i = 0; i < MAX_CONCURRENT_GAMESERVER_CONNECTIONS; i++)
	{
		delete(GameServerConnections[i]);
	}

	shutdown(GameServerListener, SD_BOTH);
	closesocket(GameServerListener);

	CloseHandle(IoPort);

	WSACleanup();

    return 0;
}