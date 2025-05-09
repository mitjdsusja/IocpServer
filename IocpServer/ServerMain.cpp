#include "pch.h"

#include <windows.h>

#include "ThreadManager.h"

#include "Service.h"
#include "PacketHandler.h"
#include "JobQueue.h"
#include "JobTimer.h"

#include "messageTest.pb.h"

#include "Global.h"
#include "GameSession.h"
#include "RoomManager.h"

#include <boost/locale.hpp>

// Job Life Cycle => shared_ptr

enum {
	GQCS_THREAD_COUNT = 5,
};

void ReserveLoopBroadcastUserInfo(Service* service);
void ReservePrintJobQueueTime();
void DBInsertuser(wstring id, wstring pw, wstring name);

int main() {
	wcout.imbue(std::locale("kor"));

	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	
	//ServerService* serverService = new ServerService(NetAddress(L"127.0.0.1", 7777), 100);
	ServerService* serverService = new ServerService(NetAddress(L"192.168.0.14", 7777), 10, []() { return make_shared<GameSession>(nullptr); });
	PacketHandler::RegisterPacketHandlers();


	serverService->Start();
	for (int32 i = 0; i < (int32)sysInfo.dwNumberOfProcessors; i++) {
		cout << "GQCS Thread Start" << endl;

		GThreadManager->Launch([=]() {
			while (true) {
				serverService->CompletionEventThread(10);
				GJobTimer->EnqueueReadyJobs(*GJobQueue);
			}
		});
	}

	for (int32 i = 0; i < (int32)3; i++) {
		cout << "Job Thread Start" << endl;

		GThreadManager->Launch([=]() {
			while (true) {
				Job* job = GJobQueue->Pop();
				job->Execute();
				delete job;
			}
		});
	}

	/*
	LDBConnector = new DBConnector();
	for (int i = 1; i <= 100; i++) {
		std::wstring num = std::to_wstring(i);
		DBInsertuser(L"bot" + num, L"bot" + num, L"bot" + num);
	}
	*/

	// Reserve User Position 
	GJobTimer->Reserve(100, [serverService]() {
		ReserveLoopBroadcastUserInfo(serverService);
	});
	GJobTimer->Reserve(1000, []() {
		ReservePrintJobQueueTime();
	});

	GThreadManager->Join();
}

void DBInsertuser(wstring id, wstring pw, wstring name) {

	std::wstring query = L"INSERT INTO users (id, password_hash, name) VALUES (";
	query += (L"'" + id + L"','" + pw + L"','" + name + L"');");

	wcout << query << endl;
	LDBConnector->ExecuteQuery(query);
}

void ReserveLoopBroadcastUserInfo(Service* service) {

	vector<RoomInfo> roomInfoList = GRoomManager->GetRoomInfoList();
	for (RoomInfo roomInfo : roomInfoList) {
		msgTest::SC_Player_Move_Notification sendPlayerMoveNotificationPacket;

		vector<PlayerInfo> playerInfoList = roomInfo._playerInfoList;
		bool needUserUpdateBroadcast = false;
		for (PlayerInfo playerInfo : playerInfoList) {

			if (playerInfo._isInfoUpdated == false) {
				continue;
			}
			else {
				needUserUpdateBroadcast = true;
			}

			msgTest::MoveState* moveState = sendPlayerMoveNotificationPacket.add_movestates();
			msgTest::Vector* position = moveState->mutable_position();
			msgTest::Vector* velocity = moveState->mutable_velocity();

			moveState->set_playername(boost::locale::conv::utf_to_utf<char>(playerInfo._name));
			position->set_x(playerInfo._position._x);
			position->set_y(playerInfo._position._y);
			position->set_z(playerInfo._position._z);
			velocity->set_x(playerInfo._velocity._x);
			velocity->set_y(playerInfo._velocity._y);
			velocity->set_z(playerInfo._velocity._z);
			moveState->set_timestamp(playerInfo._moveTimestamp);

			//wcout << playerInfo._name << " " << playerInfo._position._x << playerInfo._position._z << endl;
		}
		if (needUserUpdateBroadcast == false) continue;

		shared_ptr<Buffer> sendBuffer = PacketHandler::MakeSendBuffer(sendPlayerMoveNotificationPacket, PacketId::PKT_SC_PLAYER_MOVE_NOTIFICATION);

		Job* job = new Job([roomInfo, sendBuffer]() {
			GRoomManager->BroadcastToRoom(roomInfo._roomId, sendBuffer);
		});
		GJobQueue->Push(job);
	}
	

	GJobTimer->Reserve(100, [service]() {
		ReserveLoopBroadcastUserInfo(service);
	});
}

void ReservePrintJobQueueTime() {

	cout << "JobTime Avg : " << GJobQueue->GetJobCreateTimeAvg() << endl;

	GJobTimer->Reserve(1000, []() {
		ReservePrintJobQueueTime();
	});
}