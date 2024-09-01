#include "pch.h"
#include "RecvBuffer.h"

RecvBuffer::RecvBuffer(uint32 bufferSize) : _capacity(bufferSize){

	_buffer = new BYTE[bufferSize];
}

RecvBuffer::~RecvBuffer(){

	delete _buffer;
}

bool RecvBuffer::Write(uint32 writeSize){

	if (_capacity - _writePos < writeSize) {
		return false;
	}
	_writePos += writeSize;

	return true;
}

bool RecvBuffer::Read(uint32 readSize){

	if (_capacity - _readPos < readSize) {
		return false;
	}
	_readPos += readSize;

	return true;
}

void RecvBuffer::Clear(){

	// TODO : _buffer memset 0 
}
