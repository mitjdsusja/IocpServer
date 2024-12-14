#include "pch.h"
#include "ServerTLS.h"

thread_local BufferPool* LSendBufferPool = nullptr;
thread_local BufferPool* LBufferPool = nullptr;