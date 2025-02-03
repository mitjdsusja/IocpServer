#include "pch.h"
#include "ServerTLS.h"

thread_local BufferPool* LSendBufferPool = nullptr;
thread_local DBConnector* LDBConnector = nullptr;