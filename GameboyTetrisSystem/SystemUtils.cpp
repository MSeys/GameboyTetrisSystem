#include "SystemUtils.h"

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

TetrisBlocksContainer SystemUtils::Transpose(const TetrisBlocksContainer& container)
{
	TetrisBlocksContainer transposedBlocks;
	transposedBlocks.resize(container[0].size(), std::vector<bool>(container.size()));

	for (int i = 0; i < container.size(); ++i) {
		for (int j = 0; j < container[0].size(); ++j)
		{
			transposedBlocks[j][i] = container[i][j];
		}
	}

	return transposedBlocks;
}
