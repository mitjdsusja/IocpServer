#pragma once
#include <pch.h>

class GridManager{
public:
	pair<int, int> GetCellCoords(int16 x, int16 y) {
		return { x / CELL_SIZE, y / CELL_SIZE };
	}

	

private:


public:
	const int CELL_SIZE = 25;

private:


};

