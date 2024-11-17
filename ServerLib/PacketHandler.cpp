#include "pch.h"
#include "PacketHandler.h"
#include "BufferPool.h"
#include "messageTest.pb.h"

//TODO : Mapping Function

void PacketHandler::Init(){

	for (int32 i = 0; i < UINT16_MAX; i++) {
		packetHandleArray[i] = Handle_Invalid;
	}

	packetHandleArray[PKT_CS_REQUEST_USER_INFO] = Handle_CS_Request_User_Info;
	packetHandleArray[PKT_CS_REQUEST_OTHER_USER_INFO] = Handle_CS_Request_Other_User_Info;
	packetHandleArray[PKT_CS_SEND_POS] = Handle_CS_Send_Pos;

	packetHandleArray[PKT_SC_RESPONSE_USER_INFO] = Handle_SC_Response_User_Info;
	packetHandleArray[PKT_SC_RESPONSE_OTHER_USER_INFO] = Handle_SC_Response_Other_User_Info;
	packetHandleArray[PKT_SC_BROADCAST_POS] = Handle_SC_Broadcast_Pos;
	packetHandleArray[PKT_SC_ADD_USER] = Handle_SC_Add_User;
}

void PacketHandler::HandlePacket(shared_ptr<Session> session, PacketHeader* buffer, Service* service){
	PacketHeader* header = buffer;

	packetHandleArray[header->packetId](session, header, service);
}

void PacketHandler::Handle_Invalid(shared_ptr<Session> session, PacketHeader* buffer, Service* service){

	ErrorHandler::HandleError(L"INVALID PACKET ID", buffer->packetId);
}

void PacketHandler::Handle_CS_Request_User_Info(shared_ptr<Session> session, PacketHeader* buffer, Service* service){

	{
		// Send User Info
		SendBuffer* sendBuffer = LSendBufferPool->Pop();
		Packet_S_Response_User_Info* packet = (Packet_S_Response_User_Info*)sendBuffer->Buffer();

		packet->packetId = PKT_SC_RESPONSE_USER_INFO;
		packet->packetSize = sizeof(Packet_S_Response_User_Info);
		packet->playerId = session->GetSessionId();
		sendBuffer->Write(packet->packetSize);

		session->Send(sendBuffer);
	}
	{
		// Send Add User
		SendBuffer* sendBuffer = LSendBufferPool->Pop();
		Packet_S_Add_User* packet = (Packet_S_Add_User*)sendBuffer->Buffer();

		packet->packetId = PKT_SC_ADD_USER;
		packet->packetSize = sizeof(Packet_S_Add_User);
		packet->playerId = session->GetSessionId();
		sendBuffer->Write(packet->packetSize);

		service->Broadcast(sendBuffer);
	}
}

void PacketHandler::Handle_CS_Request_Other_User_Info(shared_ptr<Session> session, PacketHeader* buffer, Service* service){

	SendBuffer* sendBuffer = LSendBufferPool->Pop();
	Packet_S_Response_Other_User_Info* packet = (Packet_S_Response_Other_User_Info*)sendBuffer->Buffer();

	packet->packetId = PKT_SC_RESPONSE_OTHER_USER_INFO;
	packet->playerCount = 0;
	packet->packetSize = sizeof(Packet_S_Response_Other_User_Info);
	int32 playerCount = service->GetCurSessionCount();
	int32* playerArray = new int32[playerCount];
	service->GetUserIdList(playerArray);
	for (int32 i = 0; i < playerCount; i++) {
		packet->AppendUserIdData((BYTE*)packet, playerArray[i]);
		packet->packetSize += sizeof(int32);
	}
	sendBuffer->Write(packet->packetSize);

	session->Send(sendBuffer);
}

void PacketHandler::Handle_CS_Send_Pos(shared_ptr<Session> session, PacketHeader* buffer, Service* service){
	Packet_C_Send_Pos* packet = (Packet_C_Send_Pos*)buffer;

	//cout << " Packet PosX : " << packet->posX;
	//cout << " Packet PosY : " << packet->posY;
	//cout << " Packet PosZ : " << packet->posZ << endl;

	SendBuffer* sendBuffer = LSendBufferPool->Pop();
	Packet_S_Broadcast_Pos* sendPacket = (Packet_S_Broadcast_Pos*)sendBuffer->Buffer();

	sendPacket->packetId = PKT_SC_BROADCAST_POS;
	sendPacket->packetSize = sizeof(Packet_S_Broadcast_Pos);
	sendPacket->playerId = packet->playerId;
	sendPacket->posX = packet->posX;
	sendPacket->posY = packet->posY;
	sendPacket->posZ = packet->posZ;
	sendBuffer->Write(sendPacket->packetSize);

	service->Broadcast(sendBuffer);
}

/*-------------------------------------------

--------------------------------------------*/

void PacketHandler::Handle_SC_Response_User_Info(shared_ptr<Session> session, PacketHeader* buffer, Service* service) {
	// TODO : ERROR LOG
	Packet_S_Response_User_Info* packet = (Packet_S_Response_User_Info*)buffer;

	cout << "[Recv] PacketID : " << packet->packetId << " " << "UserID : " << packet->playerId << endl;
}

void PacketHandler::Handle_SC_Response_Other_User_Info(shared_ptr<Session> session, PacketHeader* buffer, Service* service){

	Packet_S_Response_Other_User_Info* packet = (Packet_S_Response_Other_User_Info*)buffer;

	//cout << "[Recv] PacketID : " << packet->packetId << endl;
	for (int32 i = 0; i < packet->playerCount; i++) {
		int32 userId = 0;
		// TODO : Deserialization

		//cout << "UserID : " << userId << endl;
	}
}

void PacketHandler::Handle_SC_Broadcast_Pos(shared_ptr<Session> session, PacketHeader* buffer, Service* service) {
	// TODO : ERROR LOG
}

void PacketHandler::Handle_SC_Add_User(shared_ptr<Session> session, PacketHeader* buffer, Service* service){

}
