#pragma once
#include "Buffer.h"

enum {
	buffer_count = 100,
	buffer_size = 4096
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
class LockBufferPool : public IBufferPool{
public:
	LockBufferPool();
	~LockBufferPool();

	Buffer* Pop() override;
	void Push(Buffer* buffer) override;

private:
	mutex _sendQueueMutex;
};
