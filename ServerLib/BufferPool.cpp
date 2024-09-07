#include "pch.h"
#include "BufferPool.h"

BufferPool::BufferPool() : _bufferCount(BUFFER_COUNT){

	_buffers.push_back(new SendBuffer(BUFFER_SIZE));
}

BufferPool::~BufferPool(){

	for (SendBuffer* buffer : _buffers) {
		delete buffer;
	}

	_bufferCount = 0;
}

SendBuffer* BufferPool::Pop(){

	lock_guard<mutex> _lock(_mutex);

	if (_buffers.empty() == true) {
		_bufferCount++;
		return new SendBuffer(BUFFER_SIZE);
	}

	SendBuffer* sendBuffer = _buffers.back();
	_buffers.pop_back();

	return sendBuffer;
}

void BufferPool::Push(SendBuffer* buffer){

	lock_guard<mutex> _lock(_mutex);

	buffer->Clear();
	_buffers.push_back(buffer);
}
