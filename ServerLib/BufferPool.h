#pragma once
#include "Buffer.h"

enum {
	BUFFER_COUNT = 1000,
	BUFFER_SIZE = 4096
};

class IBufferPool {
	

public:
	virtual ~IBufferPool() = default;
	virtual void Push(Buffer* buffer) = 0;
	virtual Buffer* Pop() = 0;

protected:
	vector<Buffer*> _buffers;
	int32 _bufferCount = 0;
	int32 _remainedCount = 0;
};

class BufferPool : public IBufferPool{
public:
	BufferPool();
	~BufferPool();

	Buffer* Pop() override;
	void Push(Buffer* buffer) override;

};

class PushLockBufferPool : public IBufferPool {
public:
	PushLockBufferPool();
	~PushLockBufferPool();

	Buffer* Pop() override;
	void Push(Buffer* buffer) override;

private:
	mutex _buffersMutex;
};

class LockBufferPool : public IBufferPool{
public:
	LockBufferPool();
	~LockBufferPool();

	Buffer* Pop() override;
	void Push(Buffer* buffer) override;

private:
	mutex _buffersMutex;
};
