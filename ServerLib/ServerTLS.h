#pragma once
#include "BufferPool.h"
#include "DBConnector.h"

extern thread_local PushLockBufferPool* LSendBufferPool;
extern thread_local DBConnector* LDBConnector;