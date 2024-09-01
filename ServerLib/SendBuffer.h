#pragma once

class SendBuffer {
public:
	SendBuffer(uint32 bufferSize);
	~SendBuffer();

	bool Write(uint32 writeSize);
	void Clear();

	BYTE* Buffer() { return _buffer; }
	uint32 Capacity() { return _capacity; }
	uint32 WriteSize() { return _writePos; }

private:
	BYTE* _buffer;

	uint32 _capacity = 0;
	uint32 _writePos = 0;

};

