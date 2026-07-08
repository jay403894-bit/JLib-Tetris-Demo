#pragma once
#include <vector>
#include <map>
#include <DirectXMath.h>
#include "Position.h"
#include "TetrisColors.h"
struct Block
{
	Block();
	void Move(int rows, int columns);
	void Rotate();
	void UndoRotation();
	std::vector<Position> GetCellPositions();
	int id;
	std::map<int, std::vector<Position>> cells;
	int cellSize;
	int rotationState;
	std::vector<DirectX::XMFLOAT4> colors;
	int rowOffset = 0; // Offset for the block's position on the grid
	int columnOffset = 0; // Offset for the block's position on the grid
};


