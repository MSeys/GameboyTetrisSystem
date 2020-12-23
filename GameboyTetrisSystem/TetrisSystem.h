#pragma once
#include <vector>

#include "pch.h"
class SystemView_Playfield;
class SystemView_NextPiece;
class SystemView_CurrentPiece;

class TetrisSystem
{
public:
	TetrisSystem(gbee::Emulator& emulator, const vec2& windowSize = { 800, 600 });

	void Initialize();
	void Run();
	void Quit();
	
	const std::vector<std::vector<uint8_t>>& GetPixelBuffer() const { return m_PixelBuffer; }
	const gbee::Emulator& GetEmulator() const { return m_Emulator; }
	const uint16_t* GetColors() const { return m_Colors; }
	const SDL_Color* GetSDLColors() const { return m_SDLColors; }
	
private:
	SDL_Window* m_pWindow{};
	vec2 m_WindowSize;
	bool m_Exit = false;
	const float m_FPS{ 59.73f };
	const uint16_t m_Colors[4]{ 0xFFFF, 0x5555 * 2, 0x5555, 0x0000 };
	const SDL_Color m_SDLColors[4]{ { 255, 255, 255, 255 }, { 170, 170, 170, 255 }, { 85, 85, 85, 255 }, { 0, 0, 0, 255 } };
	
	gbee::Emulator& m_Emulator;
	std::vector<std::vector<uint8_t>> m_PixelBuffer;

	SystemView_Playfield* m_pView_Playfield;
	SystemView_NextPiece* m_pView_NextPiece;
	SystemView_CurrentPiece* m_pView_CurrentPiece;
	
	void SetKeyState(const SDL_Event& event) const;
	void UpdatePixelBuffer();

	void InitializeImGuiStyle();
};
