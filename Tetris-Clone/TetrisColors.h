#pragma once
#include <DirectXMath.h>
#include <Colors.h>
#include <vector>
extern const DirectX::XMFLOAT4 darkGrey;
extern const DirectX::XMFLOAT4 green;
extern const DirectX::XMFLOAT4 red;
extern const DirectX::XMFLOAT4 orange;
extern const DirectX::XMFLOAT4 yellow;
extern const DirectX::XMFLOAT4 purple;
extern const DirectX::XMFLOAT4 cyan;
extern const DirectX::XMFLOAT4 blue;

std::vector<DirectX::XMFLOAT4> GetCellColors();