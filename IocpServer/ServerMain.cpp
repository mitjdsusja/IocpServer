#include "pch.h"

#include <windows.h>

#include "ThreadManager.h"

#include "Service.h"
#include "PacketHandler.h"
#include "Actor.h"
#include "TimedJob.h"
#include "JobScheduler.h"

#include "messageTest.pb.h"

#include "Global.h"
#include "GameSession.h"
#include "RoomManager.h"
#include "MonitorManager.h"
#include "ActorManager.h"

#include <boost/locale.hpp>

// Job Life Cycle => shared_ptr

enum {
	GQCS_THREAD_COUNT = 2,
	WORKER_THREAD_COUNT = 5,
};

void DBInsertuser(wstring id, wstring pw, wstring name);

int main() {
	wcout.imbue(std::locale("kor"));

	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	
	//ServerService* serverService = new ServerService(NetAddress(L"127.0.0.1", 7777), 100);
	ServerService* serverService = new ServerService(NetAddress(L"192.168.0.14", 7777), 10, []() { return make_shared<GameSession>(nullptr); });
	PacketHandler::RegisterPacketHandlers();

	while (true) {
		if (GMonitorManager->CreateMonitorProcess() == true) {
			break;
		}
		else {
			this_thread::sleep_for(1s);
		}
	}
	while (true) {
		if (GMonitorManager->ConnectPipe() == true) {
			break;
		}
		else {
			this_thread::sleep_for(1s);
		}
	}

	serverService->Start();
	for (int32 i = 0; i < (int32)GQCS_THREAD_COUNT; i++) {
		spdlog::info("GQCS Thread Start");

		GThreadManager->Launch([=]() {
			while (true) {
				serverService->CompletionEventThread(10);
				GJobScheduler->CheckTimedJob();
				//GJobTimer->EnqueueReadyJobs(*GJobQueue);
			}
		});
	}

	for (int32 i = 0; i < (int32)WORKER_THREAD_COUNT; i++) {
		spdlog::info("Job Thread Start");

		GThreadManager->Launch([=]() {
			while (true) {
				try {
					shared_ptr<Actor> actor = GJobScheduler->PopActor();
					actor->ExecuteJob();
				}
				catch (exception& e) {
					spdlog::info("job Thread Exception : {}", e.what());
				}
				catch (...) {
					spdlog::info("Job Thread Unknown Exception : {}");
				}
				
			}
		});
	}
	
	while (true) {

		GActorManager->RequestAllLatencyAndSendToMonitor();

		this_thread::sleep_for(3s);
	}

	GThreadManager->Join();
}

void DBInsertuser(wstring id, wstring pw, wstring name) {

	std::wstring query = L"INSERT INTO users (id, password_hash, level, name, pos_x, pos_y, pos_z) VALUES (";
	query += L"'";

	query += id + L"','";
	query += pw + L"',";
	query += to_wstring(1) + L",'";
	query += name + L"',";
	query += to_wstring(0) + L",";
	query += to_wstring(100) + L",";
	query += to_wstring(0);

	query += L");";

	wcout << query << endl;
	LDBConnector->ExecuteQuery(query);
}
