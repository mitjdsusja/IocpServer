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
		msgTest::SC_Response_User_Info packetUserInfo;
		msgTest::UserInfo* userInfo = packetUserInfo.mutable_userinfo();
		msgTest::UserInfo::Position* position = userInfo->mutable_position();
		msgTest::UserInfo::Velocity* velocity = userInfo->mutable_velocity();

		position->set_x(10);
		userInfo->set_id(session->GetSessionId());

		SendBuffer* sendBuffer = MakeSendBuffer(packetUserInfo, PacketId::PKT_SC_RESPONSE_USER_INFO);

		session->Send(sendBuffer);
	}
	{
		// Send Add Users
		msgTest::SC_Add_User packetUserInfo;
		msgTest::UserInfo* userInfo = packetUserInfo.mutable_userinfo();
		msgTest::UserInfo::Position* position = userInfo->mutable_position();
		msgTest::UserInfo::Velocity* velocity = userInfo->mutable_velocity();

		userInfo->set_id(session->GetSessionId());

		SendBuffer* sendBuffer = MakeSendBuffer(packetUserInfo, PacketId::PKT_SC_ADD_USER);

		service->Broadcast(sendBuffer);
	}
}

void PacketHandler::Handle_CS_Request_Other_User_Info(shared_ptr<Session> session, PacketHeader* buffer, Service* service){
}

void PacketHandler::Handle_CS_Send_Pos(shared_ptr<Session> session, PacketHeader* buffer, Service* service){
}

/*-------------------------------------------

--------------------------------------------*/

void PacketHandler::Handle_SC_Response_User_Info(shared_ptr<Session> session, PacketHeader* buffer, Service* service) {

	PacketHeader* header = (PacketHeader*)buffer;
	int32 dataSize = header->packetSize - sizeof(PacketHeader);

	msgTest::SC_Response_User_Info packetUserInfo;
	packetUserInfo.ParseFromArray(&header[1], dataSize);

	cout << "User Id : " << packetUserInfo.userinfo().id() << endl;
	cout << "UserPos " << "X : " << packetUserInfo.userinfo().position().x() << " ";
	cout << "UserPos " << "Y : " << packetUserInfo.userinfo().position().y() << " ";
	cout << "UserPos " << "Z : " << packetUserInfo.userinfo().position().z() << " " << endl;
}

void PacketHandler::Handle_SC_Response_Other_User_Info(shared_ptr<Session> session, PacketHeader* buffer, Service* service){

}

void PacketHandler::Handle_SC_Broadcast_Pos(shared_ptr<Session> session, PacketHeader* buffer, Service* service) {
	// TODO : ERROR LOG
}

void PacketHandler::Handle_SC_Add_User(shared_ptr<Session> session, PacketHeader* buffer, Service* service){

	PacketHeader* header = (PacketHeader*)buffer;
	int32 dataSize = header->packetSize - sizeof(PacketHeader);

	msgTest::SC_Add_User packetAddUser;
	packetAddUser.ParseFromArray(&header[1], dataSize);

	cout << "Add User ID :" << packetAddUser.userinfo().id() << endl;
	cout << "UserPos " << "X : " << packetAddUser.userinfo().position().x() << " ";
	cout << "UserPos " << "Y : " << packetAddUser.userinfo().position().y() << " ";
	cout << "UserPos " << "Z : " << packetAddUser.userinfo().position().z() << " " << endl;
}
