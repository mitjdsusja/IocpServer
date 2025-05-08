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
#include "Vector.h"

void PacketHandler::RegisterPacketHandlers() {

	for (int32 i = 0; i < UINT16_MAX; i++) {
		packetHandleArray[i] = Handle_Invalid;
	}

	/*------------
		C -> S
	-------------*/
	packetHandleArray[PKT_CS_LOGIN_REQUEST] = Handle_CS_Login_Request;
	packetHandleArray[PKT_CS_ROOM_LIST_REQUEST] = Handle_CS_Room_List_Request;
	packetHandleArray[PKT_CS_MY_PLAYER_INFO_REQUEST] = Handle_CS_Player_Info_Request;
	packetHandleArray[PKT_CS_ROOM_PLAYER_LIST_REQUEST] = Handle_CS_Room_Player_List_Request;
	packetHandleArray[PKT_CS_ENTER_ROOM_REQUEST] = Handle_CS_Enter_Room_Request;
	packetHandleArray[PKT_CS_CREATE_ROOM_REQUEST] = Handle_CS_Create_Room_Request;
	packetHandleArray[PKT_CS_PLAYER_MOVE_REQUEST] = Handle_CS_Player_Move_Request;


	/*------------
		S -> C
	-------------*/
	packetHandleArray[PKT_SC_LOGIN_RESPONSE] = Handle_SC_Login_Response;
	packetHandleArray[PKT_SC_ROOM_LIST_RESPONSE] = Handle_SC_Room_List_Response;
	packetHandleArray[PKT_SC_MY_PLAYER_INFO_RESPONSE] = Handle_SC_Player_Info_Response;
	packetHandleArray[PKT_SC_ROOM_PLAYER_LIST_RESPONSE] = Handle_SC_Player_List_Response;
	packetHandleArray[PKT_SC_ENTER_ROOM_RESPONSE] = Handle_SC_Enter_Room_Response;

}

