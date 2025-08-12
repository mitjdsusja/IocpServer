#include "pch.h"
#include "Global.h"

#include "Player.h"
#include "GameManager.h"

void GameManager::ReqeustEnterRoomAllPlayer()
{
    uint32 playerCount = GPlayerManager->GetPlayerCount();
    uint32 roomCount = _roomList.size();

    for (uint32 i = roomCount; i < playerCount; ++i)
    {
        uint32 roomIndex = i % roomCount; 
        GPlayerManager->RequestEnterRoom(i, _roomList[roomIndex]._roomId);
    }
}

void GameManager::RequestEnterRoomAllPlayer(int32 roomId){

    GPlayerManager->AllPlayerRequestEnterRoom(roomId);
}

void GameManager::ReqeustCreateRoom(int32 createRoomCount){

    GPlayerManager->RequestCreateRoom(createRoomCount);
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

uint64 GameManager::GetNowServerTimeMs(){

    return GetNowClientTimeMs() + _serverTimeOffsetMs;
}

uint64 GameManager::GetNowClientTimeMs(){

    return chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - _GClientStartTimePoint).count();
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
