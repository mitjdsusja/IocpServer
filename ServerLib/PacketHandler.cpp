#include "pch.h"
#include "PacketHandler.h"

void PacketHandler::HandlePacket(PacketHeader* buffer){
	switch (buffer->packetId) {

	case C_SEND:
		Handle_C_SEND(buffer);
		break;
	case S_SEND:
		Handle_S_SEND(buffer);
		break;
	default:
		ErrorHandler::HandleError(L"Not Defined PacketId");
		break;
	}
}

void PacketHandler::Handle_S_SEND(PacketHeader* buffer){

}

void PacketHandler::Handle_C_SEND(PacketHeader* buffer){

	Packet_C_SEND* packet = (Packet_C_SEND*)buffer;
	
	cout << "Packet ID : " << packet->id << endl;
	cout << "Packet HP : " << packet->hp << endl;
	cout << "Packet MP : " << packet->mp << endl;
}
