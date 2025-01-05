#include "pch.h"
#include "PacketHandler.h"
#include "ServerGlobal.h"

#include "Job.h"
#include "JobQueue.h"
#include "BufferPool.h"
#include "JobTimer.h"

#include "messageTest.pb.h"

//TODO : Mapping Function

void PacketHandler::Init(){

	for (int32 i = 0; i < UINT16_MAX; i++) {
		packetHandleArray[i] = Handle_Invalid;
	}

	packetHandleArray[PKT_CS_CONNECT_SERVER] = Handle_CS_Connect_Server;
	packetHandleArray[PKT_CS_REQUEST_SERVER_STATE] = Handle_CS_Request_Server_State;
	packetHandleArray[PKT_CS_MOVE_USER] = Handle_CS_Move_User;

	packetHandleArray[PKT_SC_ACCEPT_CLIENT] = Handle_SC_Accept_Client;
	packetHandleArray[PKT_SC_RESPONSE_SERVER_STATE] = Handle_SC_Response_Server_State;
	packetHandleArray[PKT_SC_RESULT_MOVE_USER] = Handle_SC_Result_Move_User;
	packetHandleArray[PKT_SC_CONNET_OTHER_USER] = Handle_SC_Connect_Other_User;
}

void PacketHandler::HandlePacket(shared_ptr<Session> session, PacketHeader* dataBuffer, Service* service) {

	shared_ptr<Buffer> buffer = shared_ptr<Buffer>(GSendBufferPool->Pop(), [](Buffer* buffer) { GSendBufferPool->Push(buffer); });

	memcpy(buffer->GetBuffer(), dataBuffer, dataBuffer->packetSize);

	int32 packetId = dataBuffer->packetId;

	// push jobQueue
	Job* job = new Job([session, buffer, service, packetId]() {
		packetHandleArray[packetId](session, buffer, service);
	});
	GJobQueue->Push(job);
}

void PacketHandler::Handle_Invalid(shared_ptr<Session> session, shared_ptr<Buffer> buffer, Service* service){

	PacketHeader* header = (PacketHeader*)buffer->GetBuffer();

	ErrorHandler::HandleError(L"INVALID PACKET ID", header->packetId);
}

void PacketHandler::Handle_CS_Connect_Server(shared_ptr<Session> session, shared_ptr<Buffer> dataBuffer, Service* service){

	// Send User Info
	{
		msgTest::SC_Accept_Client packetAcceptClient;
		msgTest::UserInfo* userInfo = packetAcceptClient.mutable_userinfo();
		msgTest::Position* position = userInfo->mutable_position();
		msgTest::Direction* direction = userInfo->mutable_direction();

		int32 userId = session->GetSessionId();
		session->SetUserId(userId);
		userInfo->set_id(userId);

		shared_ptr<Buffer> sendBuffer = MakeSendBuffer(packetAcceptClient, PacketId::PKT_SC_ACCEPT_CLIENT);
		Job* job = new Job([session, sendBuffer]() {
			session->Send(sendBuffer);
			});
		GJobQueue->Push(job);
	}

	// Broadcast Connect User Info
	{
		msgTest::SC_Connect_Other_User packetConnectOtherUser;
		msgTest::UserInfo* userInfo = packetConnectOtherUser.mutable_userinfo();
		msgTest::Position* position = userInfo->mutable_position();
		msgTest::Direction* direction = userInfo->mutable_direction();

		int32 userId = session->GetSessionId();
		userInfo->set_id(userId);

		shared_ptr<Buffer> sendBuffer = MakeSendBuffer(packetConnectOtherUser, PacketId::PKT_SC_CONNET_OTHER_USER);
		Job* job = new Job([service, sendBuffer]() {
			service->Broadcast(sendBuffer);
			});
		GJobQueue->Push(job);
	}
}

void PacketHandler::Handle_CS_Request_Server_State(shared_ptr<Session> session, shared_ptr<Buffer> dataBuffer, Service* service){

	//Send Other User Info
	{
		msgTest::SC_Response_Server_State packetUsersInfo;
		
		vector<UserInfo> usersInfo;
		service->GetUsersInfo(usersInfo);

		for (UserInfo info : usersInfo) {
			msgTest::UserInfo* userInfo = packetUsersInfo.add_userinfos();
			msgTest::Position* position = userInfo->mutable_position();
			msgTest::Direction* direction = userInfo->mutable_direction();

			Position userPos = info.GetPosition();
			Direction userVel = info.GetDirection();
			userInfo->set_id(info.GetId());
			position->set_x(userPos.x);
			position->set_y(userPos.y);
			position->set_z(userPos.z);
			direction->set_x(userVel.x);
			direction->set_y(userVel.y);
			direction->set_z(userVel.z);
		}

		shared_ptr<Buffer> sendBuffer = MakeSendBuffer(packetUsersInfo, PacketId::PKT_SC_RESPONSE_SERVER_STATE);
		Job* job = new Job([session, sendBuffer]() {
			session->Send(sendBuffer);
		});
		GJobQueue->Push(job);
	}
}

