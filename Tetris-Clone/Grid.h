#pragma once
#include <vector>
#include <DirectXMath.h>
#include <Renderer2D.h>
#include "TetrisColors.h"
// Single source of truth for board dimensions -- previously ALSO duplicated as private members
// on Grid with the same names/values, which was fragile (the array bound below resolves to
// whichever "numRows"/"numCols" is in scope at the point of declaration; reordering members
// could silently break that). Public + constexpr means GameplayScene::Draw() can use them
// directly too, no accessor needed.
constexpr int numRows = 20;
constexpr int numCols = 10;
constexpr int cellSize = 30;

struct Grid {
	Grid();
	int grid[numRows][numCols]; // 20 rows, 10 columns
	std::vector<DirectX::XMFLOAT4> colors; // Store colors for each tile ID
	void Initialize();
	void Print();
	bool IsCellOutside(int row, int column);
	bool IsCellEmpty(int row, int column);
	bool IsRowFull(int row);
	void ClearRow(int row);
	void MoveRowDown(int row, int numRows);
	int ClearFullRows();
};
