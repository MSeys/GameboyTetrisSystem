#pragma once
#include <string>
#include <vector>
typedef std::vector<std::vector<bool>> TetrisBlocksContainer;
typedef std::vector<std::vector<uint8_t>> GameboyBuffer;

struct vec2
{
	float x;
	float y;

	vec2(float x = 0, float y = 0) : x(x), y(y) {}
	std::string ToString() const
	{
		return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
	}
};

struct ivec2
{
	int x;
	int y;

	ivec2(int x = 0, int y = 0) : x(x), y(y) {}
	std::string ToString() const
	{
		return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
	}
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

enum class TetrisMenu
{
	CREDITS, START, GAME_SELECT, LEVEL_SELECT, PLAY, GAME_OVER
};

namespace SystemUtils
{
	/**
	 * \brief Checks the buffer to see if there is a block there. Uses corners and checks if the pixel is colored black (3).
	 * \param blocks Converted blocks Data
	 * \param startPos Starting pixel
	 * \param pixelBuffer Pixel Buffer from System
	 */
	void CheckTetrisBlocks(TetrisBlocksContainer& blocks, const ivec2& startPos, const GameboyBuffer& pixelBuffer);

	/**
	 * \brief Gets the pixels from the buffer that we want based on two "coordinates"
	 * \param startPos First corner of pixel volume we want to receive
	 * \param endPos Opposite corner of pixel volume we want to receive
	 * \param pixelBuffer Pixel Buffer from System
	 * \return Pixels we need
	 */
	GameboyBuffer GetPixels(const ivec2& startPos, const ivec2& endPos, const GameboyBuffer& pixelBuffer);
	
	/**
	 * \brief We check the blocks container and try to recognize what piece it is by checking the data
	 * \param blocks The container we check (4 columns, 2 rows)
	 * \return Tetris Piece it recognizes
	 */
	TetrisPiece GetPiece(const TetrisBlocksContainer& blocks);
	std::string TetrisPieceToString(const TetrisPiece& piece);

	std::string TetrisMenuToString(const TetrisMenu& menu);
	
	TetrisBlocksContainer Transpose(const TetrisBlocksContainer& container);

	template<class T>
	int CountVector2D(std::vector<std::vector<T>> vector2d, T value)
	{
		int count{};

		for (int i{}; i < int(vector2d.size()); i++)
		{
			count += int(std::count(vector2d[i].cbegin(), vector2d[i].cend(), value));
		}

		return count;
	}
}