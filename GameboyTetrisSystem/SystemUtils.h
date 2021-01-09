#pragma once
#include <deque>
#include <iostream>
#include <string>
#include <vector>
#include "../GameboyEmulator/EmulatorClean.h"

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
	TetrisBlocksContainer blocks;
};

enum class TetrisMoveSet{ LEFT, RIGHT, ROTATE };
struct TetrisMove
{
	bool valid{};
	std::deque<TetrisMoveSet> moveSet;
	TetrisBlocksContainer newPlayfield;
	TetrisBlocksContainer blockOnly;

	double hScore{};
	int hAggregateHeight{};
	int hCompleteLines{};
	int hHoles{};
	int hBumpiness{};

	bool operator<(const TetrisMove& other) const;
};

struct BestTetrisMove
{
	bool valid{};

	double hScore{};
	int hAggregateHeight{};
	int hCompleteLines{};
	int hHoles{};
	int hBumpiness{};
	
	std::deque<TetrisMoveSet> moveSet;
	std::vector<TetrisMove> movesDepth;
};

typedef std::vector<TetrisPieceRotation> TetrisPieceData;

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

enum class TetrisPiece : int
{
	O_PIECE, I_PIECE, S_PIECE, Z_PIECE, L_PIECE, J_PIECE, T_PIECE, NO_PIECE
};

enum class TetrisMenu
{
	CREDITS, START, GAME_SELECT, LEVEL_SELECT, PLAY, GAME_OVER
};

TetrisBlocksContainer operator+(const TetrisBlocksContainer& lhs, const TetrisBlocksContainer& rhs);


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
	gbee::Key TetrisMoveSetToKey(const TetrisMoveSet& moveSet);

	
	/**
	 * \brief Based on a given piece rotation and position, checks whether it is a valid move on the playfield
	 * \param playfield The playfield container
	 * \param tetrisBlock The block rotation we are checking on the playfield
	 * \param pos the first position of where the piece would be located
	 * \return boolean whether it is valid move
	 */
	bool IsValidMove(const TetrisBlocksContainer& playfield, const TetrisPieceRotation& tetrisBlock, const ivec2& pos);
	void AddTetrisBlock(TetrisBlocksContainer& playfield, const TetrisPieceRotation& tetrisBlock, const ivec2& pos);

	TetrisMove GetTetrisMove(const TetrisBlocksContainer& playfield, const TetrisPieceRotation& tetrisBlock, int movement, int rotation);
	std::vector<TetrisMove> GetAllTetrisMoves(const TetrisBlocksContainer& playfield, const TetrisPieceData& pieceData);
	BestTetrisMove GetBestTetrisMove(const TetrisBlocksContainer& playfield, const std::vector<TetrisPieceData>& pieces);
	
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