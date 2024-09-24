#include "pch.h"
#include "BufferPool.h"

BufferPool::BufferPool() : _bufferCount(buffer_count){

	for (int32 i = 0; i < buffer_count; i++) {
		_buffers.push_back(new SendBuffer(buffer_size));
	}
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
		return new SendBuffer(buffer_size);
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
