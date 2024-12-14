#include "pch.h"
#include "PacketHandler.h"
#include "ServerGlobal.h"

#include "JobQueue.h"
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

void PacketHandler::HandlePacket(shared_ptr<Session> session, PacketHeader* dataBuffer, Service* service) {

	shared_ptr<Buffer> buffer(LBufferPool->Pop(),
		[](Buffer* buffer) { LBufferPool->Push(buffer); });

	memcpy(buffer->GetBuffer(), dataBuffer, dataBuffer->packetSize);

	int32 packetId = dataBuffer->packetId;

	// push jobQueue
	GJobQueue->Push([session, buffer, service, packetId]() {
		packetHandleArray[packetId](session, buffer, service);
		});
}

void PacketHandler::Handle_Invalid(shared_ptr<Session> session, shared_ptr<Buffer> buffer, Service* service){

	PacketHeader* header = (PacketHeader*)buffer->GetBuffer();

	ErrorHandler::HandleError(L"INVALID PACKET ID", header->packetId);
}

void PacketHandler::Handle_CS_Request_User_Info(shared_ptr<Session> session, shared_ptr<Buffer> dataBuffer, Service* service){

	{
		// Send User Info
		msgTest::SC_Response_User_Info packetUserInfo;
		msgTest::UserInfo* userInfo = packetUserInfo.mutable_userinfo();
		msgTest::UserInfo::Position* position = userInfo->mutable_position();
		msgTest::UserInfo::Velocity* velocity = userInfo->mutable_velocity();

		userInfo->set_id(session->GetSessionId());

		Buffer* sendBuffer = MakeSendBuffer(packetUserInfo, PacketId::PKT_SC_RESPONSE_USER_INFO);

		session->Send(sendBuffer);
	}
	{
		// Send Add Users
		msgTest::SC_Add_User packetUserInfo;
		msgTest::UserInfo* userInfo = packetUserInfo.mutable_userinfo();
		msgTest::UserInfo::Position* position = userInfo->mutable_position();
		msgTest::UserInfo::Velocity* velocity = userInfo->mutable_velocity();

		userInfo->set_id(session->GetSessionId());

		Buffer* sendBuffer = MakeSendBuffer(packetUserInfo, PacketId::PKT_SC_ADD_USER);

		service->Broadcast(sendBuffer);
	}
}

void PacketHandler::Handle_CS_Request_Other_User_Info(shared_ptr<Session> session, shared_ptr<Buffer> dataBuffer, Service* service){

	//Send Other User Info
	{
		msgTest::SC_Response_Other_User_Info packetUsersInfo;
		
		vector<UserInfo*> usersInfo;
		service->GetUsersInfo(usersInfo);

		for (UserInfo* info : usersInfo) {
			msgTest::UserInfo* userInfo = packetUsersInfo.add_usersinfo();
			msgTest::UserInfo::Position* position = userInfo->mutable_position();
			msgTest::UserInfo::Velocity* velocity = userInfo->mutable_velocity();

			Position& userPos = info->GetPosition();
			Velocity& userVel = info->GetVelocity();
			userInfo->set_id(info->GetId());
			position->set_x(userPos.x);
			position->set_y(userPos.y);
			position->set_z(userPos.z);
			velocity->set_x(userVel.x);
			velocity->set_y(userVel.y);
			velocity->set_z(userVel.z);
		}

		Buffer* sendBuffer = MakeSendBuffer(packetUsersInfo, PacketId::PKT_SC_RESPONSE_OTHER_USER_INFO);
		session->Send(sendBuffer);
	}
}

void PacketHandler::Handle_CS_Send_Pos(shared_ptr<Session> session, shared_ptr<Buffer> dataBuffer, Service* service){

}

/*-------------------------------------------
	
--------------------------------------------*/

void PacketHandler::Handle_SC_Response_User_Info(shared_ptr<Session> session, shared_ptr<Buffer> dataBuffer, Service* service) {

	PacketHeader* header = (PacketHeader*)dataBuffer->GetBuffer();
	int32 dataSize = header->GetDataSize();

	msgTest::SC_Response_User_Info packetUserInfo;
	packetUserInfo.ParseFromArray(&header[1], dataSize);

	cout << "User Id : " << packetUserInfo.userinfo().id() << endl;
	cout << "UserPos " << "X : " << packetUserInfo.userinfo().position().x() << " ";
	cout << "UserPos " << "Y : " << packetUserInfo.userinfo().position().y() << " ";
	cout << "UserPos " << "Z : " << packetUserInfo.userinfo().position().z() << " " << endl;
}

void PacketHandler::Handle_SC_Response_Other_User_Info(shared_ptr<Session> session, shared_ptr<Buffer> dataBuffer, Service* service){

	PacketHeader* header = (PacketHeader*)dataBuffer->GetBuffer();
	int32 dataSize = header->GetDataSize();

	msgTest::SC_Response_Other_User_Info packetOtherUserInfo;
	packetOtherUserInfo.ParseFromArray(&header[1], dataSize);
	
	for (msgTest::UserInfo userInfo : packetOtherUserInfo.usersinfo()) {
		cout << "User[" << userInfo.id() << "]";
		cout << "position [x:" << userInfo.position().x() << ", ";
		cout << "y:" << userInfo.position().y() << ", ";
		cout << "z:" << userInfo.position().z() << "]";
		cout << "velocity [x:" << userInfo.velocity().x() << ", ";
		cout << "y:" << userInfo.velocity().y() << ", ";
		cout << "z:" << userInfo.velocity().z() << "]";
	}
}

void PacketHandler::Handle_SC_Broadcast_Pos(shared_ptr<Session> session, shared_ptr<Buffer> dataBuffer, Service* service) {
	// TODO : ERROR LOG
}

void PacketHandler::Handle_SC_Add_User(shared_ptr<Session> session, shared_ptr<Buffer> dataBuffer, Service* service){

	PacketHeader* header = (PacketHeader*)dataBuffer->GetBuffer();
	int32 dataSize = header->GetDataSize();

	msgTest::SC_Add_User packetAddUser;
	packetAddUser.ParseFromArray(&header[1], dataSize);

	cout << "Add User ID :" << packetAddUser.userinfo().id() << endl;
	cout << "UserPos " << "X : " << packetAddUser.userinfo().position().x() << " ";
	cout << "UserPos " << "Y : " << packetAddUser.userinfo().position().y() << " ";
	cout << "UserPos " << "Z : " << packetAddUser.userinfo().position().z() << " " << endl;
}
