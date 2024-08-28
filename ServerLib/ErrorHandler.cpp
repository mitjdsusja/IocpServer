#include "pch.h"
#include "ErrorHandler.h"

void ErrorHandler::HandleError(wstring msg, int32 err){
	wcout << "[ERROR] " << msg << " : " << err << endl;
}
