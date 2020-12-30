#pragma once
#include "EView.h"
#include "Renderer.h"

class SystemView_Playfield : public EView
{
	const ivec2 m_StartPos{ 16, 7 };
	
	// We do one less row as it is above the current block and as the buffer has a pixel row offset
	const int m_Columns{ TETRIS_COLUMNS }, m_Rows{ TETRIS_ROWS - 1 };
	const int m_Width{ BLOCK_SIZE * m_Columns }, m_Height{ BLOCK_SIZE * m_Rows };

	SDL_Texture* m_pDataTexture;

	TetrisBlocksContainer m_Blocks;

	void RemoveCurrentPiece()
	{
		for(int x{}; x < 4; x++)
		{
			for(int y{}; y < 2; y++)
			{
				m_Blocks[3 + x][y] = false;
			}
		}
	}
	
public:
	SystemView_Playfield() : EView("System View - Playfield", ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar)
	{
		m_pDataTexture = SDL_CreateTexture(Renderer::GetInstance().GetSDLRenderer(),
			SDL_PIXELFORMAT_RGBA4444,
			SDL_TEXTUREACCESS_TARGET,
			m_Width, m_Height);

		m_Blocks.resize(m_Columns, std::vector<bool>(m_Rows));
	}

	~SystemView_Playfield()
	{
		SDL_DestroyTexture(m_pDataTexture);
	}

	TetrisBlocksContainer GetPlayfield() const { return m_Blocks; }
	
	void Update() override
	{
		SDL_SetRenderTarget(Renderer::GetInstance().GetSDLRenderer(), m_pDataTexture);
		SDL_SetRenderDrawColor(Renderer::GetInstance().GetSDLRenderer(), 0, 0, 0, 255);
		SDL_RenderClear(Renderer::GetInstance().GetSDLRenderer());

		SystemUtils::CheckTetrisBlocks(m_Blocks, m_StartPos, m_pSystem->GetPixelBuffer());
		RemoveCurrentPiece();
		
		Renderer::GetInstance().DrawTetrisBlocks(m_Blocks);
		
		SDL_SetRenderTarget(Renderer::GetInstance().GetSDLRenderer(), nullptr);
	}

	void DrawGUI() override
	{
		ImGui::SetNextWindowSize({ 300, 175 });
		ImGui::Begin(m_Name.c_str(), nullptr, m_Flags);

		ImGui::Image(m_pDataTexture, { float(m_Width), float(m_Height) });
		ImGui::SameLine();

		ImGui::PushItemWidth(120);
		ImGui::BeginGroup();

		const std::string strStartPoint{ "Starting Point: (" + std::to_string(m_StartPos.x) + ", " + std::to_string(m_StartPos.y) + ")" };
		const std::string strColRows{ "Columns x Rows: " + std::to_string(m_Columns) + " x " + std::to_string(m_Rows) };

		ImGui::Text(strStartPoint.c_str());
		ImGui::Text(strColRows.c_str());

		ImGui::EndGroup();
		
		ImGui::End();
	}
};