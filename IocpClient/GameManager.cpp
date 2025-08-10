#include "pch.h"
#include "Global.h"

#include "Player.h"
#include "GameManager.h"

void GameManager::RequestEnterRoomAllPlayer(int32 roomId){

    GPlayerManager->AllPlayerRequestEnterRoom(roomId);
}

void GameManager::PlayerMovement(){

    GPlayerManager->AllPlayerRandomMove();
    GPlayerManager->AllPlayerSendMovePacket();
}

void GameManager::SendPingPacket(){

    GPlayerManager->SendPingPacketToFirstPlayer();
}

vector<RoomInfo> GameManager::GetEnterableRoomList(){

    return _roomList;
}

int32 GameManager::GetEnteredPlayerCount(){

    return _enteredPlayerCount;
}

int32 GameManager::GetEnterRoomId(){

    return _enterRoomId;
}

void GameManager::SetEnterableRoomList(const vector<RoomInfo>& roomList){

    _roomList = roomList;
}

void GameManager::AddEnterPlayerCount(){

    _enteredPlayerCount.fetch_add(1);
}

void GameManager::SetEnterRoomId(int32 roomId){

    _enterRoomId = roomId;
}

void GameManager::SetServerTimeOffsetMs(uint64 serverTimeOffsetMs){

}
