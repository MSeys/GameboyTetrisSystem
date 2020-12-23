#include "SystemUtils.h"

#include "pch.h"
#include "Renderer.h"

void SystemUtils::SetBlocks(TetrisBlocksContainer& blocks, const ivec2& startPos, const std::vector<std::vector<uint8_t>>& pixelBuffer)
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

			if (blocks[col][row])
			{
				Renderer::GetInstance().RenderFilledRect(
					{ float(col * BLOCK_SIZE), float(row * BLOCK_SIZE), float(BLOCK_SIZE), float(BLOCK_SIZE) },
					{ 255, 255, 255, 255 });
			}
		}
	}
}

TetrisPiece SystemUtils::GetPiece(const TetrisBlocksContainer& blocks)
{
	TetrisBlocksContainer transposedBlocks;
	transposedBlocks.resize(blocks[0].size(), std::vector<bool>(blocks.size()));
	
	for (int i = 0; i < blocks.size(); ++i) {
		for (int j = 0; j < blocks[0].size(); ++j)
		{
			transposedBlocks[j][i] = blocks[i][j];
		}
	}
	
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
