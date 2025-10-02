#include "pch.h"
#include "PacketHandler.h"
#include "ServerGlobal.h"
#include <boost/locale.hpp>

#include "Job.h"
#include "Actor.h"
#include "BufferPool.h"
#include "TimedJob.h"
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
	packetHandleArray[PKT_CS_ENTER_ROOM_COMPLETE] = Handle_CS_Enter_Room_Complete;
	packetHandleArray[PKT_CS_CREATE_ROOM_REQUEST] = Handle_CS_Create_Room_Request;
	packetHandleArray[PKT_CS_PLAYER_MOVE_REQUEST] = Handle_CS_Player_Move_Request;
	packetHandleArray[PKT_CS_PING] = Handle_CS_Ping;
	packetHandleArray[PKT_CS_SKILL_USE] = Handle_CS_Skill_Use;


	/*------------
		S -> C
	-------------*/
	packetHandleArray[PKT_SC_LOGIN_RESPONSE] = Handle_SC_Login_Response;
	packetHandleArray[PKT_SC_ROOM_LIST_RESPONSE] = Handle_SC_Room_List_Response;
	packetHandleArray[PKT_SC_MY_PLAYER_INFO_RESPONSE] = Handle_SC_Player_Enter_Room_Notification;
	packetHandleArray[PKT_SC_ROOM_PLAYER_LIST_RESPONSE] = Handle_SC_Player_Move_Notification;
	packetHandleArray[PKT_SC_ENTER_ROOM_RESPONSE] = Handle_SC_Player_List_In_Grid;
	packetHandleArray[PKT_SC_SKILL_RESULT] = Handle_SC_Skill_Result;
	packetHandleArray[PKT_SC_SKILL_CAST] = Handle_SC_Skill_Cast;
}

void PacketHandler::HandlePacket(shared_ptr<GameSession> session, PacketHeader* dataBuffer, Service* service) {

	shared_ptr<Buffer> buffer = shared_ptr<Buffer>(LSendBufferPool->Pop(), [](Buffer* buffer) { buffer->ReturnToOwner(); });

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

	spdlog::info("[PacketHandler::Handle_Invalid] INVALID PACKET");
}




/*------------
	C -> S
-------------*/
void PacketHandler::Handle_CS_Ping(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service){

	msgTest::CS_Ping recvPingPacket;
	recvPingPacket.ParseFromArray(dataBuffer->GetBuffer(), dataBuffer->WriteSize());

	uint64 serverTimestamp = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - GServerStartTimePoint).count();

	msgTest::SC_Pong sendPongPacket;
	sendPongPacket.set_timestamp(recvPingPacket.timestamp());
	sendPongPacket.set_servertimestamp(serverTimestamp);

	vector<shared_ptr<Buffer>> sendBuffer = MakeSendBuffer(sendPongPacket, PacketId::PKT_SC_PONG);
	
	for (auto& buffer : sendBuffer) {
		session->Send(buffer);
	}
	//GPlayerManager->PushJobSendData(session->GetSessionId(), sendBuffer);
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
		//spdlog::info("SELECT COUNT(*), usernum, level, name, pos_x, pos_y, pos_z FROM USERS WHERE id = '{}' AND password_hash = '{}';", boost::locale::conv::utf_to_utf<char>(wId), boost::locale::conv::utf_to_utf<char>(wPw));
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
				PlayerTransform position;
				PlayerStats stats;

				stats._level = level;
				baseInfo._sessionId = session->GetSessionId();
				baseInfo._name = name;
				position._position._x = parsePosition._x;
				position._position._y = parsePosition._y;
				position._position._z = parsePosition._z;
				position._lastmoveTimestamp = 0;

				GPlayerManager->PushJobCreateAndPushPlayer(session, baseInfo, position, stats);
			}
			else {
				errorMessage = "Invalid ID or password.";
				spdlog::info("[PacketHandler::Handle_CS_Login_Request] Invalid ID or password. {}", boost::locale::conv::utf_to_utf<char>(wId));
			}

			spdlog::info("[PacketHandler::Handle_CS_Login_Request] Client Login : {}", boost::locale::conv::utf_to_utf<char>(name));
		}
		else {
			errorMessage = "Query returned no results.";
			spdlog::info("[PacketHandler::Handle_CS_Login_Request] Query returned no results.");
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
	vector<shared_ptr<Buffer>> sendBuffer = PacketHandler::MakeSendBuffer(sendLoginResponsePacket, PacketId::PKT_SC_LOGIN_RESPONSE);
	
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
			room->set_hostname(hostPlayerName);
		}

		vector<shared_ptr<Buffer>> sendBuffer = PacketHandler::MakeSendBuffer(roomListResponsePacket, PacketId::PKT_SC_ROOM_LIST_RESPONSE);
		
		GPlayerManager->PushJobSendData(session->GetSessionId(), sendBuffer);
	});
}

void PacketHandler::Handle_CS_Player_Info_Request(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service) {
	
	msgTest::CS_My_Player_Info_Request recvRequestPlayerInfoPacket;
	recvRequestPlayerInfoPacket.ParseFromArray(dataBuffer->GetBuffer(), dataBuffer->WriteSize());

	uint64 sessionId = recvRequestPlayerInfoPacket.sessionid();
	if (session->GetSessionId() != sessionId) {
		spdlog::info("Session ID missmatch");
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
		spdlog::info("Query returned no results.");
		return;
	}

	// send packet
	msgTest::SC_My_Player_Info_Response sendPlayerInfoResponsePacket;
	msgTest::Player* playerInfo = sendPlayerInfoResponsePacket.mutable_playerinfo();
	msgTest::Vector* position = playerInfo->mutable_position();
	
	playerInfo->set_playerid(sessionId);
	playerInfo->set_level(level);
	playerInfo->set_name(name);
	position->set_x(posX);
	position->set_y(posY);
	position->set_z(posZ);

	vector<shared_ptr<Buffer>> sendBuffer = MakeSendBuffer(sendPlayerInfoResponsePacket, PacketId::PKT_SC_MY_PLAYER_INFO_RESPONSE);

	GPlayerManager->PushJobSendData(session->GetSessionId(), sendBuffer);
}

