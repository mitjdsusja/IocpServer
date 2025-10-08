#pragma once
#include "Vector.h"

struct GridPlayer {

	uint64 _sessionId;
	Vector<int32> _position;
	Vector<int32> _cell;
};

struct GridMoveResult {

	bool _cellChanged = false;
	Vector<int32> _oldCell;
	Vector<int32> _newCell;
};

class GridManager{
public:
	GridManager(int32 cellSize) : _cellSize(cellSize) {}
	
	void AddPlayer(uint64 sessionId, const Vector<int32>& position);
	void RemovePlayer(uint64 sessionId);
	GridMoveResult MovePosition(uint64 sessionId, const Vector<int32>& newPosition);
	
	vector<uint64> GetNearByPlayers(uint64 sessionId);
	vector<uint64> GetPlayersInCell(const Vector<int32>& cell);
	vector<uint64> GetPlayersAroundCell(const Vector<int32>& cell);

private:
	int32 GetCellCoord(int32 value) {

		return (value >= 0) ? (value / _cellSize) : ((value - _cellSize + 1) / _cellSize);
	}

	struct CellHash {
		uint64 operator()(const pair<int, int>& cell) const {
			return hash<int32>()(cell.first) ^ (hash<int32>()(cell.second) << 1);
		}
	};

private:
	int32 _cellSize;
	unordered_map<uint64, GridPlayer> _players;
	unordered_map<pair<int, int>, unordered_set<uint64>, CellHash> _grid;

};

