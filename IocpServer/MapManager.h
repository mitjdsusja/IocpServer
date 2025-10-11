#pragma once
#include "MapLoader.h"

class MapManager{
public:
	MapManager() = default;
	void InitializeMaps();

	const MapData& GetMapData(int32 mapId) {

		auto iter = maps.find(mapId);
		if (iter != maps.end()) {
			return iter->second;
		}
		else {
			throw std::runtime_error("MapManager::GetMapData - Map ID not found: " + std::to_string(mapId));
		}
	}
	
private:
	MapLoader mapLoader;
	unordered_map<int32, MapData> maps;

};

