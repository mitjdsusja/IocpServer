#pragma once
#include "Buffer.h"
#include "LockQueue.h"

enum {
	BUFFER_COUNT = 1000,
	BUFFER_SIZE = 4096
};

class IBufferPool {

public:
	virtual ~IBufferPool() = default;
	virtual void Push(Buffer* buffer) = 0;
	virtual Buffer* Pop(uint32 requiredSize) = 0;

};

static int32 RoundUpBucket(int32 size) {

	int32 bucketSize = 256;
	while (bucketSize < size) {

		bucketSize *= 2;
	}
	return bucketSize;
}

class BufferPool : public IBufferPool{
public:
	BufferPool() = default;
	~BufferPool();

	Buffer* Pop(uint32 requiredSize) override;
	void Push(Buffer* buffer) override;

private:
	unordered_map<uint32, vector<Buffer*>> _pool;
};

class ThreadLocalBufferPool : public IBufferPool {
public:
	ThreadLocalBufferPool() = default;
	~ThreadLocalBufferPool();

	Buffer* Pop(uint32 requiredSize) override;
	void Push(Buffer* buffer) override;

private:
	void DrainReturnQueue();

private:
	unordered_map<uint32, vector<Buffer*>> _pool;
	LockQueue<Buffer*> _bufferReturnQueue;

};

class LockBufferPool : public IBufferPool{
public:
	LockBufferPool() = default;
	~LockBufferPool();

	Buffer* Pop(uint32 requiredSize) override;
	void Push(Buffer* buffer) override;

private:
	unordered_map<uint32, vector<Buffer*>> _pool;
	mutex _poolMutex;
};
