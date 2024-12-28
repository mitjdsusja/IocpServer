#include "pch.h"
#include "BufferPool.h"



BufferPool::BufferPool() : _bufferCount(buffer_count), _remainedCount(buffer_count){

	for (int32 i = 0; i < buffer_count; i++) {
		_buffers.push_back(new Buffer(buffer_size));
	}
}

BufferPool::~BufferPool(){

	for (Buffer* buffer : _buffers) {
		delete buffer;
	}

	_bufferCount = 0;
}

Buffer* BufferPool::Pop(){

	if (_buffers.empty() == true) {
		_bufferCount++;
		cout << "Total Buffer Count : " << _bufferCount << endl;
		return new Buffer(buffer_size);
	}
	Buffer* Buffer = _buffers.back();
	_buffers.pop_back();
	_remainedCount--;

	cout << this_thread::get_id() << "<POP> Remained Buffer : " << _remainedCount << endl;

	return Buffer;
}

void BufferPool::Push(Buffer* buffer){

	buffer->Clear();
	_buffers.push_back(buffer);
	_remainedCount++;

	cout << this_thread::get_id() << "<PUSH> Remained Buffer : " << _remainedCount << endl;
}
