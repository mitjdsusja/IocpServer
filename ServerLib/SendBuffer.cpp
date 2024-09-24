#include "pch.h"
#include "SendBuffer.h"

SendBuffer::SendBuffer(uint32 bufferSize) : _capacity(bufferSize){

	_buffer = new BYTE[_capacity];
}

SendBuffer::~SendBuffer(){

	if (_buffer != nullptr) {
		delete _buffer;
	}
	_buffer = nullptr;
}

bool SendBuffer::Write(uint32 writeSize){

	if (_capacity - _writePos < writeSize) {
		return false;
	}
	_writePos += writeSize;

	return true;
}

void SendBuffer::Clear(){

	_writePos = 0;
}
