#include "pch.h"
#include "BufferPool.h"


BufferPool::BufferPool(){

	for (int32 i = 0; i < BUFFER_COUNT; i++) {
		
		Buffer* buffer = new Buffer(BUFFER_SIZE);
		buffer->SetOwnerBufferPool(this);
		_buffers.push_back(buffer);
	}

	_bufferCount = BUFFER_COUNT;
	_remainedCount = BUFFER_COUNT;
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
		
		Buffer* buffer = new Buffer(BUFFER_SIZE);
		buffer->SetOwnerBufferPool(this);

		return buffer;
	}
	Buffer* buffer = _buffers.back();
	_buffers.pop_back();
	_remainedCount--;

	//spdlog::info("thread({})BufferPool::Pop() TotalBufferCount : {} , RemainedBufferCount : {}", hash<thread::id>{}(this_thread::get_id()), _bufferCount, _remainedCount);
	//cout << this_thread::get_id() << "<POP> Remained Buffer : " << _remainedCount << endl;

	return buffer;
}

void BufferPool::Push(Buffer* buffer){

	buffer->Clear();
	_buffers.push_back(buffer);
	_remainedCount++;

	//spdlog::info("thread({})BufferPool::Push() TotalBufferCount : {} , RemainedBufferCount : {}", hash<thread::id>{}(this_thread::get_id()), _bufferCount, _remainedCount);
	//cout << this_thread::get_id() << "<PUSH> Remained Buffer : " << _remainedCount << endl;
}

LockBufferPool::LockBufferPool(){

	for (int32 i = 0; i < BUFFER_COUNT; i++) {

		Buffer* buffer = new Buffer(BUFFER_SIZE);
		buffer->SetOwnerBufferPool(this);
		_buffers.push_back(buffer);
	}
	_bufferCount = BUFFER_COUNT;
	_remainedCount = BUFFER_COUNT;
}

LockBufferPool::~LockBufferPool(){

	for (Buffer* buffer : _buffers) {
		delete buffer;
	}

	_bufferCount = 0;
}

Buffer* LockBufferPool::Pop(){

	lock_guard<mutex> lock(_buffersMutex);

	if (_buffers.empty() == true) {
		_bufferCount++;
		spdlog::info("Total Buffer Count : {}", _bufferCount);
		//cout << "Total Buffer Count : " << _bufferCount << endl;
		
		Buffer* buffer = new Buffer(BUFFER_SIZE);
		buffer->SetOwnerBufferPool(this);

		return buffer;
	}
	Buffer* buffer = _buffers.back();
	_buffers.pop_back();
	_remainedCount--;

	//spdlog::info("Pop - Remained BUFFER Count : {}", _remainedCount);
	//cout << this_thread::get_id() << "<POP> Remained Buffer : " << _remainedCount << endl;

	return buffer;
}

void LockBufferPool::Push(Buffer* buffer){

	lock_guard<mutex> lock(_buffersMutex);

	buffer->Clear();
	_buffers.push_back(buffer);
	_remainedCount++;

	//spdlog::info("Push - Remained BUFFER Count : {}", _remainedCount);

	//cout << this_thread::get_id() << "<PUSH> Remained Buffer : " << _remainedCount << endl;
}

PushLockBufferPool::PushLockBufferPool(){

	for (int32 i = 0; i < BUFFER_COUNT; i++) {

		Buffer* buffer = new Buffer(BUFFER_SIZE);
		buffer->SetOwnerBufferPool(this);
		_buffers.push_back(buffer);
	}

	_bufferCount = BUFFER_COUNT;
	_remainedCount = BUFFER_COUNT;
}

PushLockBufferPool::~PushLockBufferPool(){

	for (Buffer* buffer : _buffers) {
		delete buffer;
	}

	_bufferCount = 0;
}

Buffer* PushLockBufferPool::Pop(){

	if (_buffers.empty() == true) {
		_bufferCount++;
		cout << "Total Buffer Count : " << _bufferCount << endl;

		Buffer* buffer = new Buffer(BUFFER_SIZE);
		buffer->SetOwnerBufferPool(this);

		return buffer;
	}
	Buffer* buffer = _buffers.back();
	_buffers.pop_back();
	_remainedCount--;

	//spdlog::info("thread({})BufferPool::Pop() TotalBufferCount : {} , RemainedBufferCount : {}", hash<thread::id>{}(this_thread::get_id()), _bufferCount, _remainedCount);
	//cout << this_thread::get_id() << "<POP> Remained Buffer : " << _remainedCount << endl;

	return buffer;
}

void PushLockBufferPool::Push(Buffer* buffer){

	lock_guard<mutex> lock(_buffersMutex);

	buffer->Clear();
	_buffers.push_back(buffer);
	_remainedCount++;
}