void PacketHandler::Handle_CS_Move_User(shared_ptr<Session> session, shared_ptr<Buffer> dataBuffer, Service* service){
	cout << "[RECV] Handle_CS_Move_User " << session->GetSessionId() << " ";
	
	// Update UserInfo
	PacketHeader* header = (PacketHeader*)dataBuffer->GetBuffer();
	int32 dataSize = header->packetSize - sizeof(PacketHeader);

	msgTest::CS_Move_User recvMoveUser;
	recvMoveUser.ParseFromArray(((BYTE*)header) + sizeof(PacketHeader), dataSize);

	UserInfo userInfo;
	userInfo.SetId(recvMoveUser.movestate().userid());
	userInfo.SetPosition(recvMoveUser.movestate().position().x(), recvMoveUser.movestate().position().y(), recvMoveUser.movestate().position().z());
	userInfo.SetDirection(recvMoveUser.movestate().direction().x(), recvMoveUser.movestate().direction().y(), recvMoveUser.movestate().direction().z());
	userInfo.SetLastMovePacket(recvMoveUser.movestate().timestamp());

	cout << "Set User Info : " << recvMoveUser.movestate().userid() << " ";
	cout << "Pos : " << recvMoveUser.movestate().position().x() << " " << recvMoveUser.movestate().position().y() << " " << recvMoveUser.movestate().position().z() << endl;

	session->SetUserInfo(userInfo);
}

/*-------------------------------------------
	
--------------------------------------------*/

void PacketHandler::Handle_SC_Accept_Client(shared_ptr<Session> session, shared_ptr<Buffer> dataBuffer, Service* service) {

	PacketHeader* header = (PacketHeader*)dataBuffer->GetBuffer();
	int32 dataSize = header->GetDataSize();

	msgTest::SC_Accept_Client packetAcceptClient;
	packetAcceptClient.ParseFromArray(&header[1], dataSize);

	cout << "User Id : " << packetAcceptClient.userinfo().id() << endl;
	cout << "UserPos " << "X : " << packetAcceptClient.userinfo().position().x() << " ";
	cout << "UserPos " << "Y : " << packetAcceptClient.userinfo().position().y() << " ";
	cout << "UserPos " << "Z : " << packetAcceptClient.userinfo().position().z() << " " << endl;
}

void PacketHandler::Handle_SC_Response_Server_State(shared_ptr<Session> session, shared_ptr<Buffer> dataBuffer, Service* service){

	PacketHeader* header = (PacketHeader*)dataBuffer->GetBuffer();
	int32 dataSize = header->GetDataSize();

	msgTest::SC_Response_Server_State packetOtherUserInfo;
	packetOtherUserInfo.ParseFromArray(&header[1], dataSize);
	
	for (msgTest::UserInfo userInfo : packetOtherUserInfo.userinfos()) {
		cout << "User[" << userInfo.id() << "]";
		cout << "position [x:" << userInfo.position().x() << ", ";
		cout << "y:" << userInfo.position().y() << ", ";
		cout << "z:" << userInfo.position().z() << "]";
		cout << "velocity [x:" << userInfo.direction().x() << ", ";
		cout << "y:" << userInfo.direction().y() << ", ";
		cout << "z:" << userInfo.direction().z() << "]";
	}
}

void PacketHandler::Handle_SC_Result_Move_User(shared_ptr<Session> session, shared_ptr<Buffer> dataBuffer, Service* service) {
	// TODO : ERROR LOG
}

void PacketHandler::Handle_SC_Connect_Other_User(shared_ptr<Session> session, shared_ptr<Buffer> dataBuffer, Service* service){

	PacketHeader* header = (PacketHeader*)dataBuffer->GetBuffer();
	int32 dataSize = header->GetDataSize();

	msgTest::SC_Connect_Other_User packetConnectOtherUser;
	packetConnectOtherUser.ParseFromArray(&header[1], dataSize);

	cout << "Add User ID :" << packetConnectOtherUser.userinfo().id() << endl;
	cout << "UserPos " << "X : " << packetConnectOtherUser.userinfo().position().x() << " ";
	cout << "UserPos " << "Y : " << packetConnectOtherUser.userinfo().position().y() << " ";
	cout << "UserPos " << "Z : " << packetConnectOtherUser.userinfo().position().z() << " " << endl;
}
