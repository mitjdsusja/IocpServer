#pragma once

#include <iostream> 
#include <vector>

using namespace std;

#include "WinSock2.h"
#include "WS2tcpip.h"
#include "MSWSock.h"
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")

#include "types.h"
#include "ErrorHandler.h"

#include "NetAddress.h"
#include "SocketManager.h"
#include "OverlappedEvent.h"