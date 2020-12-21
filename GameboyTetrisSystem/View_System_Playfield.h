#pragma once
#include "EView.h"
#include "Renderer.h"

class View_System_Playfield : public EView
{
	const ivec2 m_StartPos{ 16, 7 };
	const int m_BlockSize{ 8 }, m_OtherSide{ m_BlockSize - 1 };
#if _DEBUG
	const int m_NrCorners{ 4 }, m_ReqNrCorners{ 3 };
#else
	const int m_NrCorners{ 3 }, m_ReqNrCorners{ 2 };
#endif
	
	// We do one less row as it is above the current block and as the buffer has a 2 pixel row offset
	const int m_Columns{ 10 }, m_Rows{ 17 };
	const int m_Width{ m_BlockSize * m_Columns }, m_Height{ m_BlockSize * m_Rows };

	SDL_Texture* m_pDataTexture;

	std::vector<std::vector<bool>> m_Blocks;
	
public:
	View_System_Playfield() : EView("System View - Playfield", ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar)
	{
		m_pDataTexture = SDL_CreateTexture(Renderer::GetInstance().GetSDLRenderer(),
			SDL_PIXELFORMAT_RGBA4444,
			SDL_TEXTUREACCESS_TARGET,
			m_Width, m_Height);

		m_Blocks.resize(m_Width, std::vector<bool>(m_Height));
	}

	~View_System_Playfield()
	{
		SDL_DestroyTexture(m_pDataTexture);
	}

	void Update() override
	{
		SDL_SetRenderTarget(Renderer::GetInstance().GetSDLRenderer(), m_pDataTexture);
		SDL_SetRenderDrawColor(Renderer::GetInstance().GetSDLRenderer(), 0, 0, 0, 255);
		SDL_RenderClear(Renderer::GetInstance().GetSDLRenderer());

		for(int col{}; col < m_Columns; col++)
		{
			for (int row{}; row < m_Rows; row++)
			{
				std::vector<bool> corners(m_NrCorners);
				const ivec2 startPixel{ m_StartPos.x + col * m_BlockSize, m_StartPos.y + row * m_BlockSize };

				const auto colorBlack = 3;
				if(m_NrCorners >= 1)
					corners[0] = m_pSystem->GetPixelBuffer()[startPixel.x][startPixel.y] == colorBlack;
				if (m_NrCorners >= 2)
					corners[1] = m_pSystem->GetPixelBuffer()[startPixel.x + m_OtherSide][startPixel.y] == colorBlack;
				if (m_NrCorners >= 3)
					corners[2] = m_pSystem->GetPixelBuffer()[startPixel.x][startPixel.y + m_OtherSide] == colorBlack;
				if (m_NrCorners == 4)
					corners[3] = m_pSystem->GetPixelBuffer()[startPixel.x + m_OtherSide][startPixel.y + m_OtherSide] == colorBlack;

				m_Blocks[col][row] = std::count(corners.cbegin(), corners.cend(), true) >= m_ReqNrCorners;

				if (m_Blocks[col][row])
				{
					Renderer::GetInstance().RenderFilledRect(
						{ float(col * m_BlockSize), float(row * m_BlockSize), float(m_BlockSize), float(m_BlockSize) },
						{ 255, 255, 255, 255 });
				}
			}
		}

		SDL_SetRenderTarget(Renderer::GetInstance().GetSDLRenderer(), nullptr);

	}

	void DrawGUI() override
	{
		ImGui::SetNextWindowSize({ 300, 175 });
		ImGui::Begin("System View - Playfield", nullptr, m_Flags);

		ImGui::Image(m_pDataTexture, { float(m_Width), float(m_Height) });
		ImGui::SameLine();

		ImGui::PushItemWidth(120);
		ImGui::BeginGroup();

		const std::string strStartPoint{ "Starting Point: (" + std::to_string(m_StartPos.x) + ", " + std::to_string(m_StartPos.y) + ")" };
		const std::string strColRows{ "Columns x Rows: " + std::to_string(m_Columns) + " x " + std::to_string(m_Rows) };
		const std::string strNrCorners{ "Nr. Corners: " + std::to_string(m_NrCorners) };
		
		ImGui::Text(strStartPoint.c_str());
		ImGui::Text(strColRows.c_str());
		ImGui::Text(strNrCorners.c_str());
		
		ImGui::EndGroup();
		
		ImGui::End();
	}
};