#include "pch.h"
#include "RecvBuffer.h"

RecvBuffer::RecvBuffer(uint32 bufferSize) : _capacity(bufferSize){

	_buffer = new BYTE[bufferSize];
}

RecvBuffer::~RecvBuffer(){

	delete _buffer;
}

bool RecvBuffer::Write(uint32 writeSize){

	if (FreeSize() < writeSize) {
		return false;
	}
	_writePos += writeSize;

	return true;
}

bool RecvBuffer::Read(uint32 readSize){
	
	if (DataSize() < readSize) {
		return false;
	}
	_readPos += readSize;

	if (_capacity <= _readPos * 2) {
		DataShift();
	}

	return true;
}

void RecvBuffer::Clear(){

	// TODO : _buffer memset 0 
	_writePos = 0;
	_readPos = 0;
}

void RecvBuffer::DataShift(){

	memcpy(&_buffer[0], ReadPos(), DataSize());
	_writePos = DataSize();
	_readPos = 0;
}
