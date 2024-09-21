#include "pch.h"
#include "PacketHandler.h"
#include "BufferPool.h"

void PacketHandler::HandlePacket(PacketHeader* buffer, Service* service){
	switch (buffer->packetId) {

	case C_SEND:
		Handle_C_SEND(buffer, service);
		break;
	case S_SEND:
		Handle_S_SEND(buffer, service);
		break;
	case C_MOVE:
		Handle_C_MOVE(buffer, service);
		break;
	default:
		ErrorHandler::HandleError(L"Not Defined PacketId");
		break;
	}
}

void PacketHandler::Handle_S_SEND(PacketHeader* buffer, Service* service){

}

void PacketHandler::Handle_C_SEND(PacketHeader* buffer, Service* service){

	Packet_C_SEND* packet = (Packet_C_SEND*)buffer;
	
	cout << "Packet ID : " << packet->id << endl;
	cout << "Packet HP : " << packet->hp << endl;
	cout << "Packet MP : " << packet->mp << endl;
}

void PacketHandler::Handle_C_MOVE(PacketHeader* buffer, Service* service){
	Packet_C_MOVE* packet = (Packet_C_MOVE*)buffer;

	//cout << " Packet PosX : " << packet->posX;
	//cout << " Packet PosY : " << packet->posY;
	//cout << " Packet PosZ : " << packet->posZ << endl;

	SendBuffer* sendBuffer = GSendBufferPool->Pop();
	Packet_S_MOVE* sendPacket = (Packet_S_MOVE*)sendBuffer->Buffer();

	sendPacket->packetId = S_MOVE;
	sendPacket->packetSize = sizeof(Packet_S_MOVE);
	sendPacket->playerId = packet->playerId;
	sendPacket->posX = packet->posX;
	sendPacket->posY = packet->posY;
	sendPacket->posZ = packet->posZ;
	sendBuffer->Write(sendPacket->packetSize);
	service->Broadcast(sendBuffer);
	GSendBufferPool->Push(sendBuffer);
}
