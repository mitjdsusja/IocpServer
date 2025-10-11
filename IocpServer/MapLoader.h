#pragma once
#include <fstream>

struct MapData {

	int32 mapId;
	int64 width;
	int64 height;
	vector<uint8> grid;
};

class MapLoader{
public:
	static MapData LoadMap(const string& path);
};

