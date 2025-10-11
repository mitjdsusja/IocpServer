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
	MapData LoadMap(const string& path) {

		std::ifstream file(path, std::ios::binary);
		MapData mapData;
		if (!file) {
			
		}

		file.read(reinterpret_cast<char*>(&mapData.mapId), sizeof(mapData.mapId));
	}
};

