#include "pch.h"
#include "PacketHandler.h"
#include "BufferPool.h"

//TODO : Mapping Function

void PacketHandler::Init(){

	for (int32 i = 0; i < UINT16_MAX; i++) {
		packetHandleArray[i] = Handle_Invalid;
	}

	packetHandleArray[PKT_C_REQUEST_INFO] = Handle_C_Request_Info;
	packetHandleArray[PKT_C_POS] = Handle_C_Pos;

	packetHandleArray[PKT_S_RESPONE_INFO] = Handle_S_Respone_Info;
	packetHandleArray[PKT_S_BROADCAST_POS] = handle_S_Pos_Broadcast;
}

void PacketHandler::HandlePacket(shared_ptr<Session> session, PacketHeader* buffer, Service* service){
	PacketHeader* header = buffer;
	packetHandleArray[header->packetId](session, header, service);
}

void PacketHandler::Handle_Invalid(shared_ptr<Session> session, PacketHeader* buffer, Service* service){

	// TODO : Log
}

void PacketHandler::Handle_C_Request_Info(shared_ptr<Session> session, PacketHeader* buffer, Service* service){

	Packet_C_Request_Info* packet = (Packet_C_Request_Info*)buffer;
}

void PacketHandler::Handle_C_Pos(shared_ptr<Session> session, PacketHeader* buffer, Service* service){
	Packet_C_Pos* packet = (Packet_C_Pos*)buffer;

	//cout << " Packet PosX : " << packet->posX;
	//cout << " Packet PosY : " << packet->posY;
	//cout << " Packet PosZ : " << packet->posZ << endl;

	SendBuffer* sendBuffer = GSendBufferPool->Pop();
	Packet_S_Broadcast_Pos* sendPacket = (Packet_S_Broadcast_Pos*)sendBuffer->Buffer();

	sendPacket->packetId = PKT_S_BROADCAST_POS;
	sendPacket->packetSize = sizeof(PKT_S_BROADCAST_POS);
	sendPacket->playerId = packet->playerId;
	sendPacket->posX = packet->posX;
	sendPacket->posY = packet->posY;
	sendPacket->posZ = packet->posZ;
	sendBuffer->Write(sendPacket->packetSize);

	service->Broadcast(sendBuffer);
	GSendBufferPool->Push(sendBuffer);
}

void PacketHandler::Handle_S_Respone_Info(shared_ptr<Session> session, PacketHeader* buffer, Service* service) {

}

void PacketHandler::handle_S_Pos_Broadcast(shared_ptr<Session> session, PacketHeader* buffer, Service* service) {

}