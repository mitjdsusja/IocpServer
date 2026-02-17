#include "pch.h"
#include "Buffer.h"

Buffer::Buffer(uint32 bufferSize) : _capacity(bufferSize){

	_buffer = new BYTE[bufferSize];
}

Buffer::~Buffer(){

	if (_buffer != nullptr) {
		delete _buffer;
	}
	_buffer = nullptr;
}

bool Buffer::Write(uint32 writeSize){

	if(_writePos + writeSize > _capacity) {
		return false;
	}
	_writePos += writeSize;

	return true;
}

void Buffer::Clear(){

	_writePos = 0;
}

void Buffer::ReturnToOwner() {

	Clear();
	_owner->Push(this);
}
