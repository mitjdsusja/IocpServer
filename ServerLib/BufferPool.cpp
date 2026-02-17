#include "pch.h"
#include "BufferPool.h"

BufferPool::~BufferPool(){

	for (auto& pair : _pool) {

		for(Buffer* buffer : pair.second) {

			delete buffer;
		}
	}
	_pool.clear();
}

Buffer* BufferPool::Pop(uint32 requiredSize){

	Buffer* buffer = nullptr;
	const uint32 cap = RoundUpBucket(requiredSize);

	auto& bufferList = _pool[cap];
	if (bufferList.empty()) {

		buffer = new Buffer(cap);
		buffer->SetOwnerBufferPool(this);
	}
	else {
		buffer = bufferList.back();
		bufferList.pop_back();
	}
	buffer->Clear();

	return buffer;
}

void BufferPool::Push(Buffer* buffer) {

	buffer->Clear();
	_pool[buffer->Capacity()].push_back(buffer);
}

LockBufferPool::~LockBufferPool(){

	lock_guard<mutex> lock(_poolMutex);

	for (auto& pair : _pool) {

		for (Buffer* buffer : pair.second) {

			delete buffer;
		}
	}
	_pool.clear();
}

Buffer* LockBufferPool::Pop(uint32 requiredSize){

	Buffer* buffer = nullptr;
	const uint32 cap = RoundUpBucket(requiredSize);

	{
		lock_guard<mutex> lock(_poolMutex);

		auto& bufferList = _pool[cap];
		if (bufferList.empty()) {

			buffer = new Buffer(cap);
			buffer->SetOwnerBufferPool(this);
		}
		else {
			buffer = bufferList.back();
			bufferList.pop_back();
		}
		buffer->Clear();
	}

	return buffer;
}

void LockBufferPool::Push(Buffer* buffer){

	buffer->Clear();

	{
		lock_guard<mutex> lock(_poolMutex);

		_pool[buffer->Capacity()].push_back(buffer);
	}
}

ThreadLocalBufferPool::~ThreadLocalBufferPool(){

	for (auto& pair : _pool) {

		for (Buffer* buffer : pair.second) {

			delete buffer;
		}
	}
	_pool.clear();

	vector<Buffer*> buffers;
	int32 count = _bufferReturnQueue.PopAll(buffers);
	for(int32 i =0; i < buffers.size(); i++) {

		if(buffers[i] != nullptr) {

			delete buffers[i];
		}
	}	
	buffers.clear();
}

Buffer* ThreadLocalBufferPool::Pop(uint32 requiredSize){

	uint32 cap = RoundUpBucket(requiredSize);

	// 남은 버퍼가 없으면 반환된 버퍼큐에서 먼저 흡수
	if (_pool.empty() == true) {
	
		DrainReturnQueue();
	}

	Buffer* buffer = nullptr;
	// 하나도 없으면 생성
	if (_pool.empty() == true) {

		buffer = new Buffer(cap);
		buffer->SetOwnerBufferPool(this);

		return buffer;
	}
	else {
		buffer = _pool[cap].back();
		_pool[cap].pop_back();
	}
	buffer->Clear();

	return buffer;
}

void ThreadLocalBufferPool::Push(Buffer* buffer){

	buffer->Clear();
	_bufferReturnQueue.Push(buffer);
}

void ThreadLocalBufferPool::DrainReturnQueue(){

	vector<Buffer*> buffers;
	int32 bufferCount = _bufferReturnQueue.PopAll(buffers);

	for(int i=0; i < bufferCount; i++) {

		Buffer* buffer = buffers[i];
		_pool[buffer->Capacity()].push_back(buffer);
	}
}
