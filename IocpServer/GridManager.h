#pragma once
#include "Vector.h"

struct GridPlayer {

	uint64 _sessionId;
	Vector<int16> _position;
	Vector<int16> _cell;
};

class GridManager{
public:
	GridManager(int32 cellSize) : _cellSize(cellSize) {}
	
	void AddPlayer(uint64 sessionId, Vector<int16> position);
	void RemovePlayer(uint64 sessionId);
	void MovePosition(uint64 sessionId, Vector<int16> newPosition);
	vector<uint64> GetNearByPlayers(uint64 sessionId);

private:
	int16 GetCellCoord(int16 value) {

		return (value >= 0) ? (value / _cellSize) : ((value - _cellSize + 1) / _cellSize);
	}

	struct CellHash {
		uint64 operator()(const pair<int, int>& cell) const {
			return hash<int32>()(cell.first) ^ (hash<int32>()(cell.second) << 1);
		}
	};

private:
	int16 _cellSize;
	unordered_map<uint64, GridPlayer> _players;
	unordered_map<pair<int, int>, unordered_set<uint64>, CellHash> _grid;

};

