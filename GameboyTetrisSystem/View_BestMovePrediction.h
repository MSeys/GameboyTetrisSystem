#pragma once
#include "EView.h"
#include "Renderer.h"
#include "SystemView_Playfield.h"

class View_BestMovePrediction : public EView
{
	SDL_Texture* m_pDataTexture;

	// We do one less row as it is above the current block and as the buffer has a pixel row offset
	const int m_Columns{ TETRIS_COLUMNS }, m_Rows{ TETRIS_ROWS - 1 };
	const int m_Width{ BLOCK_SIZE * m_Columns }, m_Height{ BLOCK_SIZE * m_Rows };
	int m_MovesDepth{ 2 };
	std::vector<TetrisPiece> m_Pieces;
	BestTetrisMove m_BestMove;
	
	TetrisBlocksContainer m_PredictionPlayfield;
	SystemView_Playfield* m_pView_Playfield{};

public:
	View_BestMovePrediction() : EView("HV - Best Move Prediction", ImGuiWindowFlags_NoCollapse)
	{
		m_pDataTexture = SDL_CreateTexture(Renderer::GetInstance().GetSDLRenderer(),
			SDL_PIXELFORMAT_RGBA4444,
			SDL_TEXTUREACCESS_TARGET,
			m_Width, m_Height);

		m_PredictionPlayfield.resize(m_Columns, std::vector<bool>(m_Rows));
	}

	~View_BestMovePrediction()
	{
		SDL_DestroyTexture(m_pDataTexture);
	}

	void SetViewPlayfield(SystemView_Playfield* pView) { m_pView_Playfield = pView; }

	void Update() override
	{
		std::vector<TetrisPieceData> piecesData(m_MovesDepth);
		for(int i{}; i < m_MovesDepth; i++)
		{
			piecesData[i] = m_pSystem->GetPiecesData().at(m_Pieces[i]);
		}
		
		m_BestMove = SystemUtils::GetBestTetrisMove(m_PredictionPlayfield, piecesData);

		if (!m_BestMove.valid)
			return;

		SDL_SetRenderTarget(Renderer::GetInstance().GetSDLRenderer(), m_pDataTexture);
		SDL_SetRenderDrawColor(Renderer::GetInstance().GetSDLRenderer(), 0, 0, 0, 255);
		SDL_RenderClear(Renderer::GetInstance().GetSDLRenderer());

		Renderer::GetInstance().DrawTetrisBlocks(m_PredictionPlayfield);
		for (const TetrisMove& move : m_BestMove.movesDepth)
		{
			Renderer::GetInstance().DrawTetrisBlocks(move.blockOnly, { 0, 0, 255, 255 });
		}
		
		SDL_SetRenderTarget(Renderer::GetInstance().GetSDLRenderer(), nullptr);
	}

	void DrawGUI() override
	{
		ImGui::Begin(m_Name.c_str(), nullptr, m_Flags);
		ImGui::Image(m_pDataTexture, { float(m_Width), float(m_Height) });

		ImGui::SameLine();

		ImGui::PushItemWidth(120);
		ImGui::BeginGroup();

		ImGui::Text("Best Move Info");
		ImGui::Spacing();
		ImGui::Separator();

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();
		
		ImGui::Text("Moves");
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Text("Moves Depth: ");
		ImGui::SameLine();
		ImGui::DragInt("##MovesDepth", &m_MovesDepth, 1, 1, 5);
		m_Pieces.resize(m_MovesDepth);

		for(int i{}; i < m_MovesDepth; i++)
		{
			int value = int(m_Pieces[i]);
			ImGui::Combo(std::string("##BestMovePiece" + std::to_string(i)).c_str(), &value, "O-Piece\0I-Piece\0S-Piece\0Z-Piece\0L-Piece\0J-Piece\0T-Piece\0");
			m_Pieces[i] = TetrisPiece(value);
		}

		if (ImGui::Button("Update Playfield", { -1, 0 }))
			m_PredictionPlayfield = m_pView_Playfield->GetPlayfield();

		if (ImGui::Button("Update Prediction", { -1, 0 }))
			Update();
		
		ImGui::EndGroup();

		ImGui::End();
	}
};