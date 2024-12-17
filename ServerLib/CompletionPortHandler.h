#pragma once
class CompletionPortHandler{
public:
	CompletionPortHandler();

	bool RegisterHandle(HANDLE handle, ULONG_PTR key = 0);
	void GetCompletionEvent(uint32 ms = INFINITE);

	HANDLE GetHandle() { return _completionPort; }

private:
	HANDLE _completionPort = nullptr;

};

