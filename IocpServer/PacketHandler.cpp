#include "pch.h"
#include "PacketHandler.h"
#include "ServerGlobal.h"

#include "Job.h"
#include "JobQueue.h"
#include "BufferPool.h"
#include "JobTimer.h"
#include "Service.h"

#include "messageTest.pb.h"

//TODO : Mapping Function

void PacketHandler::RegisterPacketHandlers() {

	for (int32 i = 0; i < UINT16_MAX; i++) {
		packetHandleArray[i] = Handle_Invalid;
	}

	/*------------
		C -> S
	-------------*/
	packetHandleArray[PKT_CS_LOGIN] = Handle_CS_Login;
	packetHandleArray[PKT_CS_REQUEST_ROOM_LIST] = Handle_CS_Request_Room_List;
	packetHandleArray[PKT_CS_REQUEST_USER_INFO] = Handle_CS_Request_User_Info;
	packetHandleArray[PKT_CS_REQUEST_USER_LIST] = Handle_CS_Request_User_List;
	packetHandleArray[PKT_CS_ENTER_ROOM] = Handle_CS_Enter_Room;


	/*------------
		S -> C
	-------------*/
	packetHandleArray[PKT_SC_LOGIN_SUCCESS] = Handle_SC_Login_Success;
	packetHandleArray[PKT_SC_LOGIN_FAIL] = Handle_SC_Login_Fail;
	packetHandleArray[PKT_SC_RESPONSE_ROOM_LIST] = Handle_SC_Response_Room_List;
	packetHandleArray[PKT_SC_RESPONSE_USER_INFO] = Handle_SC_Response_User_Info;
	packetHandleArray[PKT_SC_RESPONSE_USER_LIST] = Handle_SC_Response_User_List;
	packetHandleArray[PKT_SC_ENTER_ROOM_SUCCESS] = Handle_SC_Enter_Room_Success;
	packetHandleArray[PKT_SC_ENTER_ROOM_FAIL] = Handle_SC_Enter_Room_Fail;

}

void PacketHandler::HandlePacket(shared_ptr<Session> session, PacketHeader* dataBuffer, Service* service) {

	shared_ptr<Buffer> buffer = shared_ptr<Buffer>(GSendBufferPool->Pop(), [](Buffer* buffer) { GSendBufferPool->Push(buffer); });

	BYTE* data = ((BYTE*)dataBuffer) + sizeof(PacketHeader);
	int32 dataSize = dataBuffer->GetDataSize();

	memcpy(buffer->GetBuffer(), data, dataSize);
	buffer->Write(dataBuffer->packetSize);

	int32 packetId = dataBuffer->packetId;

	// push jobQueue
	Job* job = new Job([session, buffer, service, packetId]() {
		packetHandleArray[packetId](session, buffer, service);
		});
	GJobQueue->Push(job);
}

void PacketHandler::Handle_Invalid(shared_ptr<Session> session, shared_ptr<Buffer> buffer, Service* service) {

	PacketHeader* header = (PacketHeader*)buffer->GetBuffer();

	ErrorHandler::HandleError(L"INVALID PACKET ID", header->packetId);
}


/*------------
	C -> S
-------------*/
void PacketHandler::Handle_CS_Login(shared_ptr<Session> session, shared_ptr<Buffer> dataBuffer, Service* service) {
	
	// Check Id, Passwd
	msgTest::CS_Login recvLoginPacket;
	recvLoginPacket.ParseFromArray(dataBuffer->GetBuffer(), dataBuffer->WriteSize());
	
	string id = recvLoginPacket.id();
	string pw = recvLoginPacket.passwd();
	cout << "[Login] ID : " << id << " " << "PW : " << pw << endl;

	bool userExists = false;
	{
		// database
		wstring wId(id.begin(), id.end());
		wstring wPw(pw.begin(), pw.end());

		std::wstring query = L"SELECT COUNT(*) FROM USERS WHERE id = '" + wId + L"' AND password_hash = '" + wPw + L"';";
		vector<vector<wstring>> result = LDBConnector->ExecuteSelectQuery(query);

		if (!result.empty() && !result[0].empty()) {
			int count = std::stoi(result[0][0]); // 문자열을 숫자로 변환
			if (count > 0) {
				//wcout << L"User found!" << endl;
				userExists = true;
			}
			else {
				wcout << L"Invalid ID or password." << endl;
			}
		}
		else {
			wcout << L"Query returned no results." << endl;
		}

	}

	shared_ptr<Buffer> sendBuffer;
	if (userExists == true) {
		msgTest::SC_Login_Success sendLoginSuccessPacket;
		sendLoginSuccessPacket.set_sessionid(session->GetSessionId());
		sendBuffer = PacketHandler::MakeSendBuffer(sendLoginSuccessPacket, PacketId::PKT_SC_LOGIN_SUCCESS);
	}
	else {
		msgTest::SC_Login_Fail sendLoginFailPacket;
		sendLoginFailPacket.set_errorcode(1);
		sendBuffer = PacketHandler::MakeSendBuffer(sendLoginFailPacket, PacketId::PKT_SC_LOGIN_FAIL);
	}

	// Send Result
	Job* job = new Job([session, sendBuffer]() {
		session->Send(sendBuffer);
	});
	GJobQueue->Push(job);
}
void PacketHandler::Handle_CS_Request_Room_List(shared_ptr<Session> session, shared_ptr<Buffer> dataBuffer, Service* service) {

}
void PacketHandler::Handle_CS_Request_User_Info(shared_ptr<Session> session, shared_ptr<Buffer> dataBuffer, Service* service) {

}
void PacketHandler::Handle_CS_Request_User_List(shared_ptr<Session> session, shared_ptr<Buffer> dataBuffer, Service* service) {

}
void PacketHandler::Handle_CS_Enter_Room(shared_ptr<Session> session, shared_ptr<Buffer> dataBuffer, Service* service) {

}


/*------------
	S -> C
-------------*/
void PacketHandler::Handle_SC_Login_Success(shared_ptr<Session> session, shared_ptr<Buffer> dataBuffer, Service* service) {

}
void PacketHandler::Handle_SC_Login_Fail(shared_ptr<Session> session, shared_ptr<Buffer> dataBuffer, Service* service) {

}
void PacketHandler::Handle_SC_Response_Room_List(shared_ptr<Session> session, shared_ptr<Buffer> dataBuffer, Service* service) {

}
void PacketHandler::Handle_SC_Response_User_Info(shared_ptr<Session> session, shared_ptr<Buffer> dataBuffer, Service* service) {

}
void PacketHandler::Handle_SC_Response_User_List(shared_ptr<Session> session, shared_ptr<Buffer> dataBuffer, Service* service) {

}
void PacketHandler::Handle_SC_Enter_Room_Success(shared_ptr<Session> session, shared_ptr<Buffer> dataBuffer, Service* service) {

}
void PacketHandler::Handle_SC_Enter_Room_Fail(shared_ptr<Session> session, shared_ptr<Buffer> dataBuffer, Service* service) {

}



