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
	case C_MOVE:
		Handle_C_MOVE(buffer);
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

void PacketHandler::Handle_C_MOVE(PacketHeader* buffer){
	Packet_C_MOVE* packet = (Packet_C_MOVE*)buffer;

	cout << "Packet posX : " << packet->posX << endl;
	cout << "Packet posY : " << packet->posY << endl;
	cout << "Packet posZ : " << packet->posZ << endl;
}
