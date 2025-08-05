#include "pch.h"
#include "Global.h"

#include "Player.h"
#include "GameManager.h"

shared_ptr<PlayerManager> GPlayerManager = nullptr;
shared_ptr<GameManager> GGameManager = nullptr;


class Global {
public:
	Global() {
		GPlayerManager = make_shared<PlayerManager>();
		GGameManager = make_shared<GameManager>();
	}
	
	~Global() {

	}

}Global;