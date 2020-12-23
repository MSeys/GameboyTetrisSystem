#pragma once
#include "EView.h"
#include "Renderer.h"

class View_PixelInspector : public EView
{
	SDL_Texture* m_pPixelBufferTexture;
	SDL_Texture* m_pPixelTexture;
	const float m_PixelTextureSize{ 150 };

	ivec2 m_PBI_XLimits{ 0, GAMEBOY_SCREEN_X };
	ivec2 m_PBI_YLimits{ 0, GAMEBOY_SCREEN_Y };

	ivec2 m_PixelPosition{ 80, 72 };
	int m_NrSidePixels{ 2 };

	void UpdatePixelBufferTexture() const
	{
		uint16_t pixelBuffer[GAMEBOY_SCREEN_X * GAMEBOY_SCREEN_Y]{};
		for (int pw{}; pw < GAMEBOY_SCREEN_X; pw++)
		{
			for (int ph{}; ph < GAMEBOY_SCREEN_Y; ph++)
			{
				if (pw >= m_PBI_XLimits.x && pw <= m_PBI_XLimits.y && ph >= m_PBI_YLimits.x && ph <= m_PBI_YLimits.y)
				{
					int i = pw + ph * GAMEBOY_SCREEN_X;
					pixelBuffer[i] = m_pSystem->GetColors()[m_pSystem->GetPixelBuffer()[pw][ph]];
				}
			}
		}
		
		SDL_UpdateTexture(m_pPixelBufferTexture, nullptr, static_cast<void*>(pixelBuffer), GAMEBOY_SCREEN_X * sizeof(uint16_t));

		
	}

	void UpdatePixelInspectTexture() const
	{
		SDL_SetRenderTarget(Renderer::GetInstance().GetSDLRenderer(), m_pPixelTexture);
		SDL_SetRenderDrawColor(Renderer::GetInstance().GetSDLRenderer(), 0, 0, 0, 255);
		SDL_RenderClear(Renderer::GetInstance().GetSDLRenderer());
		
		const int nrPixels = m_NrSidePixels * 2 + 1;
		const float pixelSize = m_PixelTextureSize / nrPixels;
		for (int pw{}; pw <= nrPixels; pw++)
		{
			for (int ph{}; ph < nrPixels; ph++)
			{
				const ivec2 pixelPos{ m_PixelPosition.x - m_NrSidePixels + pw, m_PixelPosition.y - m_NrSidePixels + ph };
				if (pixelPos.x > 0 && pixelPos.x < GAMEBOY_SCREEN_X && pixelPos.y > 0 && pixelPos.y < GAMEBOY_SCREEN_Y)
				{
					Renderer::GetInstance().RenderFilledRect(
						{ pixelSize * pw, pixelSize * ph, pixelSize + 1, pixelSize + 1 },
						m_pSystem->GetSDLColors()[m_pSystem->GetPixelBuffer()[pixelPos.x][pixelPos.y]]);
				}
			}
		}

		Renderer::GetInstance().RenderLineRect(
			{ pixelSize * m_NrSidePixels, pixelSize * m_NrSidePixels, pixelSize, pixelSize },
			{ 255, 0, 0, 255 });
		
		SDL_SetRenderTarget(Renderer::GetInstance().GetSDLRenderer(), nullptr);
	}
	
public:
	View_PixelInspector() : EView("Pixel Inspector View", ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar)
	{
		m_pPixelBufferTexture = SDL_CreateTexture(Renderer::GetInstance().GetSDLRenderer(),
			SDL_PIXELFORMAT_RGBA4444,
			SDL_TEXTUREACCESS_TARGET,
			GAMEBOY_SCREEN_X, GAMEBOY_SCREEN_Y);

		m_pPixelTexture = SDL_CreateTexture(Renderer::GetInstance().GetSDLRenderer(),
			SDL_PIXELFORMAT_RGBA4444,
			SDL_TEXTUREACCESS_TARGET,
			150, 150);
	}

	~View_PixelInspector()
	{
		SDL_DestroyTexture(m_pPixelBufferTexture);
		SDL_DestroyTexture(m_pPixelTexture);
	}

	void Update() override
	{
		UpdatePixelBufferTexture();
		UpdatePixelInspectTexture();
	}

	void DrawGUI() override
	{
		Update();
		
		ImGui::SetNextWindowSize({ 300, 210 });
		ImGui::Begin("View - Pixel Inspector", nullptr, m_Flags);

		if(ImGui::BeginTabBar("TabBar"))
		{
			if (ImGui::BeginTabItem("Pixel Buffer Inspector"))
			{
				ImGui::Image(m_pPixelBufferTexture, { GAMEBOY_SCREEN_X, GAMEBOY_SCREEN_Y });
				ImGui::SameLine();

				ImGui::PushItemWidth(120);
				ImGui::BeginGroup();

				ImGui::Text("X - Min / Max");
				ImGui::DragInt("##XMin", &m_PBI_XLimits.x);
				ImGui::DragInt("##XMax", &m_PBI_XLimits.y);

				ImGui::Text("Y - Min / Max");
				ImGui::DragInt("##YMin", &m_PBI_YLimits.x);
				ImGui::DragInt("##YMax", &m_PBI_YLimits.y);

				ImGui::EndGroup();
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Pixel Inspector"))
			{
				ImGui::Image(m_pPixelTexture, { m_PixelTextureSize, m_PixelTextureSize });
				ImGui::SameLine();

				ImGui::PushItemWidth(120);
				ImGui::BeginGroup();

				ImGui::Text("Pixel");
				ImGui::DragInt("##PixelX", &m_PixelPosition.x);
				ImGui::DragInt("##PixelY", &m_PixelPosition.y);

				m_PixelPosition.x = Clamp<int>(0, GAMEBOY_SCREEN_X - 1, m_PixelPosition.x);
				m_PixelPosition.y = Clamp<int>(0, GAMEBOY_SCREEN_Y - 1, m_PixelPosition.y);

				ImGui::Text("Nr. Side Pixels");
				ImGui::DragInt("##NrSidePixels", &m_NrSidePixels);

				const auto color = m_pSystem->GetSDLColors()[m_pSystem->GetPixelBuffer()[m_PixelPosition.x][m_PixelPosition.y]];
				float colorF[3]{ color.r / 255.f, color.g / 255.f, color.b / 255.f };
				ImGui::Text("Pixel Color");
				ImGui::ColorEdit3("##PixelColor", colorF, ImGuiColorEditFlags_NoInputs);

				ImGui::EndGroup();				
				ImGui::EndTabItem();
			}
		}

		ImGui::EndTabBar();
		
		ImGui::End();
	}
};