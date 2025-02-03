#pragma once
#include "BufferPool.h"
#include "DBConnector.h"

extern thread_local BufferPool* LSendBufferPool;
extern thread_local DBConnector* LDBConnector;