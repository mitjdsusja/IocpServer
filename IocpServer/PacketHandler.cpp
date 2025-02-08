#include "pch.h"
#include "PacketHandler.h"
#include "ServerGlobal.h"
#include <boost/locale.hpp>

#include "Job.h"
#include "JobQueue.h"
#include "BufferPool.h"
#include "JobTimer.h"
#include "Service.h"

#include "messageTest.pb.h"

#include "Global.h"
#include "RoomManager.h"
#include "GameSession.h"

void PacketHandler::RegisterPacketHandlers() {

	for (int32 i = 0; i < UINT16_MAX; i++) {
		packetHandleArray[i] = Handle_Invalid;
	}

	/*------------
		C -> S
	-------------*/
	packetHandleArray[PKT_CS_LOGIN] = Handle_CS_Login;
	packetHandleArray[PKT_CS_REQUEST_ROOM_LIST] = Handle_CS_Request_Room_List;
	packetHandleArray[PKT_CS_REQUEST_USER_INFO] = Handle_CS_Request_User_Info;
	packetHandleArray[PKT_CS_REQUEST_USER_LIST] = Handle_CS_Request_User_List;
	packetHandleArray[PKT_CS_ENTER_ROOM] = Handle_CS_Enter_Room;


	/*------------
		S -> C
	-------------*/
	packetHandleArray[PKT_SC_LOGIN_SUCCESS] = Handle_SC_Login_Success;
	packetHandleArray[PKT_SC_LOGIN_FAIL] = Handle_SC_Login_Fail;
	packetHandleArray[PKT_SC_RESPONSE_ROOM_LIST] = Handle_SC_Response_Room_List;
	packetHandleArray[PKT_SC_RESPONSE_USER_INFO] = Handle_SC_Response_User_Info;
	packetHandleArray[PKT_SC_RESPONSE_USER_LIST] = Handle_SC_Response_User_List;
	packetHandleArray[PKT_SC_ENTER_ROOM_SUCCESS] = Handle_SC_Enter_Room_Success;
	packetHandleArray[PKT_SC_ENTER_ROOM_FAIL] = Handle_SC_Enter_Room_Fail;

}

void PacketHandler::HandlePacket(shared_ptr<Session> session, PacketHeader* dataBuffer, Service* service) {

	shared_ptr<Buffer> buffer = shared_ptr<Buffer>(GSendBufferPool->Pop(), [](Buffer* buffer) { GSendBufferPool->Push(buffer); });

	BYTE* data = ((BYTE*)dataBuffer) + sizeof(PacketHeader);
	int32 dataSize = dataBuffer->GetDataSize();

	memcpy(buffer->GetBuffer(), data, dataSize);
	buffer->Write(dataBuffer->packetSize);

	int32 packetId = dataBuffer->packetId;

	// push jobQueue
	Job* job = new Job([session, buffer, service, packetId]() {
		packetHandleArray[packetId](session, buffer, service);
		});
	GJobQueue->Push(job);
}

void PacketHandler::Handle_Invalid(shared_ptr<Session> session, shared_ptr<Buffer> buffer, Service* service) {

	PacketHeader* header = (PacketHeader*)buffer->GetBuffer();

	ErrorHandler::HandleError(L"INVALID PACKET ID", header->packetId);
}


/*------------
	C -> S
-------------*/
void PacketHandler::Handle_CS_Login(shared_ptr<Session> session, shared_ptr<Buffer> dataBuffer, Service* service) {
	
	// Check Id, Passwd
	msgTest::CS_Login recvLoginPacket;
	recvLoginPacket.ParseFromArray(dataBuffer->GetBuffer(), dataBuffer->WriteSize());
	
	string id = recvLoginPacket.id();
	string pw = recvLoginPacket.passwd();
	cout << "[Login] ID : " << id << " " << "PW : " << pw << endl;

	bool userExists = false;
	{
		// database
		wstring wId(id.begin(), id.end());
		wstring wPw(pw.begin(), pw.end());

		std::wstring query = L"SELECT COUNT(*), usernum FROM USERS WHERE id = '" + wId + L"' AND password_hash = '" + wPw + L"';";
		vector<vector<wstring>> result = LDBConnector->ExecuteSelectQuery(query);

		if (!result.empty() && !result[0].empty()) {
			int32 count = std::stoi(result[0][0]); // 문자열을 숫자로 변환
			int32 userNum = stoi(result[0][1]);
			if (count > 0) {
				//wcout << L"User found!" << endl;
				userExists = true;
				GameSession* gameSession = (GameSession*)session.get();
				gameSession->SetUserNum(userNum);
				
			}
			else {
				wcout << L"Invalid ID or password." << endl;
			}
		}
		else {
			wcout << L"Query returned no results." << endl;
		}

	}

	shared_ptr<Buffer> sendBuffer;
	if (userExists == true) {
		msgTest::SC_Login_Success sendLoginSuccessPacket;
		sendLoginSuccessPacket.set_sessionid(session->GetSessionId());
		sendBuffer = PacketHandler::MakeSendBuffer(sendLoginSuccessPacket, PacketId::PKT_SC_LOGIN_SUCCESS);
	}
	else {
		msgTest::SC_Login_Fail sendLoginFailPacket;
		sendLoginFailPacket.set_errorcode(1);
		sendBuffer = PacketHandler::MakeSendBuffer(sendLoginFailPacket, PacketId::PKT_SC_LOGIN_FAIL);
	}

	// Send Result
	Job* job = new Job([session, sendBuffer]() {
		session->Send(sendBuffer);
	});
	GJobQueue->Push(job);
}

