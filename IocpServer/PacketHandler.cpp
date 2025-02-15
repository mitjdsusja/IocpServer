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
	packetHandleArray[PKT_CS_LOGIN_REQUEST] = Handle_CS_Login_Requset;
	packetHandleArray[PKT_CS_ROOM_LIST_REQUEST] = Handle_CS_Room_List_Request;
	packetHandleArray[PKT_CS_PLAYER_INFO_REQUEST] = Handle_CS_Player_Info_Request;
	packetHandleArray[PKT_CS_PLAYER_LIST_REQUEST] = Handle_CS_Player_List_Request;
	packetHandleArray[PKT_CS_ENTER_ROOM_REQUEST] = Handle_CS_Enter_Room_Request;
	packetHandleArray[PKT_CS_CREATE_ROOM_REQUEST] = Handle_CS_Create_Room_Request;


	/*------------
		S -> C
	-------------*/
	packetHandleArray[PKT_SC_LOGIN_RESPONSE] = Handle_SC_Login_Response;
	packetHandleArray[PKT_SC_ROOM_LIST_RESPONSE] = Handle_SC_Room_List_Response;
	packetHandleArray[PKT_SC_PLAYER_INFO_RESPONSE] = Handle_SC_Player_Info_Response;
	packetHandleArray[PKT_SC_PLAYER_LIST_RESPONSE] = Handle_SC_Player_List_Response;
	packetHandleArray[PKT_SC_ENTER_ROOM_RESPONSE] = Handle_SC_Enter_Room_Response;

}

void PacketHandler::HandlePacket(shared_ptr<GameSession> session, PacketHeader* dataBuffer, Service* service) {

	shared_ptr<Buffer> buffer = shared_ptr<Buffer>(GSendBufferPool->Pop(), [](Buffer* buffer) { GSendBufferPool->Push(buffer); });

	BYTE* data = ((BYTE*)dataBuffer) + sizeof(PacketHeader);
	int32 dataSize = dataBuffer->GetDataSize();

	memcpy(buffer->GetBuffer(), data, dataSize);
	buffer->Write(dataBuffer->packetSize);

	int32 packetId = dataBuffer->packetId;

	cout << "[RECV] " << packetId << " From : " << session->GetSessionId() << endl;
	
	// push jobQueue
	Job* job = new Job([session, buffer, service, packetId]() {
		packetHandleArray[packetId](session, buffer, service);
		});
	GJobQueue->Push(job);
}

void PacketHandler::Handle_Invalid(shared_ptr<GameSession> session, shared_ptr<Buffer> buffer, Service* service) {

	PacketHeader* header = (PacketHeader*)buffer->GetBuffer();

	ErrorHandler::HandleError(L"INVALID PACKET ID", header->packetId);
}


