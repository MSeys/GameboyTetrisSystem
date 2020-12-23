#pragma once
#include <string>
#include <vector>
typedef std::vector<std::vector<bool>> TetrisBlocksContainer;

struct vec2
{
	float x;
	float y;

	vec2(float x = 0, float y = 0) : x(x), y(y) {}
};

struct ivec2
{
	int x;
	int y;

	ivec2(int x = 0, int y = 0) : x(x), y(y) {}
};

struct rectf
{
	float x;
	float y;
	float w;
	float h;

	rectf(float x = 0, float y = 0, float w = 0, float h = 0) : x(x), y(y), w(w), h(h) {}
};

struct TetrisPieceRotation
{
	int nrMoveLeft;
	int nrMoveRight;
	TetrisBlocksContainer pieceData;
};

struct TetrisPieceData
{
	std::vector<TetrisPieceRotation> rotations;
};

template<class T>
void SafeDelete(T& pObjectToDelete)
{
	if (pObjectToDelete != nullptr)
	{
		delete(pObjectToDelete);
		pObjectToDelete = nullptr;
	}
}
template<class T>
T Clamp(T min, T max, T value)
{
	if (value < min) return min;
	if (value > max) return max;

	return value;
}

enum class TetrisPiece
{
	O_PIECE, I_PIECE, S_PIECE, Z_PIECE, L_PIECE, J_PIECE, T_PIECE, NO_PIECE
};

namespace SystemUtils
{
	void SetBlocks(TetrisBlocksContainer& blocks, const ivec2& startPos, const std::vector<std::vector<uint8_t>>& pixelBuffer);
	TetrisPiece GetPiece(const TetrisBlocksContainer& blocks);
	std::string TetrisPieceToString(const TetrisPiece& piece);
}