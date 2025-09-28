#include "pch.h"
#include "GridManager.h"

void GridManager::AddPlayer(uint64 sessionId, Vector<int16> position){

	int16 cellX = GetCellCoord(position._x / 100);
	int16 cellY = GetCellCoord(position._y / 100);

	_players[sessionId] = {
		sessionId,
		{position._x, position._y, position._z},
		{cellX, cellY}
	};

	_grid[{ cellX, cellY }].insert(sessionId);
	//cout << sessionId << " new Cell : " << cellX << " " << cellY << endl;
}

void GridManager::RemovePlayer(uint64 sessionId){

	if (_players.count(sessionId)) {
		auto& p = _players[sessionId];
		_grid[{p._cell._x, p._cell._y}].erase(p._sessionId);
		_players.erase(sessionId);
	}
}

GridMoveResult GridManager::MovePosition(uint64 sessionId, Vector<int16> newPosition){

	GridMoveResult result;

	if (_players.count(sessionId) == false) {

		spdlog::info("GridManager MovePosition None Player");
		//cout << "GridManager MovePosition None Player" << endl;
		return result;
	}

	//cout << "new Position : " << newPosition._x/100 << " " << newPosition._z/100 << endl;
	GridPlayer& p = _players[sessionId];
	Vector<int16> newCell;
	newCell._x = GetCellCoord(newPosition._x / 100);
	newCell._y = GetCellCoord(newPosition._z / 100);

	if (newCell._x != p._cell._x || newCell._y != p._cell._y) {
		//cout << sessionId << " new Cell : " << newCell._x << " " << newCell._y << endl;
		_grid[{p._cell._x, p._cell._y}].erase(sessionId);
		_grid[{newCell._x, newCell._y}].insert(sessionId);

		result._cellChanged = true;
		result._oldCell = p._cell;
		result._newCell = newCell;

		p._cell._x = newCell._x;
		p._cell._y = newCell._y;
	}
	p._position = newPosition;

	return result;
}

vector<uint64> GridManager::GetNearByPlayers(uint64 sessionId) {

	vector<uint64> nearPlayers;
	if (_players.count(sessionId) == false) {

		spdlog::info("GridManager GetNearByPlayers invalid sessionId");
		//cout << "GridManager GetNearByPlayers invalid sessionId" << endl;
		return nearPlayers;
	}

	const GridPlayer& gridPlayer = _players[sessionId];

	for (int16 gridX = -1; gridX <= 1; ++gridX) {
		for (int16 gridY = -1; gridY <= 1; ++gridY) {

			int16 cellX = gridPlayer._cell._x + gridX;
			int16 cellY = gridPlayer._cell._y + gridY;
			
			auto it = _grid.find({ cellX, cellY });
			if (it == _grid.end()) {
				continue;
			}

			for (int64 otherPlayerSessionId : it->second) {
				if (otherPlayerSessionId != sessionId) {
					nearPlayers.push_back(otherPlayerSessionId);
				}
			}
		}
	}

	//cout << sessionId << " Near Players : " << nearPlayers.size() << endl;
	//for (uint64 nearPlayerSessionId : nearPlayers) {
	//	cout << nearPlayerSessionId << ", ";
	//}
	//cout << endl;

	return nearPlayers;
}

vector<uint64> GridManager::GetPlayersInCell(Vector<int16> cell){

	vector<uint64> playersInCell;

	const auto& it = _grid.find({ cell._x, cell._y });
	if(it == _grid.end()) {

		return playersInCell;
	}
	
	playersInCell.assign(it->second.begin(), it->second.end());

	return playersInCell;
}


