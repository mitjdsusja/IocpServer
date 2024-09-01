#pragma once

class BufferPool{
	enum {
		BUFFER_COUNT = 100,
		BUFFER_SIZE = 4096
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

