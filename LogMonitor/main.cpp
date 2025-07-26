#include <Windows.h>
#include <iostream>
#include <string>

using namespace std;

void ClearConsole();
void StartPipe();

int main() {

	StartPipe();
}

void ClearConsole() {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD consoleSize, charsWritten;

	GetConsoleScreenBufferInfo(hConsole, &csbi);
	consoleSize = csbi.dwSize.X * csbi.dwSize.Y;

	FillConsoleOutputCharacter(hConsole, ' ', consoleSize, { 0, 0 }, &charsWritten);
	FillConsoleOutputAttribute(hConsole, csbi.wAttributes, consoleSize, { 0, 0 }, &charsWritten);
	SetConsoleCursorPosition(hConsole, { 0, 0 });
}

void StartPipe() {

	HANDLE hPipe = CreateNamedPipeA(
		R"(\\.\pipe\monitorPipe)",
		PIPE_ACCESS_INBOUND,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
		1, 4096, 4096, 0, nullptr
	);

	if (hPipe == INVALID_HANDLE_VALUE) {
		wcout << "파이프 생성 실패 : " << GetLastError() << endl;
		return;
	}

	bool connected = ConnectNamedPipe(hPipe, nullptr);
	if (connected == false) {
		wcout << "서버 연결 실패 :" << GetLastError() << endl;
		return;
	}

	wchar_t buffer[4096];
	DWORD bytesRead;

	while (true) {
		if (ReadFile(hPipe, buffer, sizeof(buffer) - 1, &bytesRead, nullptr) && bytesRead > 0) {
			buffer[bytesRead] = '\0';
			ClearConsole();

			wcout << buffer << endl;
		}
		else {
			break;
		}
	}
	
	CloseHandle(hPipe);
}