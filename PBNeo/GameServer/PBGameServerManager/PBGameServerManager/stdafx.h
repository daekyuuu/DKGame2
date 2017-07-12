#pragma once

#include "targetver.h"

#include <winsock2.h>
#include <mswsock.h>       
#include <windows.h>
#include <process.h>
#include <stdio.h>
#include <tchar.h>
#include <string>
#include <sstream>
#include <vector>
#include <queue>

#pragma comment(lib,"ws2_32")		// Standard socket API.
#pragma comment(lib,"mswsock")		// AcceptEx, DiscoonectEx

// Constants and enums

const UINT32 WORKER_THREAD_COUNT = 1;

const UINT32 DEFAULT_LISTEN_PORT = 36100;
const UINT32 DEFAULT_LISTEN_QUEUE_SIZE = 8;
const UINT32 MAX_CONCURRENT_GAMESERVER_CONNECTIONS = 1;

enum class CompletionKeyCode
{
	COMPLETION_KEY_NONE = 0,
	COMPLETION_KEY_SHUTDOWN = 1,
	COMPLETION_KEY_IO = 2,
};