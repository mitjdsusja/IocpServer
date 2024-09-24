#pragma once

class BufferPool{
	enum {
		buffer_count = 100,
		buffer_size = 4096
	};

public:
	BufferPool();
	~BufferPool();

	SendBuffer* Pop();
	void Push(SendBuffer* buffer);

private:
	mutex _mutex;

	vector<SendBuffer*> _buffers;
	int32 _bufferCount = 0;

};

