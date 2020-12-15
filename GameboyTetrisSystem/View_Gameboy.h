#pragma once
#include "EView.h"
#include "Renderer.h"

class View_Gameboy : public EView
{
	SDL_Texture* m_pGBScreenTexture;

	void CraftPixelBuffer(uint16_t* buffer) const
	{
		std::bitset<(160 * 144) * 2> bitBuffer{ m_pSystem->GetEmulator().GetFrameBuffer(0) };

	#pragma loop(hint_parallel(20))
		for (int i{ 0 }; i < (160 * 144); ++i) {
			const uint8_t val{ uint8_t((bitBuffer[i * 2] << 1) | uint8_t(bitBuffer[i * 2 + 1])) };
			buffer[i] = (0xFFFF - (val * 0x5555));
		}
	}
	
public:
	View_Gameboy() : EView("Gameboy View", ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar)
	{
		m_pGBScreenTexture = SDL_CreateTexture(Renderer::GetInstance().GetSDLRenderer(), 
								SDL_PIXELFORMAT_RGBA4444, 
								SDL_TEXTUREACCESS_STREAMING, 
									160, 144);
	}

	~View_Gameboy()
	{
		SDL_DestroyTexture(m_pGBScreenTexture);
	}

	void Update() override
	{
		
	}
	
	void DrawGUI() override
	{
		uint16_t pixelBuffer[160 * 144]{};
		CraftPixelBuffer(pixelBuffer);
	
		SDL_UpdateTexture(m_pGBScreenTexture, nullptr, static_cast<void*>(pixelBuffer), 160 * sizeof(uint16_t));

		ImGui::SetNextWindowSize({ 175, 200 });
		//ImGui::SetNextWindowPos({ 25, 25 });
		ImGui::Begin("View - Gameboy", nullptr, m_Flags);
		ImGui::Image(m_pGBScreenTexture, { 160, 144 });

		if (ImGui::Button("(Un)pause", { -1, 0 }))
			m_pSystem->GetEmulator().SetPauseState(!m_pSystem->GetEmulator().GetPauseState(0), 0);
		ImGui::End();
	}
};
