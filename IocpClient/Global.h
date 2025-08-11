#pragma once

class PlayerManager;
class GameManager;

extern shared_ptr<PlayerManager> GPlayerManager;
extern shared_ptr<GameManager> GGameManager;

extern chrono::steady_clock::time_point _GClientStartTimePoint;