void PacketHandler::Handle_CS_Room_Player_List_Request(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service) {
	
	msgTest::CS_Room_Player_List_Request recvRoomPlayerListRequestPacket;
	recvRoomPlayerListRequestPacket.ParseFromArray(dataBuffer->GetBuffer(), dataBuffer->WriteSize());

	int32 roomId = recvRoomPlayerListRequestPacket.roomid();
	
	GRoomManager->PushJobGetRoomPlayerList(roomId, [session](vector<RoomPlayerData> roomPlayerList) {

		msgTest::SC_Room_Player_List_Response sendRoomPlayerListResponsePacket;
		for (const auto& roomPlayer : roomPlayerList) {
			msgTest::Player* player = sendRoomPlayerListResponsePacket.add_playerlist();
			msgTest::Vector* position = player->mutable_position();
			string name = boost::locale::conv::utf_to_utf<char>(roomPlayer._name);
			
			player->set_playerid(roomPlayer._sessionId);
			player->set_name(name);
			player->set_level(roomPlayer._stats._level);
			position->set_x(roomPlayer._transform._position._x);
			position->set_y(roomPlayer._transform._position._y);
			position->set_z(roomPlayer._transform._position._z);
		}

		vector<shared_ptr<Buffer>> sendBuffer = MakeSendBuffer(sendRoomPlayerListResponsePacket, PacketId::PKT_SC_ROOM_PLAYER_LIST_RESPONSE);

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
	initRoomInfo._maxPlayerCount = 100;

	RoomPlayerData hostPlayerData;
	hostPlayerData._sessionId = session->GetSessionId();
	hostPlayerData._name = hostName;

	GRoomManager->PushJobCreateAndPushRoom(initRoomInfo, hostPlayerData);
}

void PacketHandler::Handle_CS_Enter_Room_Request(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service) {

	msgTest::CS_Enter_Room_Request recvEnterRoomRequestPacket;
	recvEnterRoomRequestPacket.ParseFromArray(dataBuffer->GetBuffer(), dataBuffer->WriteSize());

	int32 _enterRoomId = recvEnterRoomRequestPacket.roomid();

	GPlayerManager->PushJobGetRoomPlayer(session->GetSessionId(), [_enterRoomId](PlayerBaseInfo baseInfo, PlayerTransform position) {
		
		RoomPlayerData roomPlayer;
		roomPlayer._sessionId = baseInfo._sessionId;
		roomPlayer._transform._moveTimeStamp = position._lastmoveTimestamp;
		roomPlayer._name = baseInfo._name;
		roomPlayer._transform._position = position._position;
		roomPlayer._transform._updatePosition = true;
		roomPlayer._transform._velocity = position._velocity;

		GRoomManager->PushJobEnterRoom(_enterRoomId, roomPlayer);
	});
}

void PacketHandler::Handle_CS_Player_Move_Request(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service){

	msgTest::CS_Player_Move_Request recvPlayerMoveReqeustPacket;
	recvPlayerMoveReqeustPacket.ParseFromArray(dataBuffer->GetBuffer(), dataBuffer->WriteSize());

	msgTest::MoveState moveState = recvPlayerMoveReqeustPacket.movestate();
	RoomPlayerData roomPlayerData;
	roomPlayerData._sessionId = session->GetSessionId();
	roomPlayerData._transform._moveTimeStamp = moveState.timestamp();
	roomPlayerData._transform._position = { (int16)moveState.position().x(), (int16)moveState.position().y(), (int16)moveState.position().z() };
	roomPlayerData._transform._velocity = { (int16)moveState.velocity().x(), (int16)moveState.velocity().y(), (int16)moveState.velocity().z() };
	roomPlayerData._transform._rotation = { (int16)moveState.rotation().x(), (int16)moveState.rotation().y(), (int16)moveState.rotation().z() };
	roomPlayerData._transform._updatePosition = true;

	GRoomManager->PushJobMovePlayer(moveState.roomid(), roomPlayerData);
}

void PacketHandler::Handle_CS_Enter_Room_Complete(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service){

	GRoomManager->PushJobEnterRoomComplete(session->GetSessionId());
}

void PacketHandler::Handle_CS_Skill_Use(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service){

	msgTest::CS_Skill_Use recvPacketSkillUse;
	recvPacketSkillUse.ParseFromArray(dataBuffer->GetBuffer(), dataBuffer->WriteSize());



	GRoomManager->PushJobSkillUse();
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

void PacketHandler::Handle_SC_Create_Room_Response(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* serviec){

}

void PacketHandler::Handle_SC_Player_Enter_Room_Notification(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* serviec) {

}

void PacketHandler::Handle_SC_Player_Move_Notification(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* serviec) {

}

void PacketHandler::Handle_SC_Player_List_In_Grid(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* serviec) {

}

void PacketHandler::Handle_SC_Skill_Result(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service) {


}

void PacketHandler::Handle_SC_Skill_Cast(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service) {


}