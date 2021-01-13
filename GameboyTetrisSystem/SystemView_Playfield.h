#pragma once
#include "EView.h"
#include "Renderer.h"

class SystemView_Playfield : public EView
{
	const ivec2 m_StartPos{ 16, 7 };
	
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
	SystemView_Playfield() : EView("SV - Playfield", ImGuiWindowFlags_NoCollapse)
	{
		m_pDataTexture = SDL_CreateTexture(Renderer::GetInstance().GetSDLRenderer(),
			SDL_PIXELFORMAT_RGBA4444,
			SDL_TEXTUREACCESS_TARGET,
			PLAYFIELD_SIZE_X, PLAYFIELD_SIZE_Y);

		m_Blocks.resize(PLAYFIELD_SIZE_X / BLOCK_SIZE, std::vector<bool>(PLAYFIELD_SIZE_Y / BLOCK_SIZE));
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
		ImGui::Begin(m_Name.c_str(), nullptr, m_Flags);

		ImGui::Image(m_pDataTexture, { float(PLAYFIELD_SIZE_X), float(PLAYFIELD_SIZE_Y) });

		ImGui::PushItemWidth(120);
		ImGui::BeginGroup();

		const std::string strStartPoint{ "Starting Point: (" + std::to_string(m_StartPos.x) + ", " + std::to_string(m_StartPos.y) + ")" };
		const std::string strColRows{ "Columns x Rows: " + std::to_string(PLAYFIELD_SIZE_X / BLOCK_SIZE) + " x " + std::to_string(PLAYFIELD_SIZE_Y / BLOCK_SIZE) };

		ImGui::Text(strStartPoint.c_str());
		ImGui::Text(strColRows.c_str());

		ImGui::EndGroup();
		
		ImGui::End();
	}
};