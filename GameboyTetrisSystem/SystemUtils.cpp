#include "SystemUtils.h"

#include <iostream>


#include "pch.h"

void SystemUtils::CheckTetrisBlocks(TetrisBlocksContainer& blocks, const ivec2& startPos, const GameboyBuffer& pixelBuffer)
{
	const int columns{ int(blocks.size()) }, rows{ int(blocks[0].size()) };
	
	for (int col{}; col < columns; col++)
	{
		for (int row{}; row < rows; row++)
		{
			std::vector<bool> corners(CORNERS);
			const ivec2 startPixel{ startPos.x + col * BLOCK_SIZE, startPos.y + row * BLOCK_SIZE };

			if (CORNERS >= 1)
				corners[0] = pixelBuffer[startPixel.x][startPixel.y] == COLOR_BLACK;
			if (CORNERS >= 2)
				corners[1] = pixelBuffer[startPixel.x + OTHER_SIDE][startPixel.y] == COLOR_BLACK;
			if (CORNERS >= 3)
				corners[2] = pixelBuffer[startPixel.x][startPixel.y + OTHER_SIDE] == COLOR_BLACK;
			if (CORNERS == 4)
				corners[3] = pixelBuffer[startPixel.x + OTHER_SIDE][startPixel.y + OTHER_SIDE] == COLOR_BLACK;

			blocks[col][row] = std::count(corners.cbegin(), corners.cend(), true) >= REQ_CORNERS;
		}
	}
}

GameboyBuffer SystemUtils::GetPixels(const ivec2& startPos, const ivec2& endPos, const GameboyBuffer& pixelBuffer)
{
	const int xSize{ endPos.x - startPos.x + 1 }, ySize{ endPos.y - startPos.y + 1 };
	GameboyBuffer specificPixels(xSize, std::vector<uint8_t>(ySize));

	for (int x{}; x < xSize; x++)
	{
		for (int y{}; y < ySize; y++)
		{
			specificPixels[x][y] = pixelBuffer[startPos.x + x][startPos.y + y];
		}
	}

	return specificPixels;
}

TetrisPiece SystemUtils::GetPiece(const TetrisBlocksContainer& blocks)
{
	auto transposedBlocks = Transpose(blocks);
	
	const unsigned int countFirstRow = unsigned(std::count(transposedBlocks[0].cbegin(), transposedBlocks[0].cend(), true));
	const unsigned int countSecondRow = unsigned(std::count(transposedBlocks[1].cbegin(), transposedBlocks[1].cend(), true));
	
	if(countFirstRow == 4 && countSecondRow == 0)
		return TetrisPiece::I_PIECE;
	
	if (countFirstRow == 3 && countSecondRow == 1)
	{
		if(transposedBlocks[1][1])
			return TetrisPiece::T_PIECE;
		if (transposedBlocks[1][0])
			return TetrisPiece::L_PIECE;
		if (transposedBlocks[1][2])
			return TetrisPiece::J_PIECE;
	}
	
	if(countFirstRow == 2 && countSecondRow == 2)
	{
		if (!transposedBlocks[0][0] && !transposedBlocks[1][0] && transposedBlocks[1][1] && transposedBlocks[1][2])
			return TetrisPiece::O_PIECE;
		if (transposedBlocks[1][0] && transposedBlocks[1][1])
			return TetrisPiece::S_PIECE;
		if (transposedBlocks[1][1] && transposedBlocks[1][2])
			return TetrisPiece::Z_PIECE;
	}

	return TetrisPiece::NO_PIECE;
}

std::string SystemUtils::TetrisPieceToString(const TetrisPiece& piece)
{
	switch(piece)
	{
	case TetrisPiece::O_PIECE: return "O Piece";
	case TetrisPiece::I_PIECE: return "I Piece";
	case TetrisPiece::S_PIECE: return "S Piece";
	case TetrisPiece::Z_PIECE: return "Z Piece";
	case TetrisPiece::L_PIECE: return "L Piece";
	case TetrisPiece::J_PIECE: return "J Piece";
	case TetrisPiece::T_PIECE: return "T Piece";
	case TetrisPiece::NO_PIECE: return "No Piece";
	}
	return "Never happens";
}

