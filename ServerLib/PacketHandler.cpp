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
		msgTest::SC_Response_User_Info userInfo;
	}
	{
		// Send Add User
	}
}

void PacketHandler::Handle_CS_Request_Other_User_Info(shared_ptr<Session> session, PacketHeader* buffer, Service* service){
}

void PacketHandler::Handle_CS_Send_Pos(shared_ptr<Session> session, PacketHeader* buffer, Service* service){
}

/*-------------------------------------------

--------------------------------------------*/

void PacketHandler::Handle_SC_Response_User_Info(shared_ptr<Session> session, PacketHeader* buffer, Service* service) {

}

void PacketHandler::Handle_SC_Response_Other_User_Info(shared_ptr<Session> session, PacketHeader* buffer, Service* service){

}

void PacketHandler::Handle_SC_Broadcast_Pos(shared_ptr<Session> session, PacketHeader* buffer, Service* service) {
	// TODO : ERROR LOG
}

void PacketHandler::Handle_SC_Add_User(shared_ptr<Session> session, PacketHeader* buffer, Service* service){

}
