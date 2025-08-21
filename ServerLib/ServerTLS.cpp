#include "pch.h"
#include "ServerTLS.h"

thread_local ThreadLocalBufferPool* LSendBufferPool = nullptr;
thread_local DBConnector* LDBConnector = nullptr;