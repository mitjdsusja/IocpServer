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
#include "PlayerManager.h"
#include "GameSession.h"
#include "RoomManager.h"
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
	packetHandleArray[PKT_CS_PING] = Handle_CS_Ping;


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
	
	packetHandleArray[packetId](session, buffer, service);
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
	
	GPlayerManager->PushJobSendData(session->GetSessionId(), sendBuffer);
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
			Vector<int16> parsePosition(stoi(result[0][4]), stoi(result[0][5]), stoi(result[0][6]));

			if (count > 0) {
				//wcout << L"User found!" << endl;
				userExists = true;
				
				session->SetDbId(userNum);

				PlayerBaseInfo baseInfo;
				PlayerPosition position;
				PlayerStats stats;

				stats._level = level;
				baseInfo._name = name;
				position._position._x = parsePosition._x;
				position._position._y = parsePosition._y;
				position._position._z = parsePosition._z;
				position._moveTimestamp = 0;

				GPlayerManager->PushJobCreateAndPushPlayer(session, baseInfo, position, stats);
			}
			else {
				errorMessage = "Invalid ID or password.";
				wcout << L"[PacketHandler::Handle_CS_Login_Request] Invalid ID or password."  << wId << endl;
			}

			wcout << L"[PacketHandler::Handle_CS_Login_Request] Client Login : " << name << endl;
		}
		else {
			errorMessage = "Query returned no results.";
			wcout << L"[PacketHandler::Handle_CS_Login_Request] Query returned no results." << endl;
		}
	}

	msgTest::SC_Login_Response sendLoginResponsePacket;

	if (userExists == true) {
		sendLoginResponsePacket.set_success(true);
		sendLoginResponsePacket.set_sessionid(session->GetSessionId());
	}
	else {
		sendLoginResponsePacket.set_success(false);
		sendLoginResponsePacket.set_errormessage(errorMessage);
	}
	shared_ptr<Buffer> sendBuffer = PacketHandler::MakeSendBuffer(sendLoginResponsePacket, PacketId::PKT_SC_LOGIN_RESPONSE);

	GPlayerManager->PushJobSendData(session->GetSessionId(), sendBuffer);
}

void PacketHandler::Handle_CS_Room_List_Request(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service) {

	GRoomManager->PushJobGetRoomInfoList([session](vector<RoomInfo> roomInfoList) {

		msgTest::SC_Room_List_Response roomListResponsePacket;
		for (const auto& roomInfo : roomInfoList) {
			msgTest::Room* room = roomListResponsePacket.add_roomlist();
			string roomName = boost::locale::conv::utf_to_utf<char>(roomInfo._initRoomInfo._roomName);
			string hostPlayerName = boost::locale::conv::utf_to_utf<char>(roomInfo._hostPlayerName);

			room->set_roomid(roomInfo._initRoomInfo._roomId);
			room->set_roomname(roomName);
			room->set_maxplayercount(roomInfo._initRoomInfo._maxPlayerCount);
			room->set_playercount(roomInfo._curPlayerCount);
			room->set_hostplayername(hostPlayerName);
		}

		shared_ptr<Buffer> sendBuffer = PacketHandler::MakeSendBuffer(roomListResponsePacket, PacketId::PKT_SC_ROOM_LIST_RESPONSE);

		GPlayerManager->SendData(session->GetSessionId(), sendBuffer);
	});
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
	wstring query = L"SELECT name, level, pos_x, pos_y, pos_z from users WHERE usernum = " + to_wstring(session->GetDbId()) + L";";
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

	GPlayerManager->PushJobSendData(session->GetSessionId(), sendBuffer);
}

