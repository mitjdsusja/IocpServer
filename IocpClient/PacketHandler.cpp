#include "pch.h"
#include "Global.h"

#include "PacketHandler.h"
#include "ServerGlobal.h"
#include <boost/locale.hpp>

#include "BufferPool.h"
#include "Service.h"

#include "messageTest.pb.h"

#include "GameSession.h"
#include "Room.h"
#include "Player.h"
#include "GameManager.h"

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
	packetHandleArray[PKT_SC_PONG] = Handle_SC_Pong;
	packetHandleArray[PKT_SC_ROOM_LIST_RESPONSE] = Handle_SC_Room_List_Response;
	packetHandleArray[PKT_SC_MY_PLAYER_INFO_RESPONSE] = Handle_SC_Player_Info_Response;
	packetHandleArray[PKT_SC_ROOM_PLAYER_LIST_RESPONSE] = Handle_SC_Player_List_Response;
	packetHandleArray[PKT_SC_ENTER_ROOM_RESPONSE] = Handle_SC_Enter_Room_Response;
	packetHandleArray[PKT_SC_CREATE_ROOM_RESPONSE] = Handle_SC_Create_Room_Response;
	packetHandleArray[PKT_SC_PLAYER_ENTER_ROOM_NOTIFICATION] = Handle_SC_Player_Enter_Room_Notification;
	packetHandleArray[PKT_SC_PLAYER_MOVE_NOTIFICATION] = Handle_SC_Player_Move_Notification;
	packetHandleArray[PKT_SC_PLAYER_LIST_IN_GRID] = Handle_SC_Player_List_In_Grid;

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

	spdlog::info("INVALID PACKET ID : {}", header->packetId);
	//ErrorHandler::HandleError(L"INVALID PACKET ID", header->packetId);
}




/*------------
	C -> S
-------------*/
void PacketHandler::Handle_CS_Ping(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service) {
}

void PacketHandler::Handle_CS_Login_Request(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service) {
}

void PacketHandler::Handle_CS_Room_List_Request(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service) {
}

void PacketHandler::Handle_CS_Player_Info_Request(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service) {
}

void PacketHandler::Handle_CS_Room_Player_List_Request(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service) {
}

void PacketHandler::Handle_CS_Create_Room_Request(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service) {
}

void PacketHandler::Handle_CS_Enter_Room_Request(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service) {
}

void PacketHandler::Handle_CS_Player_Move_Request(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service) {
}



/*------------
	S -> C
-------------*/
void PacketHandler::Handle_SC_Pong(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service) {

	msgTest::SC_Pong recvPacketPong;
	recvPacketPong.ParseFromArray(dataBuffer->GetBuffer(), dataBuffer->WriteSize());

	uint64 pingSendTime = recvPacketPong.timestamp();
	uint64 serverTimestampMs = recvPacketPong.servertimestamp();
	uint64 curClientTime = GGameManager->GetNowClientTimeMs();
	uint64 rtt = curClientTime - pingSendTime;

	if (pingSendTime > curClientTime) {
		
		spdlog::info("INVALID RTT : {}", rtt);
		return;
	}

	int64 serverTimeOffsetMs = (serverTimestampMs + (rtt / 2)) - curClientTime;

	GGameManager->SetServerTimeOffsetMs(serverTimeOffsetMs);
	
	spdlog::info("RTT : {}, Cur ServerTime: {}", rtt, GGameManager->GetNowServerTimeMs());
}

void PacketHandler::Handle_SC_Login_Response(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service) {

	msgTest::SC_Login_Response recvLoginResponsePacket;
	recvLoginResponsePacket.ParseFromArray(dataBuffer->GetBuffer(), dataBuffer->WriteSize());

	if (recvLoginResponsePacket.success() == true) {

		GPlayerManager->CreatePlayerAndAdd(session, recvLoginResponsePacket.sessionid());
		spdlog::info("[PacketHandler::Handle_SC_Login_Response] Login Success ");
	}
	else {

		spdlog::info("[PacketHandler::Handle_SC_Login_Response] Login Fail ");
	}
}

void PacketHandler::Handle_SC_Room_List_Response(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service) {

	msgTest::SC_Room_List_Response recvRoomListResponse;
	recvRoomListResponse.ParseFromArray(dataBuffer->GetBuffer(), dataBuffer->WriteSize());

	vector<RoomInfo> roomList(recvRoomListResponse.roomlist_size());
	spdlog::info("Recv Room List : {}", recvRoomListResponse.roomlist_size());

	for (int i = 0; i < recvRoomListResponse.roomlist_size(); ++i) {

		msgTest::Room room = recvRoomListResponse.roomlist(i);
		roomList[i]._roomId = room.roomid();
	}

	GGameManager->SetEnterableRoomList(roomList);
}

void PacketHandler::Handle_SC_Player_Info_Response(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service) {


}

void PacketHandler::Handle_SC_Player_List_Response(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service) {


}

void PacketHandler::Handle_SC_Enter_Room_Response(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* service) {

	msgTest::SC_Enter_Room_Response recvPacketEnterRoomResponse;
	recvPacketEnterRoomResponse.ParseFromArray(dataBuffer->GetBuffer(), dataBuffer->WriteSize());
	
	msgTest::Room room = recvPacketEnterRoomResponse.room();
	int32 roomId = room.roomid();
	string roomName = room.roomname();

	GGameManager->AddEnterPlayerCount();
	spdlog::info("Cur Enter Player Count : {}", GGameManager->GetEnteredPlayerCount());
}

void PacketHandler::Handle_SC_Create_Room_Response(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* serviec) {

}

void PacketHandler::Handle_SC_Player_Enter_Room_Notification(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* serviec) {

}

void PacketHandler::Handle_SC_Player_Move_Notification(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* serviec) {

}

void PacketHandler::Handle_SC_Player_List_In_Grid(shared_ptr<GameSession> session, shared_ptr<Buffer> dataBuffer, Service* serviec) {

}