#include "pch.h"
#include "ErrorHandler.h"

void ErrorHandler::HandleError(wstring msg, int32 err){

	spdlog::info("[ERROR] {} : {}", boost::locale::conv::utf_to_utf<char>(msg), err);
	//wcout << "[ERROR] " << msg << " : " << err << endl;
}
