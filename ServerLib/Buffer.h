#pragma once

class IBufferPool;

class Buffer {
public:
	Buffer(uint32 bufferSize);
	~Buffer();

	bool Write(uint32 writeSize);
	void Clear();

	BYTE* GetBuffer() { return _buffer; }
	uint32 Capacity() { return _capacity; }
	uint32 WriteSize() { return _writePos; }

	void SetOwnerBufferPool(IBufferPool* owner) { _owner = owner; }
	void ReturnToOwner();

private:
	BYTE* _buffer;

	uint32 _capacity = 0;
	uint32 _writePos = 0;

	IBufferPool* _owner = nullptr;
};

