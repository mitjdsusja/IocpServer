#include "pch.h"
#include "MonitorManager.h"

#include <Windows.h>

bool MonitorManager::CreateMonitorProcess(){

    STARTUPINFOW si = {};
    PROCESS_INFORMATION pi = {};

    si.cb = sizeof(si);

    wstring exePath = L"LogMonitor.exe";

    bool success = CreateProcess(
        nullptr,
        &exePath[0],
        nullptr, nullptr, false,
        CREATE_NEW_CONSOLE,
        nullptr, nullptr, &si, &pi
    );

    if (success == false) {
        wcout << "Monitor ���� ���� :" << GetLastError() << endl;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return success;
}

bool MonitorManager::ConnectPipe(){

    const wstring pipeName = L"\\\\.\\pipe\\monitorPipe";

    hPipe = CreateFileW(
        pipeName.c_str(),
        GENERIC_WRITE,
        0, nullptr,
        OPEN_EXISTING,
        0, nullptr
    );

    if (hPipe == INVALID_HANDLE_VALUE) {
        wcout << L"������ ���� ���� : " << GetLastError() << endl;
        return false;
    }

    return true;
}

void MonitorManager::PushJobSendMsg(const wstring& sendMessage){

    shared_ptr<MonitorManager> self = static_pointer_cast<MonitorManager>(shared_from_this());

    unique_ptr<Job> job = make_unique<Job>([self, sendMessage]() {
        self->SendMsg(sendMessage);
    });

    PushJob(move(job));
}

void MonitorManager::SendMsg(const wstring& sendMessage){

    DWORD bytesWritten = 0;

    bool result = WriteFile(
        hPipe,
        sendMessage.c_str(),
        static_cast<DWORD>(sendMessage.size()),
        &bytesWritten,
        nullptr
    );

    if (result == false) {
        wcout << "[MonitorManager::SendMsg] Send Fail : " << GetLastError() << endl;
    }
}
