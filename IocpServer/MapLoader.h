#pragma once
#include <fstream>

enum MapTile {

	WALKABLE = 0,
	BLOCKED = 1,
};;

struct MapData {

	int32 mapId;
	int64 width;
	int64 height;
	vector<uint8> grid;

	bool IsWalkable(uint64 x, uint64 y) const {

		if (x < 0 || x >= width || y < 0 || y >= height) {
			return false;
		}
		return grid[y * width + x] == WALKABLE; // 0 = walkable, 1 = blocked
	}
};

class MapLoader{
public:
	static MapData LoadMap(const string& path);
};

