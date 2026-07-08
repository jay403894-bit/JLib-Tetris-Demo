#include "Block.h"

Block::Block()
	: cellSize(30), rotationState(0)
{
	colors = GetCellColors();
}

void Block::Move(int rows, int columns)
{
	rowOffset += rows;
	columnOffset += columns;
}

void Block::Rotate()
{
	rotationState++;
	if (rotationState >= cells.size())
	{
		rotationState = 0;
	}
}

void Block::UndoRotation()
{
	rotationState--;
	if (rotationState < 0)
	{
		rotationState = cells.size() - 1;
	}
}

std::vector<Position> Block::GetCellPositions() {
	std::vector<Position> tiles = cells[rotationState];
	std::vector<Position> movedTiles;

	for (Position item : tiles) {
		Position newPos = Position(item.row + rowOffset, item.column + columnOffset);
		movedTiles.push_back(newPos);
	}
	return movedTiles;	
}