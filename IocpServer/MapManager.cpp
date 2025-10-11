#include "pch.h"
#include "MapManager.h"

void MapManager::InitializeMaps(){

	maps.insert({ 1, mapLoader.LoadMap("NavMeshGrid.bin") });
}
