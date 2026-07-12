#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "GameplayScene.h"
#include "SceneManager.h"
#include <TaskScheduler.h>
#include <thread>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <Helpers.h>

using namespace JLib;

GameplayScene::GameplayScene(
                             JLib::Time& time, JLib::Renderer2D& renderer, std::shared_ptr<JLib::InputManager> input, JLib::Camera2D& camera,
                             JLib::Mesh* quadMesh, JLib::TextureHandle tileTexture, uint32_t width, uint32_t height)
	: time(&time)
	, renderer(renderer)
	, input(input)
	, camera(camera)
	, quadMesh(quadMesh)
	, tileTexture(tileTexture)
	, width(width)
	, height(height)
{
	// WorldToScreen centers world (0,0) on the screen's middle -- with the camera parked there,
	// {0,0} maps to screen center. The board itself is defined below in board-local coordinates
	// centered on its OWN middle, so this camera position puts the board's center at the screen's
	// center, i.e. the whole board exactly fills the window.
	camera.position = {90.0f, 0.0f };
	camera.zoom = 1.0f;
	grid = Grid();
	blocks = GetAllBlocks();
	currentBlock = GetRandomBlock();
	nextBlock = GetRandomBlock();
	gridOutlineEffectID = renderer.RegisterEffect(L"shaders\\VertexShader.cso", L"shaders\\GridOutlinePS.cso",
		JLib::Renderer2D::DefaultAlphaBlend());
	if (!sound.Initialize(1)) {
		throw("failed to initialize sound on thread 1");
	}

	music = sound.PlayLoop(JLib::ExeRelativeA("sound\\tomasz-kucza-chiptune-tchaikovsky.mp3").c_str());
	if (!music.IsValid()) {
		throw("PlayLoop(\"music.mp3\") failed to load -- put a real file next to the exe to test.\n");
	}
}

void GameplayScene::HandleInput(float dt)
{
	uint8_t keyPressed = 0;
	input->GetAnyKeyPressed(keyPressed);
	if (isGameOver && keyPressed != 0) {
		isGameOver = false;
		Reset();
	}
	if (input->IsKeyDown(VK_DOWN)) {
		MoveBlockDown();
		UpdateScore(0, 1);
	}
	switch (keyPressed) {
	case VK_ESCAPE:
		SceneManager::PopScene();
		break;
	case VK_LEFT:
		MoveBlockLeft();
		break;
	case VK_RIGHT:
		MoveBlockRight();
		break;
	case VK_UP:
		RotateBlock();
		break;
	}

}

GameplayScene::~GameplayScene()
{
	sound.Shutdown();
}

bool GameplayScene::EventTriggered(double interval)
{
	double currentTime = time->GetTime();

	if (currentTime - lastUpdateTime >= interval)
	{
		lastUpdateTime = currentTime;
		return true;
	}
	return false;
}

void GameplayScene::LockBlock()
{
	std::vector<Position> tiles = currentBlock.GetCellPositions();
	for (Position item : tiles) {
		grid.grid[item.row][item.column] = currentBlock.id;
	}
	currentBlock = nextBlock;
	if (!BlockFits())
	{
		isGameOver = true;
	}
	nextBlock = GetRandomBlock();
	int rowsCleared = grid.ClearFullRows();
	UpdateScore(rowsCleared, 0);
}

void GameplayScene::RotateBlock()
{
	if (!isGameOver) {
		currentBlock.Rotate();
		if (IsBlockOutside() || !BlockFits())
		{
			currentBlock.UndoRotation();
		}
	}
}

bool GameplayScene::IsBlockOutside()
{
	std::vector<Position> tiles = currentBlock.GetCellPositions();
	for (Position item : tiles) {
		if (grid.IsCellOutside(item.row, item.column))
		{
			return true;
		}
	}
	return false;
}

void GameplayScene::MoveBlockLeft()
{
	if (!isGameOver) {
		currentBlock.Move(0, -1);
		if (IsBlockOutside() || !BlockFits())
		{
			MoveBlockRight();
		}
	}
}

void GameplayScene::MoveBlockRight()
{
	if (!isGameOver) {
		currentBlock.Move(0, 1);
		if (IsBlockOutside() || !BlockFits())
		{
			MoveBlockLeft();
		}
	}
}

