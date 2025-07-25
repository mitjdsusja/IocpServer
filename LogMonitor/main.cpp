#include <Windows.h>
#include <iostream>
#include <string>

using namespace std;

int main() {


}

void ClearConsole() {

}

void StartPipeServer() {

	HANDLE hPipe = CreateNamedPipeA(
		R"(\\. \pipe\monitorPipe)",
		PIPE_ACCESS_INBOUND,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
		1, 4096, 4096, 0, nullptr
	);

	if (hPipe == INVALID_HANDLE_VALUE) {
		cout << "파이프 생성 실패 : " << GetLastError() << endl;
		return;
	}

	bool connected = ConnectNamedPipe(hPipe, nullptr);
	if (connected == false) {
		cout << "서버 연결 실패 :" << GetLastError() << endl;
		return;
	}

	char buffer[4096];
	DWORD bytesRead;

	while (true) {
		if (ReadFile(hPipe, buffer, sizeof(buffer) - 1, &bytesRead, nullptr) && bytesRead > 0) {
			buffer[bytesRead] = '\0';
			cout << buffer << endl;

			ClearConsole();
		}
		else {
			break;
		}
	}
	
	CloseHandle(hPipe);
}