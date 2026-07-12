#pragma once
#include <vector>
#include <memory>
#include <DirectXMath.h>
#include <Renderer2D.h>
#include <ResourceManager.h>
#include <Camera2D.h>
#include <InputManager.h>
#include <GetTime.h>
#include <SoundManager.h>
#include "Scene.h"
#include "Block.h"
#include "Grid.h"
#include "Blocks.h"
// Ported from the source project's Level1 -- owns its own PhysicsWorld/SpatialGrid built from
// an inline map, and drives HandleInput/Update/Draw through SceneManager instead of being
// hardcoded into main()'s loop.
class GameplayScene : public Scene
{
public:
	GameplayScene( JLib::Time& time, JLib::Renderer2D& renderer, std::shared_ptr<JLib::InputManager> input, JLib::Camera2D& camera,
	              JLib::Mesh* quadMesh, JLib::TextureHandle tileTexture, uint32_t width, uint32_t height);

	void Update(bool& isRunning, float dt = 0.0f) override;
	void Draw() override;
	Block GetRandomBlock();
	std::vector<Block> GetAllBlocks();
	void DrawBlock(Block& block);
	void DrawBlock(Block& block, int x, int y);
	void HandleInput(float dt) override;
	~GameplayScene() override;

private:
	bool EventTriggered(double interval);
	void LockBlock();
	void RotateBlock();
	bool IsBlockOutside();
	void MoveBlockLeft();
	void MoveBlockRight();
	void MoveBlockDown();
	bool BlockFits();
	void Reset();
	void UpdateScore(int linesCleared, int moveDownPoints);
	enum class CMD { QUIT };
	JLib::Time* time;
	Grid grid;
	Block currentBlock;
	Block nextBlock;
	int score = 0;
	double lastUpdateTime = 0.0;
	bool isGameOver = false;
	JLib::SoundManager sound;
	JLib::SoundHandle music;
	std::vector<Block> blocks;
	std::vector<CMD> cmdQ;
	JLib::Renderer2D& renderer;
	std::shared_ptr<JLib::InputManager> input;
	JLib::Camera2D& camera;
	JLib::Mesh* quadMesh;
	JLib::TextureHandle tileTexture; // FlushBatchTask/ResourceManager::Resolve requires a valid
	                                 // handle -- a default-constructed "no texture" one throws
	                                 // instead of silently skipping, so every tile still needs
	                                 // a real texture; color still does the per-tileID tinting.
	uint32_t width, height;
	// Same VertexShader.hlsl as the default effect, but GridOutlinePS.hlsl draws only a thin
	// black border (transparent interior) -- registered once in the constructor, then every
	// cell submits a second BatchItem with this effectID layered on top of its fill to show
	// grid lines without a separate hand-drawn line mesh.
	uint32_t gridOutlineEffectID = 0;

	DirectX::XMFLOAT4 ColorForTileID(uint32_t tileID) const;
	// Shared by Draw() (background grid) and DrawBlock() (the falling piece) -- both need to turn
	// an ABSOLUTE (row, col) on the 20x10 board into a final screen position, and that math
	// (board-centered world space -> camera.WorldToScreen) shouldn't be duplicated in two places.
	// pixelOffset is an optional extra nudge in SCREEN pixels, applied after the camera transform
	// (e.g. DrawBlock's raylib-tutorial-style offsetX/offsetY inset) -- {0,0} for the plain grid.
	DirectX::XMFLOAT2 CellToScreen(int row, int col, DirectX::XMFLOAT2 pixelOffset = { 0.0f, 0.0f }) const;
};
