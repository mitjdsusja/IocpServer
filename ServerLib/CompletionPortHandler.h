#pragma once
class CompletionPortHandler{
public:
	CompletionPortHandler();

	bool RegisterHandle(HANDLE handle, ULONG_PTR key);
	void GetCompletionEvent();

	HANDLE GetHandle() { return _completionPort; }

private:
	HANDLE _completionPort = nullptr;
};

