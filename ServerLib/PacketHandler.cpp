#include "pch.h"
#include "PacketHandler.h"
#include "BufferPool.h"

//TODO : Mapping Function

void PacketHandler::Init(){

	for (int32 i = 0; i < UINT16_MAX; i++) {
		packetHandleArray[i] = Handle_Invalid;
	}

	packetHandleArray[PKT_C_REQUEST_USER_INFO] = Handle_C_Request_User_Info;
	packetHandleArray[PKT_C_SEND_POS] = Handle_C_Send_Pos;

	packetHandleArray[PKT_S_RESPONSE_USER_INFO] = Handle_S_Response_User_Info;
	packetHandleArray[PKT_S_BROADCAST_POS] = Handle_S_Broadcast_Pos;
}

void PacketHandler::HandlePacket(shared_ptr<Session> session, PacketHeader* buffer, Service* service){
	PacketHeader* header = buffer;
	packetHandleArray[header->packetId](session, header, service);
}

void PacketHandler::Handle_Invalid(shared_ptr<Session> session, PacketHeader* buffer, Service* service){

	ErrorHandler::HandleError(L"INVALID PACKET ID");
}

void PacketHandler::Handle_C_Request_User_Info(shared_ptr<Session> session, PacketHeader* buffer, Service* service){

	SendBuffer* sendBuffer = GSendBufferPool->Pop();
	Packet_S_Response_User_Info* packet = (Packet_S_Response_User_Info*)sendBuffer->Buffer();

	packet->packetId = PKT_S_RESPONSE_USER_INFO;
	packet->packetSize = sizeof(Packet_S_Response_User_Info);
	packet->playerId = session->GetSessionId();
	sendBuffer->Write(packet->packetSize);

	session->Send(sendBuffer);
	GSendBufferPool->Push(sendBuffer);
}

void PacketHandler::Handle_C_Request_Other_User_Info(shared_ptr<Session> session, PacketHeader* buffer, Service* service){

	// TODO : Send Other User Info
	SendBuffer* sendBuffer = GSendBufferPool->Pop();
	Packet_S_Response_Other_User_Info* packet = (Packet_S_Response_Other_User_Info*)sendBuffer->Buffer();

	packet->packetId = PKT_S_RESPONSE_OTHER_USER_INFO;
	packet->playerCount = service->GetCurSessionCount();
	packet->packetSize = sizeof(packet->playerCount);

	int32* playerArray = new int32[packet->playerCount];
	service->GetUserIdList(playerArray);
	for (int32 i = 0; i < packet->playerCount; i++) {
		packet->AppendUserIdData((BYTE*)packet, playerArray[i]);
		packet->packetSize += sizeof(int32);
	}
}

void PacketHandler::Handle_C_Send_Pos(shared_ptr<Session> session, PacketHeader* buffer, Service* service){
	Packet_C_Send_Pos* packet = (Packet_C_Send_Pos*)buffer;

	//cout << " Packet PosX : " << packet->posX;
	//cout << " Packet PosY : " << packet->posY;
	//cout << " Packet PosZ : " << packet->posZ << endl;

	SendBuffer* sendBuffer = GSendBufferPool->Pop();
	Packet_S_Broadcast_Pos* sendPacket = (Packet_S_Broadcast_Pos*)sendBuffer->Buffer();

	sendPacket->packetId = PKT_S_BROADCAST_POS;
	sendPacket->packetSize = sizeof(Packet_S_Broadcast_Pos);
	sendPacket->playerId = packet->playerId;
	sendPacket->posX = packet->posX;
	sendPacket->posY = packet->posY;
	sendPacket->posZ = packet->posZ;
	sendBuffer->Write(sendPacket->packetSize);

	service->Broadcast(sendBuffer);
	GSendBufferPool->Push(sendBuffer);
}

void PacketHandler::Handle_S_Response_User_Info(shared_ptr<Session> session, PacketHeader* buffer, Service* service) {
	// TODO : ERROR LOG
}

void PacketHandler::Handle_S_Response_Other_User_Info(shared_ptr<Session> session, PacketHeader* buffer, Service* service){

}

void PacketHandler::Handle_S_Broadcast_Pos(shared_ptr<Session> session, PacketHeader* buffer, Service* service) {
	// TODO : ERROR LOG
}

void PacketHandler::Handle_S_Add_User(shared_ptr<Session> session, PacketHeader* buffer, Service* service){

}
