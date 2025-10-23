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
	packetHandleArray[PKT_SC_PLAYER_LEAVE_GRID_NOTIFICATION] = Handle_SC_Player_Leave_Grid_Notification;
	packetHandleArray[PKT_SC_PLAYER_ENTER_GRID_NOTIFICATION] = Handle_SC_Player_Enter_Grid_Notification;
}

void PacketHandler::HandlePacket(shared_ptr<GameSession> session, const PacketContext& packetContext, Service* service) {

	packetHandleArray[packetContext.header.packetId](session, packetContext, service);
}

void PacketHandler::Handle_Invalid(shared_ptr<GameSession> session, const PacketContext& packetContext, Service* service) {

	spdlog::info("[PacketHandler::Handle_Invalid] INVALID PACKET - packetId : {}", packetContext.header.packetId);
}




/*------------
	C -> S
-------------*/
void PacketHandler::Handle_CS_Ping(shared_ptr<GameSession> session, const PacketContext& packetContext, Service* service) {
}

void PacketHandler::Handle_CS_Login_Request(shared_ptr<GameSession> session, const PacketContext& packetContext, Service* service) {
}

void PacketHandler::Handle_CS_Room_List_Request(shared_ptr<GameSession> session, const PacketContext& packetContext, Service* service) {
}

void PacketHandler::Handle_CS_Player_Info_Request(shared_ptr<GameSession> session, const PacketContext& packetContext, Service* service) {
}

void PacketHandler::Handle_CS_Room_Player_List_Request(shared_ptr<GameSession> session, const PacketContext& packetContext, Service* service) {
}

void PacketHandler::Handle_CS_Create_Room_Request(shared_ptr<GameSession> session, const PacketContext& packetContext, Service* service) {
}

void PacketHandler::Handle_CS_Enter_Room_Request(shared_ptr<GameSession> session, const PacketContext& packetContext, Service* service) {
}

void PacketHandler::Handle_CS_Player_Move_Request(shared_ptr<GameSession> session, const PacketContext& packetContext, Service* service) {
}



/*------------
	S -> C
-------------*/
void PacketHandler::Handle_SC_Pong(shared_ptr<GameSession> session, const PacketContext& packetContext, Service* service) {

	msgTest::SC_Pong recvPacketPong;
	recvPacketPong.ParseFromArray(packetContext.GetDataPtr(), packetContext.GetDataSize());

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

void PacketHandler::Handle_SC_Login_Response(shared_ptr<GameSession> session, const PacketContext& packetContext, Service* service) {

	msgTest::SC_Login_Response recvLoginResponsePacket;
	recvLoginResponsePacket.ParseFromArray(packetContext.GetDataPtr(), packetContext.GetDataSize());

	if (recvLoginResponsePacket.success() == true) {

		GPlayerManager->CreatePlayerAndAdd(session, session->GetSessionId());
		spdlog::info("[PacketHandler::Handle_SC_Login_Response] Login Success : bot" + to_string(session->GetSessionId()));
	}
	else {

		spdlog::info("[PacketHandler::Handle_SC_Login_Response] Login Fail ");
	}
}

void PacketHandler::Handle_SC_Room_List_Response(shared_ptr<GameSession> session, const PacketContext& packetContext, Service* service) {

	msgTest::SC_Room_List_Response recvRoomListResponse;
	recvRoomListResponse.ParseFromArray(packetContext.GetDataPtr(), packetContext.GetDataSize());

	vector<RoomInfo> roomList(recvRoomListResponse.roomlist_size());
	spdlog::info("Recv Room List : {}", recvRoomListResponse.roomlist_size());

	for (int i = 0; i < recvRoomListResponse.roomlist_size(); ++i) {

		msgTest::Room room = recvRoomListResponse.roomlist(i);
		roomList[i]._roomId = room.roomid();
	}

	GGameManager->SetEnterableRoomList(roomList);
}

void PacketHandler::Handle_SC_Player_Info_Response(shared_ptr<GameSession> session, const PacketContext& packetContext, Service* service) {


}

void PacketHandler::Handle_SC_Player_List_Response(shared_ptr<GameSession> session, const PacketContext& packetContext, Service* service) {


}

void PacketHandler::Handle_SC_Enter_Room_Response(shared_ptr<GameSession> session, const PacketContext& packetContext, Service* service) {

	msgTest::SC_Enter_Room_Response recvPacketEnterRoomResponse;
	recvPacketEnterRoomResponse.ParseFromArray(packetContext.GetDataPtr(), packetContext.GetDataSize());
	
	msgTest::Room room = recvPacketEnterRoomResponse.room();
	int32 roomId = room.roomid();
	string roomName = room.roomname();

	GPlayerManager->SetEnterRoomId(session->GetSessionId(), roomId);
	GGameManager->AddEnterPlayerCount();
	//spdlog::info("Cur Enter Player Count : {}", GGameManager->GetEnteredPlayerCount());

	// Ready to Move
	{
		msgTest::CS_Enter_Room_Complete sendPacketEnterRoomComplete;
		
		auto sendBuffers = PacketHandler::MakeSendBuffer(sendPacketEnterRoomComplete, PKT_CS_ENTER_ROOM_COMPLETE);

		for (auto& buffer : sendBuffers) {

			session->Send(buffer);
		}
	}
}

void PacketHandler::Handle_SC_Create_Room_Response(shared_ptr<GameSession> session, const PacketContext& packetContext, Service* serviec) {

	msgTest::SC_Create_Room_Response recvPacketCreateRoomResponse;
	recvPacketCreateRoomResponse.ParseFromArray(packetContext.GetDataPtr(), packetContext.GetDataSize());

	bool success = recvPacketCreateRoomResponse.success();

	if (success == false) {

		string errorMsg = recvPacketCreateRoomResponse.errormessage();
		spdlog::info("CreateRoom Fail : {}", errorMsg);
		return;
	}

	GPlayerManager->SetEnterRoomId(session->GetSessionId(), recvPacketCreateRoomResponse.room().roomid());
	GGameManager->AddEnterPlayerCount();
	GGameManager->_createRoomPlayerSessionCount.fetch_add(1);
}

void PacketHandler::Handle_SC_Player_Enter_Room_Notification(shared_ptr<GameSession> session, const PacketContext& packetContext, Service* serviec) {

}

void PacketHandler::Handle_SC_Player_Move_Notification(shared_ptr<GameSession> session, const PacketContext& packetContext, Service* serviec) {

}

void PacketHandler::Handle_SC_Player_List_In_Grid(shared_ptr<GameSession> session, const PacketContext& packetContext, Service* serviec) {

}

void PacketHandler::Handle_SC_Player_Leave_Grid_Notification(shared_ptr<GameSession> session, const PacketContext& packetContext, Service* serviec){

}

void PacketHandler::Handle_SC_Player_Enter_Grid_Notification(shared_ptr<GameSession> session, const PacketContext& packetContext, Service* serviec) {

}