void GameplayScene::MoveBlockDown()
{
	if (!isGameOver) {
		currentBlock.Move(1, 0);
		if (IsBlockOutside() || !BlockFits()) {
			currentBlock.Move(-1, 0);
			LockBlock();
		}
	}
}

bool GameplayScene::BlockFits()
{
	std::vector<Position> tiles = currentBlock.GetCellPositions();
	for (Position item : tiles) {
		if (grid.IsCellEmpty(item.row, item.column) == false)
		{
			return false;
		}
	}
	return true;
}

void GameplayScene::Reset()
{
	grid.Initialize();
	blocks = GetAllBlocks();
	currentBlock = GetRandomBlock();
	nextBlock = GetRandomBlock();
	score = 0;
	music = sound.PlayLoop(JLib::ExeRelativeA("sound\\tomasz-kucza-chiptune-tchaikovsky.mp3").c_str());
	if (!music.IsValid()) {
		throw("PlayLoop(\"music.mp3\") failed to load -- put a real file next to the exe to test.\n");
	}
}

void GameplayScene::UpdateScore(int linesCleared, int moveDownPoints)
{
	switch (linesCleared) {
	case 1: score += 100; break;
	case 2: score += 300; break;
	case 3: score += 500; break;
	default: break;
	}
	score += moveDownPoints;
}

void GameplayScene::Update(bool& isRunning, float dt)
{
	if (EventTriggered(0.2))
	{
		MoveBlockDown();
	}
	if (isGameOver)
		sound.Stop(music);
}

DirectX::XMFLOAT2 GameplayScene::CellToScreen(int row, int col, DirectX::XMFLOAT2 pixelOffset) const
{
	// Board-local world space, centered on the BOARD'S OWN middle (not top-left) -- matches what
	// Camera2D::WorldToScreen expects (world (0,0) -> screen center). With camera.position at
	// {0,0} (set in the constructor), this puts the board's center at the screen's center, so any
	// leftover slack between the board's design size and the actual window size gets split evenly
	// on both edges instead of all dumped on the right/bottom like the old top-left-anchored math.
	float boardHalfWidth = (numCols * cellSize) * 0.5f;
	float boardHalfHeight = (numRows * cellSize) * 0.5f;
	// quadVertices span -0.5..+0.5, so position is the CELL'S CENTER, not its top-left corner --
	// same convention Game01's GameplayScene::Draw uses.
	DirectX::XMFLOAT2 worldPos = {
		col * (float)cellSize + cellSize * 0.5f - boardHalfWidth,
		row * (float)cellSize + cellSize * 0.5f - boardHalfHeight
	};
	DirectX::XMFLOAT2 screenPos = camera.WorldToScreen(worldPos, { (float)width, (float)height });
	screenPos.x += pixelOffset.x;
	screenPos.y += pixelOffset.y;
	return screenPos;
}