/*------------
	C -> S
-------------*/
void PacketHandler::Handle_CS_Login_Requset(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service) {
	
	// Check Id, Passwd
	msgTest::CS_Login_Request recvLoginPacket;
	recvLoginPacket.ParseFromArray(dataBuffer->GetBuffer(), dataBuffer->WriteSize());
	
	string id = recvLoginPacket.id();
	string pw = recvLoginPacket.password();
	cout << "[Login] ID : " << id << " " << "PW : " << pw << endl;

	bool userExists = false;
	{
		// database
		wstring wId(id.begin(), id.end());
		wstring wPw(pw.begin(), pw.end());

		std::wstring query = L"SELECT COUNT(*), usernum, name FROM USERS WHERE id = '" + wId + L"' AND password_hash = '" + wPw + L"';";
		vector<vector<wstring>> result = LDBConnector->ExecuteSelectQuery(query);

		if (!result.empty() && !result[0].empty()) {
			int32 count = std::stoi(result[0][0]); // 문자열을 숫자로 변환
			int32 userNum = stoi(result[0][1]);
			wstring name = result[0][2];
			if (count > 0) {
				//wcout << L"User found!" << endl;
				userExists = true;
				
				session->SetDbId(userNum);
				GPlayerManager->CreateAndAddPlayer(session, session->GetSessionId(), name);
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
	msgTest::SC_Login_Response sendLoginResponsePacket;

	if (userExists == true) {
		sendLoginResponsePacket.set_success(true);
		sendLoginResponsePacket.set_session_id(session->GetSessionId());
	}
	else {
		sendLoginResponsePacket.set_success(false);
		sendLoginResponsePacket.set_error_message("LOGIN FAIL");
	}
	sendBuffer = PacketHandler::MakeSendBuffer(sendLoginResponsePacket, PacketId::PKT_SC_LOGIN_RESPONSE);

	// Send Result
	Job* job = new Job([session, sendBuffer]() {
		session->Send(sendBuffer);
	});
	GJobQueue->Push(job);
}

void PacketHandler::Handle_CS_Room_List_Request(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service) {
	
	vector<RoomInfo> roomInfoList = GRoomManager->GetRoomInfoList();

	msgTest::SC_Room_List_Response sendRoomListRequestPacket;
	for (const auto& roomInfo : roomInfoList) {
		msgTest::Room* room = sendRoomListRequestPacket.add_room_list();
		string roomName = boost::locale::conv::utf_to_utf<char>(roomInfo._roomName);
		string hostPlayerName = boost::locale::conv::utf_to_utf<char>(roomInfo._hostPlayerName);

		room->set_roomid(roomInfo._roomId);
		room->set_roomname(roomName);
		room->set_maxplayercount(roomInfo._maxPlayerCount);
		room->set_playercount(roomInfo._curPlayerCount);
		room->set_hostplayername(hostPlayerName);
	}

	shared_ptr<Buffer> sendBuffer = MakeSendBuffer(sendRoomListRequestPacket, PacketId::PKT_SC_ROOM_LIST_RESPONSE);
	Job* job = new Job([session, sendBuffer]() {
		session->Send(sendBuffer);
	});
	GJobQueue->Push(job);
}

void PacketHandler::Handle_CS_Player_Info_Request(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service) {
	
	msgTest::CS_Player_Info_Request recvRequestPlayerInfoPacket;
	recvRequestPlayerInfoPacket.ParseFromArray(dataBuffer->GetBuffer(), dataBuffer->WriteSize());

	uint64 sessionId = recvRequestPlayerInfoPacket.session_id();
	if (session->GetSessionId() != sessionId) {
		cout << "Session ID missmatch" << endl;
		return;
	}

	// database
	GameSession* gameSession = (GameSession*)session.get();
	wstring query = L"SELECT name, level from users WHERE usernum = " + to_wstring(gameSession->GetDbId()) + L";";
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
	msgTest::SC_Player_Info_Response sendPlayerInfoResponsePacket;
	msgTest::Player* playerInfo = sendPlayerInfoResponsePacket.mutable_player_info();
	playerInfo->set_level(level);
	playerInfo->set_name(name);
	playerInfo->mutable_position();

	shared_ptr<Buffer> sendBuffer = MakeSendBuffer(sendPlayerInfoResponsePacket, PacketId::PKT_SC_PLAYER_INFO_RESPONSE);

	Job* job = new Job([session, sendBuffer]() {
		session->Send(sendBuffer);
		});
	GJobQueue->Push(job);
}

void PacketHandler::Handle_CS_Player_List_Request(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service) {

}

void PacketHandler::Handle_CS_Create_Room_Request(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service){

	msgTest::CS_Create_Room_Request recvCreateRoomPacket;
	recvCreateRoomPacket.ParseFromArray(dataBuffer->GetBuffer(), dataBuffer->WriteSize());

	wstring roomName = boost::locale::conv::utf_to_utf<wchar_t>(recvCreateRoomPacket.room_name());
	wstring hostName = boost::locale::conv::utf_to_utf<wchar_t>(recvCreateRoomPacket.host_name());

	shared_ptr<Player> hostPlayer = GPlayerManager->GetPlayer(session->GetSessionId());
	int roomId = GRoomManager->CreateAndAddRoom(hostPlayer, roomName);
	
	msgTest::SC_Create_Room_Response sendCreateRoomResponsePacket;
	sendCreateRoomResponsePacket.set_room_id(roomId);
	

	shared_ptr<Buffer> sendBuffer = MakeSendBuffer(sendCreateRoomResponsePacket, PacketId::PKT_SC_CREATE_ROOM_RESPONSE);
	Job* job = new Job([session, sendBuffer]() {
		session->Send(sendBuffer);
		});
	GJobQueue->Push(job);
}

void PacketHandler::Handle_CS_Enter_Room_Request(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service) {

}


/*------------
	S -> C
-------------*/
void PacketHandler::Handle_SC_Login_Response(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service) {

}
void PacketHandler::Handle_SC_Room_List_Response(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service) {

}
void PacketHandler::Handle_SC_Player_Info_Response(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service) {

}
void PacketHandler::Handle_SC_Player_List_Response(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service) {

}
void PacketHandler::Handle_SC_Enter_Room_Response(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service) {

}
