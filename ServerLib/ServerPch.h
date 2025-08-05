#pragma once

#include <iostream> 
#include <string>
#include <vector>
#include <mutex>
#include <queue>
#include <atomic>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <cassert>
#include <future>

#include "spdlog/spdlog.h"

using namespace std;

#include "WinSock2.h"
#include "WS2tcpip.h"
#include "MSWSock.h"
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")

#include "types.h"
#include "Mecro.h"

#include "ServerTLS.h"
#include "ServerGlobal.h"
#include "ErrorHandler.h"
#include "NetAddress.h"
#include "SocketManager.h"
#include "OverlappedEvent.h"
#include "Buffer.h"
#include "RecvBuffer.h"
