#include "pch.h"

#include <windows.h>

#include "ThreadManager.h"

#include "Service.h"
#include "PacketHandler.h"
#include "JobQueue.h"
#include "JobTimer.h"

#include "messageTest.pb.h"

// Job Life Cycle => shared_ptr

enum {
	GQCS_THREAD_COUNT = 5,
};

void ReserveLoopBroadcastUserInfo(Service* service);

int main() {
	wcout.imbue(std::locale("kor"));

	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	
	//ServerService* serverService = new ServerService(NetAddress(L"127.0.0.1", 7777), 100);
	ServerService* serverService = new ServerService(NetAddress(L"192.168.0.14", 7777), 10);
	PacketHandler::Init();

	serverService->Start();
	// Create Thread GQCS
	for (int32 i = 0; i < (int32)sysInfo.dwNumberOfProcessors; i++) {
		cout << "GQCS Thread Start" << endl;

		GThreadManager->Launch([=]() {
			cout << "iocp thread : " << this_thread::get_id() << endl;
			while (true) {
				serverService->CompletionEventThread(10);
				GJobTimer->EnqueueReadyJobs(*GJobQueue);
			}
		});
	}

	for (int32 i = 0; i < (int32)3; i++) {
		cout << "Job Thread Start" << endl;

		GThreadManager->Launch([=]() {
			cout << "job gen thread : " << this_thread::get_id() << endl;
			while (true) {
				Job* job = GJobQueue->Pop();
				job->Execute();
				delete job;
			}
			});
	}

	/*GJobTimer->Reserve(1000, []() {
		cout << "JobTimerTest" << endl;
	});*/

	cout << "main thread : " << this_thread::get_id() << endl;

	// Reserve User Position 
	GJobTimer->Reserve(100, [serverService]() {
		ReserveLoopBroadcastUserInfo(serverService);
	});

	GThreadManager->Join();
}

void ReserveLoopBroadcastUserInfo(Service* service) {
	//cout << "Start Broadcast" << endl;

	msgTest::SC_Broadcast_User_Info packetBroadcastUserInfo;
	{
		vector<UserInfo*> userInfoList;
		service->GetUsersInfo(userInfoList);

		for (UserInfo* userInfo : userInfoList) {
			msgTest::UserInfo* packetUsersInfo = packetBroadcastUserInfo.add_userinfos();
			msgTest::Position* position = packetUsersInfo->mutable_position();
			msgTest::Direction* direction = packetUsersInfo->mutable_direction();

			packetUsersInfo->set_id(userInfo->GetId());
			position->set_x(userInfo->GetPosition().x);
			position->set_y(userInfo->GetPosition().y);
			position->set_z(userInfo->GetPosition().z);
			direction->set_x(userInfo->GetDirection().x);
			direction->set_y(userInfo->GetDirection().y);
			direction->set_z(userInfo->GetDirection().z);

			//cout << "[SEND] " << userInfo->GetPosition().x << " " << userInfo->GetPosition().z << endl;
		}
	}
	shared_ptr<Buffer> sendBuffer = PacketHandler::MakeSendBuffer(packetBroadcastUserInfo, PacketId::PKT_SC_BROADCAST_USER_INFO);
	service->Broadcast(sendBuffer);

	GJobTimer->Reserve(100, [service]() {
		ReserveLoopBroadcastUserInfo(service);
	});
}