#include "pch.h"

#include <windows.h>

#include "ThreadManager.h"

#include "Service.h"
#include "PacketHandler.h"
#include "JobQueue.h"
#include "JobTimer.h"
#include "JobScheduler.h"

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
void ReserveJobCreate();
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
				GJobScheduler->CheckTimedJob();
				//GJobTimer->EnqueueReadyJobs(*GJobQueue);
			}
		});
	}

	for (int32 i = 0; i < (int32)3; i++) {
		cout << "Job Thread Start" << endl;

		GThreadManager->Launch([=]() {
			while (true) {
				try {
					shared_ptr<JobQueueBase> jobQueue = GJobScheduler->PopJobQueue();
					jobQueue->ExecuteJob();
				}
				catch (exception& e) {
					cout << "job Thread Exception : " << e.what() << endl;
				}
				catch (...) {
					cout << "Job Thread Unknown Exception" << endl;
				}
				
			}
		});
	}

	//GJobTimer->Reserve(1000, []() {
	//	ReservePrintJobQueueInfo();
	//});
	

	GThreadManager->Join();
}

void DBInsertuser(wstring id, wstring pw, wstring name) {

	std::wstring query = L"INSERT INTO users (id, password_hash, name) VALUES (";
	query += (L"'" + id + L"','" + pw + L"','" + name + L"');");

	wcout << query << endl;
	LDBConnector->ExecuteQuery(query);
}

void ReserveLoopBroadcastUserInfo(Service* service) {

	/*GJobTimer->Reserve(100, [service]() {
		ReserveLoopBroadcastUserInfo(service);
	});*/
}


void ReserveJobCreate() {

	//Job* job = new Job([]() {
	//});

	//GJobQueue->Push(job);

	//GJobTimer->Reserve(100, []() {
	//	ReserveJobCreate();
	//});
}