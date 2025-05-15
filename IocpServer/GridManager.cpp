#include "pch.h"
#include "GridManager.h"

void GridManager::AddPlayer(uint64 sessionId, Vector<int16> position){

	int16 x = position._x / _cellSize;
	int16 y = position._y / _cellSize;

	_players[sessionId] = {
		sessionId,
		{position._x, position._y, position._z},
		{x, y}
	};

	_grid[{ x, y }].insert(sessionId);
}

void GridManager::RemovePlayer(uint64 sessionId){

	if (_players.count(sessionId)) {
		auto& p = _players[sessionId];
		_grid[{p._cell._x, p._cell._y}].erase(p._sessionId);
		_players.erase(sessionId);
	}
}

void GridManager::MovePosition(uint64 sessionId, Vector<int16> newPosition){

	if (_players.count(sessionId) == false) return;

	GridPlayer& p = _players[sessionId];
	Vector<int16> newCell;
	newCell._x = newPosition._x / _cellSize;
	newCell._y = newPosition._z / _cellSize;

	if (newCell._x != p._cell._x || newCell._y != p._cell._y) {
		_grid[{p._cell._x, p._cell._y}].erase(sessionId);
		_grid[{newCell._x, newCell._y}].insert(sessionId);
		p._cell._x = newCell._x;
		p._cell._y = newCell._y;
	}
	p._position = newPosition;
}

vector<uint64> GridManager::GetNearByPlayers(uint64 sessionId) {

	vector<uint64> nearPlayers;
	if (_players.count(sessionId) == false) return nearPlayers;

	const GridPlayer& gridPlayer = _players[sessionId];

	for (int16 gridX = -1; gridX <= 1; ++gridX) {
		for (int16 gridY = -1; gridY <= 1; ++gridY) {

			int16 cellX = gridPlayer._cell._x;
			int16 cellY = gridPlayer._cell._y;

			auto it = _grid.find({ cellX, cellY });
			if (it == _grid.end()) continue;

			for (int otherPlayerSessionId : it->second) {
				if (otherPlayerSessionId != sessionId) {
					nearPlayers.push_back(otherPlayerSessionId);
				}
			}
		}
	}

	return nearPlayers;
}


