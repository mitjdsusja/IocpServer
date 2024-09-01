#pragma once

class RecvBuffer{
public:
	RecvBuffer(uint32 bufferSize);
	~RecvBuffer();

	bool Write(uint32 writeSize);
	bool Read(uint32 writeSize);
	void Clear();

	BYTE* WritePos() { return &_buffer[_writePos]; }
	BYTE* ReadPos() { return &_buffer[_readPos]; }
	uint32 Capacity() { return _capacity; }
	uint32 DataSize() { return _writePos - _readPos; }
	uint32 FreeSize() { return _capacity - _writePos; }


private:
	BYTE* _buffer;
	
	int32 _capacity = 0;
	uint32 _writePos = 0;
	uint32 _readPos = 0;
};

