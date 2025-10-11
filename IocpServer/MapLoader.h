#pragma once
#include <fstream>
#include "Vector.h"

enum MapTile {

	BLOCKED = 0,
	WALKABLE = 1,
};;

struct MapData {
	int32 mapId;
	int32 minX;
	int32 minZ;
	int32 width;
	int32 height;
	int32 cellSize;
	vector<uint8> grid;

	bool IsWalkable(int32 worldX, int32 worldZ) const {

		Vector<int32> gridPoint = WorldToGrid(worldX, worldZ);

		if (gridPoint._x < 0 || gridPoint._x >= width || gridPoint._y < 0 || gridPoint._y >= height) {

			return false;
		}

		int64 index = gridPoint._y * width + gridPoint._x;
		return grid[index] == WALKABLE;
	};

	Vector<int32> WorldToGrid(int32 worldX, int32 worldZ) const {

		int32 gridX = ((worldX - minX) / cellSize);
		int32 gridY = ((worldZ - minZ) / cellSize);
		return { gridX, gridY };
	}
};

class MapLoader{
public:
	static MapData LoadMap(const string& path);
};

