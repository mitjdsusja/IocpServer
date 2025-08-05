#include "pch.h"
#include "Global.h"

#include "Player.h"
#include "GameManager.h"

void GameManager::RequestEnterRoomAllPlayer(int32 roomId){

    GPlayerManager->RequestEnterRoomAllPlayer(roomId);
}

vector<RoomInfo> GameManager::GetEnterableRoomList(){

    return _roomList;
}

int32 GameManager::GetEnteredPlayerCount(){

    return _enteredPlayerCount;
}

void GameManager::SetEnterableRoomList(const vector<RoomInfo>& roomList){

    _roomList = roomList;
}

void GameManager::AddEnterPlayerCount(){

    _enteredPlayerCount.fetch_add(1);
}
