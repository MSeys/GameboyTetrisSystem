#pragma once
#include <unordered_map>
#include <vector>

#include "pch.h"
#include "TetrisSystemControl.h"

class TetrisSystem
{
public:
	TetrisSystem(gbee::Emulator& emulator, const vec2& windowSize = { 800, 600 });

	void Initialize();
	void Run();
	void Quit();
	
	const GameboyBuffer& GetPixelBuffer() const { return m_PixelBuffer; }
	const gbee::Emulator& GetEmulator() const { return m_Emulator; }
	const uint16_t* GetColors() const { return m_Colors; }
	const SDL_Color* GetSDLColors() const { return m_SDLColors; }
	const std::unordered_map<TetrisPiece, TetrisPieceData>& GetPiecesData() const { return m_PiecesData; }
	
private:
	SDL_Window* m_pWindow{};
	vec2 m_WindowSize;
	bool m_Exit = false;
	const float m_FPS{ 59.73f };
	const uint16_t m_Colors[4]{ 0xFFFF, 0x5555 * 2, 0x5555, 0x0000 };
	const SDL_Color m_SDLColors[4]{ { 255, 255, 255, 255 }, { 170, 170, 170, 255 }, { 85, 85, 85, 255 }, { 0, 0, 0, 255 } };
	
	gbee::Emulator& m_Emulator;
	GameboyBuffer m_PixelBuffer;
	std::unordered_map<TetrisPiece, TetrisPieceData> m_PiecesData;
	TetrisSystemControl m_SystemControl;
	
	void SetKeyState(const SDL_Event& event) const;
	void UpdatePixelBuffer();

	void InitializeImGuiStyle();

	/**
	 * \brief Initializes the Piece Rotations Data
	 * - Matrix built up as YX, transposed to XY
	 * - Includes how many blocks it can go left / right
	 */
	void InitializePieces();
	
	void Initialize_PieceO();
	void Initialize_PieceI();
	void Initialize_PieceS();
	void Initialize_PieceZ();
	void Initialize_PieceL();
	void Initialize_PieceJ();
	void Initialize_PieceT();
};
