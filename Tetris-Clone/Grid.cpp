#include "Grid.h"
#include <iostream>
Grid::Grid() {
    Initialize();
    colors = GetCellColors();
}

void Grid::Initialize()
{
    for (int row = 0; row < numRows; ++row) {
        for (int col = 0; col < numCols; ++col) {
            grid[row][col] = 0; // Initialize all cells to 0 (empty)
        }
    }
}

void Grid::Print()
{
    for (int row = 0; row < numRows; ++row) {
        for (int col = 0; col < numCols; ++col) {
            std::cout << grid[row][col] << " ";// Initialize all cells to 0 (empty)
        }
        std::cout << std::endl;
    }
}

bool Grid::IsCellOutside(int row, int column)
{
	if (row >= 0 && row < numRows && column >= 0 && column < numCols)
	{
		return false;
	}
	return true;
}

bool Grid::IsCellEmpty(int row, int column)
{
	if (grid[row][column] == 0)
	{
		return true;
	}
	return false;
}

bool Grid::IsRowFull(int row)
{
	for (int col = 0; col < numCols; ++col) {
		if (grid[row][col] == 0) {
			return false;
		}
	}
	return true;
}

void Grid::ClearRow(int row)
{
	for (int col = 0; col < numCols; ++col) {
		grid[row][col] = 0;
	}
}

void Grid::MoveRowDown(int row, int numRows)
{
	for (int column = 0; column < numCols; ++column) {
		grid[row + numRows][column] = grid[row][column];
		grid[row][column] = 0;
	}
}

int Grid::ClearFullRows()
{
	int completed = 0;
	for (int row = numRows - 1; row >= 0; --row) {
		if (IsRowFull(row))
		{
			ClearRow(row);
			++completed;
		}
		else if (completed > 0)
		{
			MoveRowDown(row, completed);
		}
	}
	return completed;
}
