#include "pch.h"

#include <windows.h>

#include "ThreadManager.h"

#include "Service.h"
#include "PacketHandler.h"
#include "JobQueue.h"
#include "JobTimer.h"

#include "messageTest.pb.h"

#include "GameSession.h"

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

	//// Reserve User Position 
	//GJobTimer->Reserve(100, [serverService]() {
	//	ReserveLoopBroadcastUserInfo(serverService);
	//});

	//GThreadManager->Join();
}

//void ReserveLoopBroadcastUserInfo(Service* service) {
//	//cout << "Start Broadcast" << endl;
//
//	msgTest::SC_Broadcast_User_Info packetBroadcastUserInfo;
//
//	vector<UserInfo> userInfoList;
//	service->GetUsersInfo(userInfoList);
//
//	for (UserInfo& userInfo : userInfoList) {
//		msgTest::MoveState* moveState = packetBroadcastUserInfo.add_movestates();
//		moveState->set_userid(userInfo.GetId());
//		moveState->set_timestamp(userInfo.GetLastMovePacket());
//
//		msgTest::Position* position = moveState->mutable_position();
//		position->set_x(userInfo.GetPosition().x);
//		position->set_y(userInfo.GetPosition().y);
//		position->set_z(userInfo.GetPosition().z);
//
//		msgTest::Veloccity* velocity = moveState->mutable_velocity();
//		velocity->set_x(userInfo.GetVelocity().x);
//		velocity->set_y(userInfo.GetVelocity().y);
//		velocity->set_z(userInfo.GetVelocity().z);
//
//		//cout << "userId : " << moveState->userid() << " ";
//		//cout << "Pos : " << moveState->position().x() << ", " << moveState->position().z() << endl;
//	}
//	shared_ptr<Buffer> sendBuffer = PacketHandler::MakeSendBuffer(packetBroadcastUserInfo, PacketId::PKT_SC_BROADCAST_USER_INFO);
//	service->Broadcast(sendBuffer);
//
//	GJobTimer->Reserve(100, [service]() {
//		ReserveLoopBroadcastUserInfo(service);
//	});
//}