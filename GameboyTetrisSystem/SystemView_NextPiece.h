#pragma once
#include "EView.h"
#include "Renderer.h"

class SystemView_NextPiece : public EView
{
	const ivec2 m_StartPos{ 120, 111 };

	// We do one less row as it is above the current block and as the buffer has a pixel row offset
	const int m_Columns{ 4 }, m_Rows{ 2 };
	const int m_Width{ BLOCK_SIZE * m_Columns }, m_Height{ BLOCK_SIZE * m_Rows };

	SDL_Texture* m_pDataTexture;

	TetrisBlocksContainer m_Blocks;

public:
	SystemView_NextPiece() : EView("System View - Next Piece", ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar)
	{
		m_pDataTexture = SDL_CreateTexture(Renderer::GetInstance().GetSDLRenderer(),
			SDL_PIXELFORMAT_RGBA4444,
			SDL_TEXTUREACCESS_TARGET,
			m_Width, m_Height);

		m_Blocks.resize(m_Columns, std::vector<bool>(m_Rows));
	}

	~SystemView_NextPiece()
	{
		SDL_DestroyTexture(m_pDataTexture);
	}

	void Update() override
	{
		SDL_SetRenderTarget(Renderer::GetInstance().GetSDLRenderer(), m_pDataTexture);
		SDL_SetRenderDrawColor(Renderer::GetInstance().GetSDLRenderer(), 0, 0, 0, 255);
		SDL_RenderClear(Renderer::GetInstance().GetSDLRenderer());

		SystemUtils::SetBlocks(m_Blocks, m_StartPos, m_pSystem->GetPixelBuffer());

		SDL_SetRenderTarget(Renderer::GetInstance().GetSDLRenderer(), nullptr);

	}

	void DrawGUI() override
	{
		ImGui::SetNextWindowSize({ 200, 175 });
		ImGui::Begin(m_Name.c_str(), nullptr, m_Flags);

		ImGui::Spacing();
		ImGui::SameLine(70);
		ImGui::Image(m_pDataTexture, { float(m_Width * 2.f), float(m_Height * 2.f) });

		ImGui::BeginGroup();

		const std::string strStartPoint{ "Starting Point: (" + std::to_string(m_StartPos.x) + ", " + std::to_string(m_StartPos.y) + ")" };
		const std::string strColRows{ "Columns x Rows: " + std::to_string(m_Columns) + " x " + std::to_string(m_Rows) };
		const std::string strNrCorners{ "Nr. Corners: " + std::to_string(CORNERS) };
		const std::string strNrReqCorners{ "Nr. Req. Corners: " + std::to_string(REQ_CORNERS) };
		const std::string strTetrisPiece{ "Piece: " + SystemUtils::TetrisPieceToString(SystemUtils::GetPiece(m_Blocks)) };
		
		ImGui::Text(strStartPoint.c_str());
		ImGui::Text(strColRows.c_str());
		ImGui::Text(strNrCorners.c_str());
		ImGui::Text(strTetrisPiece.c_str());

		ImGui::EndGroup();

		ImGui::End();
	}
};