std::string SystemUtils::TetrisMenuToString(const TetrisMenu& menu)
{
	switch(menu)
	{
	case TetrisMenu::CREDITS: return "Credits";
	case TetrisMenu::START: return "Start";
	case TetrisMenu::GAME_SELECT: return "Game Select";
	case TetrisMenu::LEVEL_SELECT: return "Level Select";
	case TetrisMenu::PLAY: return "Play";
	case TetrisMenu::GAME_OVER: return "Game Over";
	}

	return "Never happens";
}

bool SystemUtils::IsValidMove(const TetrisBlocksContainer& playfield, const TetrisPieceRotation& tetrisBlock, const ivec2& pos)
{
	if (pos.y + tetrisBlock.blocks[0].size() > playfield[0].size())
		return false;
	
	for (int x{}; x < int(tetrisBlock.blocks.size()); x++)
	{
		for (int y{}; y < int(tetrisBlock.blocks[0].size()); y++)
		{
			if (!tetrisBlock.blocks[x][y])
				continue;

			if (playfield[pos.x + x][pos.y + y])
				return false;
		}
	}

	return true;
}

void SystemUtils::AddTetrisBlock(TetrisBlocksContainer& playfield, const TetrisPieceRotation& tetrisBlock, const ivec2& pos)
{
	for (int x{}; x < int(tetrisBlock.blocks.size()); x++)
	{
		for (int y{}; y < int(tetrisBlock.blocks[0].size()); y++)
		{
			if (!tetrisBlock.blocks[x][y])
				continue;

			playfield[pos.x + x][pos.y + y] = true;
		}
	}
}

TetrisBlocksContainer operator+(const TetrisBlocksContainer& lhs, const TetrisBlocksContainer& rhs)
{
	if(lhs.size() != rhs.size() || lhs[0].size() != rhs[0].size())
	{
		std::cout << "Incompatible Tetris Block Containers, returning empty." << std::endl;
		return {};
	}

	TetrisBlocksContainer combined(lhs);
	for (int x = 0; x < int(combined.size()); ++x) {
		for (int y = 0; y < int(combined[0].size()); ++y)
		{
			combined[x][y] = lhs[x][y] || rhs[x][y];
		}
	}

	return combined;
}

TetrisMove SystemUtils::GetTetrisMove(const TetrisBlocksContainer& playfield, const TetrisPieceRotation& tetrisBlock, int movement, int rotation)
{
	TetrisMove move;

	// First column where piece is located
	const int landedColumn = movement + std::abs(tetrisBlock.nrMoveLeft);

	// Check blocks starting from top
	for(int i{}; i < TETRIS_ROWS; i++)
	{
		const bool validMove{ IsValidMove(playfield, tetrisBlock, { landedColumn, i }) };
		if(!validMove && i == 0)
		{
			move.valid = false;
			return move;
		}

		if (!validMove)
		{
			move.blockOnly.resize(playfield.size(), std::vector<bool>(playfield[0].size()));
			AddTetrisBlock(move.blockOnly, tetrisBlock, { landedColumn, i - 1 });
			move.newPlayfield = playfield + move.blockOnly;
			break;
		}
	}

	// Calculate moveset
	for (int i{}; i < rotation; i++)
		move.moveSet.push_back(TetrisMoveSet::ROTATE);

	if (movement < 0)
	{
		for (int i{}; i < std::abs(movement); i++)
			move.moveSet.push_back(TetrisMoveSet::LEFT);
	}

	else
	{
		for (int i{}; i < movement; i++)
			move.moveSet.push_back(TetrisMoveSet::RIGHT);
	}

	move.valid = true;
	return move;
}

TetrisBlocksContainer SystemUtils::Transpose(const TetrisBlocksContainer& container)
{
	TetrisBlocksContainer transposedBlocks;
	transposedBlocks.resize(container[0].size(), std::vector<bool>(container.size()));

	for (int x = 0; x < int(container.size()); ++x) {
		for (int y = 0; y < int(container[0].size()); ++y)
		{
			transposedBlocks[y][x] = container[x][y];
		}
	}

	return transposedBlocks;
}
