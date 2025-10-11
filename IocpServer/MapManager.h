#pragma once
#include "MapLoader.h"

class MapManager{
public:
	MapManager() = default;
	void InitializeMaps();
	
private:
	MapLoader mapLoader;
	unordered_map<int32, MapData> maps;

};

