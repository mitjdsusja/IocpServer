#include "pch.h"
#include "ServerTLS.h"

thread_local PushLockBufferPool* LSendBufferPool = nullptr;
thread_local DBConnector* LDBConnector = nullptr;