void PacketHandler::Handle_CS_Room_Player_List_Request(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service) {
	
	msgTest::CS_Room_Player_List_Request recvRoomPlayerListRequestPacket;
	recvRoomPlayerListRequestPacket.ParseFromArray(dataBuffer->GetBuffer(), dataBuffer->WriteSize());

	int32 roomId = recvRoomPlayerListRequestPacket.roomid();
	
	GRoomManager->PushJobGetRoomPlayerList(roomId, [session](vector<Room::RoomPlayer> roomPlayerList) {

		msgTest::SC_Room_Player_List_Response sendRoomPlayerListResponsePacket;
		for (const auto& roomPlayer : roomPlayerList) {
			msgTest::Player* player = sendRoomPlayerListResponsePacket.add_playerlist();
			msgTest::Vector* position = player->mutable_position();
			string name = boost::locale::conv::utf_to_utf<char>(roomPlayer._gameState._name);
			player->set_name(name);
			position->set_x(roomPlayer._gameState._position._x);
			position->set_y(roomPlayer._gameState._position._y);
			position->set_z(roomPlayer._gameState._position._z);
		}

		shared_ptr<Buffer> sendBuffer = MakeSendBuffer(sendRoomPlayerListResponsePacket, PacketId::PKT_SC_ROOM_PLAYER_LIST_RESPONSE);

		GPlayerManager->PushJobSendData(session->GetSessionId(), sendBuffer);
	});
}

void PacketHandler::Handle_CS_Create_Room_Request(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service){

	msgTest::CS_Create_Room_Request recvCreateRoomPacket;
	recvCreateRoomPacket.ParseFromArray(dataBuffer->GetBuffer(), dataBuffer->WriteSize());

	wstring roomName = boost::locale::conv::utf_to_utf<wchar_t>(recvCreateRoomPacket.roomname());
	wstring hostName = boost::locale::conv::utf_to_utf<wchar_t>(recvCreateRoomPacket.hostname());

	InitRoomInfo initRoomInfo;
	initRoomInfo._roomName = roomName;

	Room::RoomPlayer hostPlayerData;
	hostPlayerData._sessionId = session->GetSessionId();
	hostPlayerData._gameState._name = hostName;

	GRoomManager->PushJobCreateAndPushRoom(initRoomInfo, hostPlayerData);
}

void PacketHandler::Handle_CS_Enter_Room_Request(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service) {

	msgTest::CS_Enter_Room_Request recvEnterRoomRequestPacket;
	recvEnterRoomRequestPacket.ParseFromArray(dataBuffer->GetBuffer(), dataBuffer->WriteSize());

	int32 enterRoomId = recvEnterRoomRequestPacket.roomid();

	GPlayerManager->PushJobGetRoomPlayer(session->GetSessionId(), [enterRoomId](PlayerBaseInfo baseInfo, PlayerPosition position) {
		
		Room::RoomPlayer roomPlayer;
		roomPlayer._sessionId = baseInfo._sessionId;
		roomPlayer._gameState._moveTimeStamp = position._moveTimestamp;
		roomPlayer._gameState._name = baseInfo._name;
		roomPlayer._gameState._position = position._position;
		roomPlayer._gameState._updatePosition = true;
		roomPlayer._gameState._velocity = position._velocity;

		GRoomManager->PushJobEnterRoom(enterRoomId, move(roomPlayer));
	});
}

void PacketHandler::Handle_CS_Player_Move_Request(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service){

	msgTest::CS_Player_Move_Request recvPlayerMoveReqeustPacket;
	recvPlayerMoveReqeustPacket.ParseFromArray(dataBuffer->GetBuffer(), dataBuffer->WriteSize());

	msgTest::MoveState moveState = recvPlayerMoveReqeustPacket.movestate();
	Room::RoomPlayer roomPlayerData;
	roomPlayerData._sessionId = session->GetSessionId();
	roomPlayerData._gameState._moveTimeStamp = moveState.timestamp();
	roomPlayerData._gameState._name = boost::locale::conv::utf_to_utf<wchar_t>(moveState.playername());
	roomPlayerData._gameState._position = { (int16)moveState.position().x(), (int16)moveState.position().y(), (int16)moveState.position().z() };
	roomPlayerData._gameState._velocity = { (int16)moveState.velocity().x(), (int16)moveState.velocity().y(), (int16)moveState.velocity().z() };
	roomPlayerData._gameState._updatePosition = true;

	GRoomManager->PushJobMovePlayer(moveState.roomid(), roomPlayerData);
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