void GameplayScene::Draw()
{
	BatchItem item;
	item.mesh = quadMesh;
	item.tex = tileTexture;
	item.size = { (float)cellSize, (float)cellSize };
	renderer.SubmitText(*renderer.GetSysFont(), 365.0f, 15.0f, "Score: ", 1.0f, JLib::Colors::White);
	renderer.SubmitText(*renderer.GetSysFont(), 410.0f, 50.0f, "{}", 1.25f, JLib::Colors::Green,0.0f,JLib::TextAlign::Center,2,score);

	BatchItem scoreBG;
	scoreBG.mesh = quadMesh;
	scoreBG.tex = tileTexture;
	scoreBG.size = { 150.0f, 100.0f };
	scoreBG.position = { 410.0f, 60.0f };
	scoreBG.color = JLib::Colors::Black;
	renderer.Submit(scoreBG);
	renderer.SubmitText(*renderer.GetSysFont(), 370.0f, 175.0f, "Next: ", 1.0f, JLib::Colors::White);
	DrawBlock(nextBlock, 350, 210);
	BatchItem nextBG;
	nextBG.mesh = quadMesh;
	nextBG.tex = tileTexture;
	nextBG.size = { 150.0f, 150.0f };
	nextBG.position = { 410.0f, 240.0f };
	nextBG.color = JLib::Colors::Black;
	renderer.Submit(nextBG);
	if (isGameOver) {
		renderer.SubmitText(*renderer.GetSysFont(), 310.0f, 450.0f, "Game Over!", 1.25f, JLib::Colors::Red);
		renderer.SubmitText(*renderer.GetSysFont(), 315.0f, 500.0f, "Press any key\n   to restart", 1.0f, JLib::Colors::White);
	}
    for (int row = 0; row < numRows; ++row) {
        for (int col = 0; col < numCols; ++col) {
            // No pointer math needed; compiler handles the [row][col] offset
            int cellValue = grid.grid[row][col]; 
            
            item.color = grid.colors[cellValue];
            item.position = CellToScreen(row, col);
            renderer.Submit(item);

            BatchItem outline = item;
            outline.effectID = gridOutlineEffectID;
            outline.zLayer = 1;
            renderer.Submit(outline);
        }
    }
    DrawBlock(currentBlock);
}
Block GameplayScene::GetRandomBlock()
{
	if (blocks.empty()) {
		blocks = GetAllBlocks();
	}
	int randomIndex = rand() % blocks.size();
	Block block = blocks[randomIndex];
	blocks.erase(blocks.begin() + randomIndex);
	return block;
}
std::vector<Block> GameplayScene::GetAllBlocks() {
	return  { IBlock(), JBlock(), LBlock(), OBlock(), SBlock(), TBlock(), ZBlock() };
}
void GameplayScene::DrawBlock(Block& block)
{
	// No offset -- Position::row/column are ABSOLUTE board coordinates already, so this draws
	// each occupied cell at exactly the same screen position CellToScreen() would give the
	// background grid's own cell at that (row, column). That's what puts the block "in its own
	// position on the grid": nothing extra to compute, just skip the offset parameter entirely
	// (CellToScreen defaults it to {0,0}).
	std::vector<Position> tiles = block.GetCellPositions();
	BatchItem bItem;
	bItem.mesh = quadMesh;
	bItem.tex = tileTexture;
	bItem.size = { (float)block.cellSize, (float)block.cellSize };
	bItem.color = block.colors[block.id];
	bItem.zLayer = 1; // draw over the background grid/its outline

	for (const Position& p : tiles) {
		bItem.position = CellToScreen(p.row, p.column);
		renderer.Submit(bItem); // fill -- default effectID (0), NOT the outline-only shader

		// Grid line on top, same pattern as Draw(): outline shader's interior is fully
		// transparent, so this only adds the black border without hiding bItem's fill color.
		BatchItem outline = bItem;
		outline.effectID = gridOutlineEffectID;
		outline.zLayer = 2; // above the block's own fill (zLayer 1)
		renderer.Submit(outline);
	}
}
void GameplayScene::DrawBlock(Block& block, int x, int y)
{
	// x, y: extra SCREEN-pixel offset (raylib-tutorial style, e.g. a small inset so the falling
	// piece doesn't sit flush against the grid lines) -- Position::row/column are ABSOLUTE board
	// coordinates already (Block::Move updates them directly, same as the raylib source), so this
	// draws through the exact same CellToScreen() the background grid uses, just with that offset
	// added on top. No separate "which rows/cols" bookkeeping needed here -- CellToScreen already
	// owns the board's coordinate system, this is just calling into it per occupied cell.
	std::vector<Position> tiles = block.cells.at(block.rotationState);
	BatchItem bItem;
	bItem.mesh = quadMesh;
	bItem.tex = tileTexture;
	bItem.size = { (float)block.cellSize, (float)block.cellSize };
	bItem.color = block.colors[block.id];
	bItem.zLayer = 1; // draw over the background grid/its outline

	for (const Position& p : tiles) {
		bItem.position = CellToScreen(p.row, p.column, { (float)x, (float)y });
		renderer.Submit(bItem); // fill -- default effectID (0), NOT the outline-only shader

		BatchItem outline = bItem;
		outline.effectID = gridOutlineEffectID;
		outline.zLayer = 2;
		renderer.Submit(outline);
	}
}
