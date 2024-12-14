#pragma once
#include "Buffer.h"

class BufferPool{
	enum {
		buffer_count = 100,
		buffer_size = 4096
	};

public:
	BufferPool();
	~BufferPool();

	Buffer* Pop();
	void Push(Buffer* buffer);

private:
	vector<Buffer*> _buffers;
	int32 _bufferCount = 0;

};
