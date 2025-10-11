#include "pch.h"
#include "MapLoader.h"

MapData MapLoader::LoadMap(const string& path) {

	std::ifstream file(path, std::ios::binary);
	MapData mapData;
	if (!file) {

		spdlog::error("MapLoader::LoadMap - Failed to open map file: {}", path);
		return mapData;
	}
	file.read(reinterpret_cast<char*>(&mapData.mapId), sizeof(mapData.mapId));
	file.read(reinterpret_cast<char*>(&mapData.width), sizeof(mapData.width));
	file.read(reinterpret_cast<char*>(&mapData.height), sizeof(mapData.height));
	
	uint64 gridSize = mapData.width * mapData.height;
	mapData.grid.resize(gridSize);
	file.read(reinterpret_cast<char*>(mapData.grid.data()), gridSize);

	return mapData;
}
