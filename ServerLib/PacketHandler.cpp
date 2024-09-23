#include "pch.h"
#include "PacketHandler.h"
#include "BufferPool.h"

void PacketHandler::HandlePacket(PacketHeader* buffer, Service* service){
	switch (buffer->packetId) {

	case c_send:
		Handle_C_SEND(buffer, service);
		break;
	case c_pos:
		Handle_C_Pos(buffer, service);
		break;


	case s_send:
		Handle_S_SEND(buffer, service);
		break;
	
	default:
		ErrorHandler::HandleError(L"Not Defined PacketId");
		break;
	}
}

void PacketHandler::Handle_C_SEND(PacketHeader* buffer, Service* service){

	Packet_C_SEND* packet = (Packet_C_SEND*)buffer;
	
}

void PacketHandler::Handle_C_Pos(PacketHeader* buffer, Service* service){
	Packet_C_Pos* packet = (Packet_C_Pos*)buffer;

	//cout << " Packet PosX : " << packet->posX;
	//cout << " Packet PosY : " << packet->posY;
	//cout << " Packet PosZ : " << packet->posZ << endl;

	SendBuffer* sendBuffer = GSendBufferPool->Pop();
	Packet_S_Pos_Broadcast* sendPacket = (Packet_S_Pos_Broadcast*)sendBuffer->Buffer();

	sendPacket->packetId = s_pos_broadcast;
	sendPacket->packetSize = sizeof(Packet_S_Pos_Broadcast);
	sendPacket->playerId = packet->playerId;
	sendPacket->posX = packet->posX;
	sendPacket->posY = packet->posY;
	sendPacket->posZ = packet->posZ;
	sendBuffer->Write(sendPacket->packetSize);
	service->Broadcast(sendBuffer);
	GSendBufferPool->Push(sendBuffer);
}

void PacketHandler::Handle_S_SEND(PacketHeader* buffer, Service* service) {

}

void PacketHandler::handle_S_Pos_Broadcast(PacketHeader* buffer, Service* service) {

}