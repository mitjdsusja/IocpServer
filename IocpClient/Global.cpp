#include "pch.h"
#include "Global.h"

#include "Player.h"
#include "GameManager.h"

shared_ptr<PlayerManager> GPlayerManager = nullptr;
shared_ptr<GameManager> GGameManager = nullptr;

chrono::steady_clock::time_point _GClientStartTimePoint;

class Global {
public:
	Global() {
		GPlayerManager = make_shared<PlayerManager>();
		GGameManager = make_shared<GameManager>();

		_GClientStartTimePoint = chrono::steady_clock::now();
	}
	
	~Global() {

	}

}Global;