#include "Room.h"

Room::Room(int32 maxUserCount) : _maxUserCount(maxUserCount){

}

void Room::Enter(shared_ptr<Session*> session){
	lock_guard<mutex> lock(_mutex);


}

void Room::Exit(shared_ptr<Session*> session){

}
