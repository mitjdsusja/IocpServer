#include "pch.h"
#include "BufferPool.h"


BufferPool::BufferPool(){

	for (int32 i = 0; i < buffer_count; i++) {
		_buffers.push_back(new Buffer(buffer_size));
	}
	_bufferCount = buffer_count;
	_remainedCount = buffer_count;
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

	//cout << this_thread::get_id() << "<POP> Remained Buffer : " << _remainedCount << endl;

	return Buffer;
}

void BufferPool::Push(Buffer* buffer){

	buffer->Clear();
	_buffers.push_back(buffer);
	_remainedCount++;

	//cout << this_thread::get_id() << "<PUSH> Remained Buffer : " << _remainedCount << endl;
}

LockBufferPool::LockBufferPool(){

	for (int32 i = 0; i < buffer_count; i++) {
		_buffers.push_back(new Buffer(buffer_size));
	}
	_bufferCount = buffer_count;
	_remainedCount = buffer_count;
}

LockBufferPool::~LockBufferPool(){

	for (Buffer* buffer : _buffers) {
		delete buffer;
	}

	_bufferCount = 0;
}

Buffer* LockBufferPool::Pop(){

	lock_guard<mutex> lock(_sendQueueMutex);

	if (_buffers.empty() == true) {
		_bufferCount++;
		spdlog::info("Total Buffer Count : {}", _bufferCount);
		//cout << "Total Buffer Count : " << _bufferCount << endl;
		return new Buffer(buffer_size);
	}
	Buffer* Buffer = _buffers.back();
	_buffers.pop_back();
	_remainedCount--;

	spdlog::info("Pop - Remained BUFFER Count : {}", _remainedCount);
	//cout << this_thread::get_id() << "<POP> Remained Buffer : " << _remainedCount << endl;

	return Buffer;
}

void LockBufferPool::Push(Buffer* buffer){
	lock_guard<mutex> lock(_sendQueueMutex);

	buffer->Clear();
	_buffers.push_back(buffer);
	_remainedCount++;

	spdlog::info("Push - Remained BUFFER Count : {}", _remainedCount);
	//cout << "++REMAININF BUFFER : " << _remainedCount << endl;

	//cout << this_thread::get_id() << "<PUSH> Remained Buffer : " << _remainedCount << endl;
}
