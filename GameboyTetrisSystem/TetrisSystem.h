#pragma once
#include <unordered_map>
#include <vector>

#include "pch.h"

class SystemView_Playfield;
class SystemView_NextPiece;
class SystemView_CurrentPiece;

class TetrisSystem
{
public:
	TetrisSystem(gbee::Emulator& emulator, const vec2& windowSize = { 800, 520 });

	void Initialize();
	void Run();
	void Quit();
	
	const GameboyBuffer& GetPixelBuffer() const { return m_PixelBuffer; }
	const gbee::Emulator& GetEmulator() const { return m_Emulator; }
	const uint16_t* GetColors() const { return m_Colors; }
	const SDL_Color* GetSDLColors() const { return m_SDLColors; }
	const std::unordered_map<TetrisPiece, TetrisPieceData>& GetPiecesData() const { return m_PiecesData; }
	
private:
	// BACK-END
	SDL_Window* m_pWindow{};
	vec2 m_WindowSize;
	bool m_Exit = false;
	const float m_FPS{ 59.73f };

	
	// GAMEBOY
	const uint16_t m_Colors[4]{ 0xFFFF, 0x5555 * 2, 0x5555, 0x0000 };
	const SDL_Color m_SDLColors[4]{ { 255, 255, 255, 255 }, { 170, 170, 170, 255 }, { 85, 85, 85, 255 }, { 0, 0, 0, 255 } };

	gbee::Emulator& m_Emulator;
	GameboyBuffer m_PixelBuffer;

	
	// SYSTEM
	std::unordered_map<TetrisPiece, TetrisPieceData> m_PiecesData;
	SystemView_Playfield* m_pView_Playfield{};
	SystemView_NextPiece* m_pView_NextPiece{};
	SystemView_CurrentPiece* m_pView_CurrentPiece{};

	TetrisMenu m_CurrentMenu;
	GameboyBuffer m_ScoreZoneBuffer;
	const ivec2 m_ScoreStart{ 135, 22 }, m_ScoreEnd{ 159, 31 };
	const ivec2 m_GameMenuStart{ 8, 7 }, m_GameMenuEnd{ 71, 14 };
	const ivec2 m_GameOverStart{ 120, 111 }, m_GameOverEnd{ 151, 126 };
	
	bool m_CanUpdatePlayData{ true }, m_CalculateMove{ false };
	int m_Frames{};
	BestTetrisMove m_CurrentMove{ false };
	
	void Initialize_Backend();
	void Initialize_ImGuiStyle();
	void Initialize_System();
	
	void Run_SetKeyState(const SDL_Event& event) const;
	void Run_Internal();
	
	void UpdatePixelBuffer();
	void UpdateSystem();
	void DrawSystemData() const;

	TetrisMenu CheckMenu() const;
	bool CheckScore();

	// Press / Release Key with a frame delay. Uses Even Frames to set state to true.
	bool UseKey(const gbee::Key& key, int framesDelay) const;
	void SetKey(const gbee::Key& key, bool value) const;
	void ResetKeys() const;
	
	void Initialize_PieceO();
	void Initialize_PieceI();
	void Initialize_PieceS();
	void Initialize_PieceZ();
	void Initialize_PieceL();
	void Initialize_PieceJ();
	void Initialize_PieceT();
};
