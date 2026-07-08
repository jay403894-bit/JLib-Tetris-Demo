#include <Colors.h>
#include "TetrisColors.h"

const DirectX::XMFLOAT4 darkGrey = JLib::Colors::DarkGray;
const DirectX::XMFLOAT4 green = JLib::Colors::Green;
const DirectX::XMFLOAT4 red = JLib::Colors::Red;
const DirectX::XMFLOAT4 orange = JLib::Colors::Orange;
const DirectX::XMFLOAT4 yellow = JLib::Colors::Yellow;
const DirectX::XMFLOAT4 purple = JLib::Colors::Purple;
const DirectX::XMFLOAT4 cyan = JLib::Colors::Cyan;
const DirectX::XMFLOAT4 blue = JLib::Colors::Blue;

std::vector<DirectX::XMFLOAT4> GetCellColors()
{
    DirectX::XMFLOAT4 darkGrey = JLib::Colors::DarkGray;
    DirectX::XMFLOAT4 green = JLib::Colors::Green;
    DirectX::XMFLOAT4 red = JLib::Colors::Red;
    DirectX::XMFLOAT4 orange = JLib::Colors::Orange;
    DirectX::XMFLOAT4 yellow = JLib::Colors::Yellow;
    DirectX::XMFLOAT4 purple = JLib::Colors::Purple;
    DirectX::XMFLOAT4 cyan = JLib::Colors::Cyan;
    DirectX::XMFLOAT4 blue = JLib::Colors::Blue;

    return { darkGrey, green, red, orange, yellow, purple, cyan, blue };
}
