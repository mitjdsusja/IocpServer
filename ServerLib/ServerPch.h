#pragma once

#include <iostream> 
#include <vector>
#include <mutex>
#include <queue>
#include <atomic>
#include <set>
#include <cassert>

using namespace std;

#include "WinSock2.h"
#include "WS2tcpip.h"
#include "MSWSock.h"
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")

#include "types.h"
#include "Mecro.h"

#include "ServerGlobal.h"
#include "ErrorHandler.h"
#include "NetAddress.h"
#include "SocketManager.h"
#include "OverlappedEvent.h"
#include "SendBuffer.h"
#include "RecvBuffer.h"
#include "Session.h"