void PacketHandler::Handle_CS_Request_Room_List(shared_ptr<Session> session, shared_ptr<Buffer> dataBuffer, Service* service) {
	
	vector<RoomInfo> roomInfoList = GRoomManager->GetRoomInfoList();

	msgTest::SC_Response_Room_List sendResponseRoomList;
	for (const auto& roomInfo : roomInfoList) {
		msgTest::Room* room = sendResponseRoomList.add_roomlist();
		string roomName = boost::locale::conv::utf_to_utf<char>(roomInfo._roomName);

		room->set_roomid(roomInfo._roomId);
		room->set_roomname(roomName);
		room->set_maxplayercount(roomInfo._maxPlayerCount);
		room->set_playercount(roomInfo._curPlayerCount);
	}

	shared_ptr<Buffer> sendBuffer = MakeSendBuffer(sendResponseRoomList, PacketId::PKT_SC_RESPONSE_ROOM_LIST);
	Job* job = new Job([session, sendBuffer]() {
		session->Send(sendBuffer);
	});
	GJobQueue->Push(job);
}

void PacketHandler::Handle_CS_Request_User_Info(shared_ptr<Session> session, shared_ptr<Buffer> dataBuffer, Service* service) {
	
	msgTest::CS_Request_User_Info recvRequestUserInfoPacket;
	recvRequestUserInfoPacket.ParseFromArray(dataBuffer->GetBuffer(), dataBuffer->WriteSize());

	uint64 sessionId = recvRequestUserInfoPacket.sessionid();
	if (session->GetSessionId() != sessionId) {
		cout << "Session ID missmatch" << endl;
		return;
	}

	// database
	GameSession* gameSession = (GameSession*)session.get();
	wstring query = L"SELECT name, level from users WHERE usernum = " + to_wstring(gameSession->GetUserNum()) + L";";
	wcout << query << endl;
	vector<vector<wstring>> result = LDBConnector->ExecuteSelectQuery(query);

	string name;
	int32 level;
	if (!result.empty() && !result[0].empty()) {
		name = boost::locale::conv::utf_to_utf<char>(result[0][0]);
		level = stoi(result[0][1]);
	}
	else {
		wcout << L"Query returned no results." << endl;
		return;
	}

	// send packet
	msgTest::SC_Response_User_Info sendResponseUserInfo;
	msgTest::Player* playerInfo = sendResponseUserInfo.mutable_playerinfo();
	playerInfo->set_level(level);
	playerInfo->set_name(name);
	playerInfo->mutable_position();

	shared_ptr<Buffer> sendBuffer = MakeSendBuffer(sendResponseUserInfo, PacketId::PKT_SC_RESPONSE_USER_INFO);

	Job* job = new Job([session, sendBuffer]() {
		session->Send(sendBuffer);
		});
	GJobQueue->Push(job);
}

void PacketHandler::Handle_CS_Request_User_List(shared_ptr<Session> session, shared_ptr<Buffer> dataBuffer, Service* service) {

}

void PacketHandler::Handle_CS_Create_Room(shared_ptr<Session> session, shared_ptr<Buffer> dataBuffer, Service* service){

	msgTest::CS_Create_Room recvCreateRoomPacket;
	recvCreateRoomPacket.ParseFromArray(dataBuffer->GetBuffer(), dataBuffer->WriteSize());

	wstring roomName = boost::locale::conv::utf_to_utf<wchar_t>(recvCreateRoomPacket.roomname());
	wstring hostName = boost::locale::conv::utf_to_utf<wchar_t>(recvCreateRoomPacket.hostname());

	shared_ptr<Player> hostPlayer = GPlayerManager->GetPlayer(session->GetSessionId());
	GRoomManager->CreateAndAddRoom(hostPlayer, roomName);
	

	// ing
}

void PacketHandler::Handle_CS_Enter_Room(shared_ptr<Session> session, shared_ptr<Buffer> dataBuffer, Service* service) {

}


/*------------
	S -> C
-------------*/
void PacketHandler::Handle_SC_Login_Success(shared_ptr<Session> session, shared_ptr<Buffer> dataBuffer, Service* service) {

}
void PacketHandler::Handle_SC_Login_Fail(shared_ptr<Session> session, shared_ptr<Buffer> dataBuffer, Service* service) {

}
void PacketHandler::Handle_SC_Response_Room_List(shared_ptr<Session> session, shared_ptr<Buffer> dataBuffer, Service* service) {

}
void PacketHandler::Handle_SC_Response_User_Info(shared_ptr<Session> session, shared_ptr<Buffer> dataBuffer, Service* service) {

}
void PacketHandler::Handle_SC_Response_User_List(shared_ptr<Session> session, shared_ptr<Buffer> dataBuffer, Service* service) {

}
void PacketHandler::Handle_SC_Enter_Room_Success(shared_ptr<Session> session, shared_ptr<Buffer> dataBuffer, Service* service) {

}
void PacketHandler::Handle_SC_Enter_Room_Fail(shared_ptr<Session> session, shared_ptr<Buffer> dataBuffer, Service* service) {

}