void PacketHandler::HandlePacket(shared_ptr<GameSession> session, PacketHeader* dataBuffer, Service* service) {

	shared_ptr<Buffer> buffer = shared_ptr<Buffer>(GSendBufferPool->Pop(), [](Buffer* buffer) { GSendBufferPool->Push(buffer); });

	BYTE* data = ((BYTE*)dataBuffer) + sizeof(PacketHeader);
	int32 dataSize = dataBuffer->GetDataSize();

	memcpy(buffer->GetBuffer(), data, dataSize);
	buffer->Write(dataSize);

	int32 packetId = dataBuffer->packetId;

	//cout << "[RECV] " << packetId << " From : " << session->GetSessionId() << endl;
	
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
void PacketHandler::Handle_CS_Ping(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service){

	msgTest::CS_Ping recvPingPacket;
	recvPingPacket.ParseFromArray(dataBuffer->GetBuffer(), dataBuffer->WriteSize());

	msgTest::SC_Pong sendPongPacket;
	sendPongPacket.set_timestamp(recvPingPacket.timestamp());

	shared_ptr<Buffer> sendBuffer = MakeSendBuffer<msgTest::SC_Pong>(sendPongPacket, PacketId::PKT_SC_PONG);
	Job* job = new Job([session, sendBuffer]() {
		session->Send(sendBuffer);
	});
	GJobQueue->Push(job);
}

void PacketHandler::Handle_CS_Login_Request(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service) {

	// Check Id, Passwd
	msgTest::CS_Login_Request recvLoginPacket;
	recvLoginPacket.ParseFromArray(dataBuffer->GetBuffer(), dataBuffer->WriteSize());

	uint32 writeSize = dataBuffer->WriteSize();

	BYTE* str = dataBuffer->GetBuffer();

	string id = recvLoginPacket.id();
	string pw = recvLoginPacket.password();
	string errorMessage = "";
	//cout << "[Login Request] ID : " << id << " " << "PW : " << pw << endl;

	bool userExists = false;
	{
		// database
		wstring wId(id.begin(), id.end());
		wstring wPw(pw.begin(), pw.end());

		std::wstring query = L"SELECT COUNT(*), usernum, level, name, pos_x, pos_y, pos_z FROM USERS WHERE id = '" + wId + L"' AND password_hash = '" + wPw + L"';";
		vector<vector<wstring>> result = LDBConnector->ExecuteSelectQuery(query);

		if (!result.empty() && !result[0].empty()) {
			int32 count = stoi(result[0][0]); // 문자열을 숫자로 변환
			int32 userNum = stoi(result[0][1]);
			int32 level = stoi(result[0][2]);
			wstring name = result[0][3];
			Vector<int16> position(stoi(result[0][4]), stoi(result[0][5]), stoi(result[0][6]));

			if (count > 0) {
				//wcout << L"User found!" << endl;
				userExists = true;
				
				session->SetDbId(userNum);

				PlayerInfo playerInfo;
				playerInfo._level = level;
				playerInfo._name = name;
				playerInfo._position._x = position._x;
				playerInfo._position._y = position._y;
				playerInfo._position._z = position._z;
				playerInfo._moveTimestamp = 0;
				GPlayerManager->SetPlayerInfo(session->GetSessionId(), playerInfo);
			}
			else {
				errorMessage = "Invalid ID or password.";
				wcout << L"Invalid ID or password."  << wId << endl;
			}
		}
		else {
			errorMessage = "Query returned no results.";
			wcout << L"Query returned no results." << endl;
		}

		
	}

	shared_ptr<Buffer> sendBuffer;
	msgTest::SC_Login_Response sendLoginResponsePacket;

	if (userExists == true) {
		sendLoginResponsePacket.set_success(true);
		sendLoginResponsePacket.set_sessionid(session->GetSessionId());
	}
	else {
		sendLoginResponsePacket.set_success(false);
		sendLoginResponsePacket.set_errormessage(errorMessage);
	}
	sendBuffer = PacketHandler::MakeSendBuffer(sendLoginResponsePacket, PacketId::PKT_SC_LOGIN_RESPONSE);

	//cout << "Login request : " << id << " _ " << pw << endl;

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
		msgTest::Room* room = sendRoomListRequestPacket.add_roomlist();
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
	
	msgTest::CS_My_Player_Info_Request recvRequestPlayerInfoPacket;
	recvRequestPlayerInfoPacket.ParseFromArray(dataBuffer->GetBuffer(), dataBuffer->WriteSize());

	uint64 sessionId = recvRequestPlayerInfoPacket.sessionid();
	if (session->GetSessionId() != sessionId) {
		cout << "Session ID missmatch" << endl;
		return;
	}

	// database
	GameSession* gameSession = (GameSession*)session.get();
	wstring query = L"SELECT name, level, pos_x, pos_y, pos_z from users WHERE usernum = " + to_wstring(gameSession->GetDbId()) + L";";
	//wcout << query << endl;
	vector<vector<wstring>> result = LDBConnector->ExecuteSelectQuery(query);

	string name;
	int32 level;
	int16 posX, posY, posZ;
	if (!result.empty() && !result[0].empty()) {
		name = boost::locale::conv::utf_to_utf<char>(result[0][0]);
		level = stoi(result[0][1]);
		posX = stoi(result[0][2]);
		posY = stoi(result[0][3]);
		posZ = stoi(result[0][4]);
	}
	else {
		wcout << L"Query returned no results." << endl;
		return;
	}

	// send packet
	msgTest::SC_My_Player_Info_Response sendPlayerInfoResponsePacket;
	msgTest::Player* playerInfo = sendPlayerInfoResponsePacket.mutable_playerinfo();
	msgTest::Vector* position = playerInfo->mutable_position();
	playerInfo->set_level(level);
	playerInfo->set_name(name);
	position->set_x(posX);
	position->set_y(posY);
	position->set_z(posZ);

	shared_ptr<Buffer> sendBuffer = MakeSendBuffer(sendPlayerInfoResponsePacket, PacketId::PKT_SC_MY_PLAYER_INFO_RESPONSE);

	Job* job = new Job([session, sendBuffer]() {
		session->Send(sendBuffer);
		});
	GJobQueue->Push(job);
}

void PacketHandler::Handle_CS_Room_Player_List_Request(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service) {
	
	msgTest::CS_Room_Player_List_Request recvRoomPlayerListRequestPacket;
	recvRoomPlayerListRequestPacket.ParseFromArray(dataBuffer->GetBuffer(), dataBuffer->WriteSize());

	int32 roomId = recvRoomPlayerListRequestPacket.roomid();
	
	// Get Player List with GRoomManager
	RoomInfo roomInfo = GRoomManager->GetRoomInfo(roomId);

	msgTest::SC_Room_Player_List_Response sendRoomPlayerListResponsePacket;
	for (const auto& playerInfo : roomInfo._playerInfoList) {
		msgTest::Player* player = sendRoomPlayerListResponsePacket.add_playerlist();
		msgTest::Vector* position = player->mutable_position();
		string name = boost::locale::conv::utf_to_utf<char>(playerInfo._name);
		player->set_name(name);
		position->set_x(playerInfo._position._x);
		position->set_y(playerInfo._position._y);
		position->set_z(playerInfo._position._z);
	}
	
	shared_ptr<Buffer> sendBuffer = MakeSendBuffer(sendRoomPlayerListResponsePacket, PacketId::PKT_SC_ROOM_PLAYER_LIST_RESPONSE);

	Job* job = new Job([session, sendBuffer]() {
		session->Send(sendBuffer);
		});
	GJobQueue->Push(job);
}

void PacketHandler::Handle_CS_Create_Room_Request(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service){

	msgTest::CS_Create_Room_Request recvCreateRoomPacket;
	recvCreateRoomPacket.ParseFromArray(dataBuffer->GetBuffer(), dataBuffer->WriteSize());

	wstring roomName = boost::locale::conv::utf_to_utf<wchar_t>(recvCreateRoomPacket.roomname());
	wstring hostName = boost::locale::conv::utf_to_utf<wchar_t>(recvCreateRoomPacket.hostname());

	shared_ptr<Player> hostPlayer = GPlayerManager->GetPlayer(session->GetSessionId());
	int roomId = GRoomManager->CreateAndAddRoom(hostPlayer, roomName);
	cout << "[CreateRoom] Room ID : " << roomId << endl;

	hostPlayer->SetRoomId(roomId);

	RoomInfo roomInfo = GRoomManager->GetRoomInfo(roomId);

	msgTest::SC_Create_Room_Response sendCreateRoomResponsePacket;
	msgTest::Room* room = sendCreateRoomResponsePacket.mutable_room();

	sendCreateRoomResponsePacket.set_success(true);
	room->set_roomid(roomInfo._roomId);
	room->set_roomname(boost::locale::conv::utf_to_utf<char>(roomInfo._roomName));
	room->set_maxplayercount(roomInfo._maxPlayerCount);
	room->set_playercount(roomInfo._curPlayerCount);
	room->set_hostplayername(boost::locale::conv::utf_to_utf<char>(roomInfo._hostPlayerName));

	shared_ptr<Buffer> sendBuffer = MakeSendBuffer(sendCreateRoomResponsePacket, PacketId::PKT_SC_CREATE_ROOM_RESPONSE);
	Job* job = new Job([session, sendBuffer]() {
		session->Send(sendBuffer);
		});
	GJobQueue->Push(job);
}

void PacketHandler::Handle_CS_Enter_Room_Request(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service) {

	int32 roomId = 0;
	// enter response
	{
		msgTest::CS_Enter_Room_Request recvEnterRoomRequestPacket;
		recvEnterRoomRequestPacket.ParseFromArray(dataBuffer->GetBuffer(), dataBuffer->WriteSize());

		roomId = recvEnterRoomRequestPacket.roomid();

		shared_ptr<Player> player = GPlayerManager->GetPlayer(session->GetSessionId());

		bool roomEnterReturn = GRoomManager->EnterRoom(roomId, session->GetSessionId(), GPlayerManager->GetPlayer(session->GetSessionId()));
		if (roomEnterReturn == false) {
			wcout << "방 입장 실패 : " << player->GetName() << endl;
			return;
		}
		player->SetRoomId(roomId);

		RoomInfo roomInfo = GRoomManager->GetRoomInfo(roomId);

		msgTest::SC_Enter_Room_Response sendEnterRoomResponsePacket;
		msgTest::Room* room = sendEnterRoomResponsePacket.mutable_room();
		sendEnterRoomResponsePacket.set_success(roomEnterReturn);
		room->set_roomid(roomInfo._roomId);
		room->set_roomname(boost::locale::conv::utf_to_utf<char>(roomInfo._roomName));
		room->set_playercount(roomInfo._curPlayerCount);
		room->set_maxplayercount(roomInfo._maxPlayerCount);
		room->set_hostplayername(boost::locale::conv::utf_to_utf<char>(roomInfo._hostPlayerName));

		shared_ptr<Buffer> sendBuffer = MakeSendBuffer(sendEnterRoomResponsePacket, PacketId::PKT_SC_ENTER_ROOM_RESPONSE);
		Job* job = new Job([session, sendBuffer]() {
			session->Send(sendBuffer);
			});
		GJobQueue->Push(job);

		if (roomEnterReturn == false) return;
	}
	{
		// notify user join
		msgTest::SC_Player_Enter_Room_Notification sendPlayerEnterRoomNotificationPacket;
		msgTest::Player* player = sendPlayerEnterRoomNotificationPacket.mutable_player();
		msgTest::Vector* position = player->mutable_position();
		PlayerInfo playerInfo = GPlayerManager->GetPlayer(session->GetSessionId())->GetPlayerInfo();

		player->set_name(boost::locale::conv::utf_to_utf<char>(playerInfo._name));
		player->set_level(playerInfo._level);
		position->set_x(playerInfo._position._x);
		position->set_y(playerInfo._position._y);
		position->set_z(playerInfo._position._z);

		shared_ptr<Buffer> sendBuffer = MakeSendBuffer(sendPlayerEnterRoomNotificationPacket, PacketId::PKT_SC_PLAYER_ENTER_ROOM_NOTIFICATION);
		wcout << "Notification Enter Player : " << playerInfo._name << endl;

		GRoomManager->BroadcastToRoom(roomId, sendBuffer);
	}
}

void PacketHandler::Handle_CS_Player_Move_Request(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service){

	msgTest::CS_Player_Move_Request recvPlayerMoveReqeustPacket;
	recvPlayerMoveReqeustPacket.ParseFromArray(dataBuffer->GetBuffer(), dataBuffer->WriteSize());

	msgTest::MoveState moveState = recvPlayerMoveReqeustPacket.movestate();
	wstring playerName = boost::locale::conv::utf_to_utf<wchar_t>(moveState.playername());
	Vector<int16> position(moveState.position().x(), moveState.position().y(), moveState.position().z());
	Vector<int16> velocity(moveState.velocity().x(), moveState.velocity().y(), moveState.velocity().z());
	int64 timestamp = moveState.timestamp();

	shared_ptr<Player> player = GPlayerManager->GetPlayer(session->GetSessionId());
	PlayerInfo playerInfo;
	playerInfo._name = playerName;
	playerInfo._position = position;
	playerInfo._velocity = velocity;
	playerInfo._moveTimestamp = timestamp;

	if (player == nullptr) {
		cout << "INVALID PLAYER" << endl;
		return;
	}
	player->SetPlayerMove(playerInfo);
}



/*------------
	S -> C
-------------*/
void PacketHandler::Handle_CS_Pong(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service){

}

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
