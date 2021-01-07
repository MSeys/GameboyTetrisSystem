#pragma once
#include "EView.h"
#include "Renderer.h"
#include "SystemView_Playfield.h"

class View_BlockPrediction : public EView
{
	SDL_Texture* m_pDataTexture;

	// We do one less row as it is above the current block and as the buffer has a pixel row offset
	const int m_Columns{ TETRIS_COLUMNS }, m_Rows{ TETRIS_ROWS - 1 };
	const int m_Width{ BLOCK_SIZE * m_Columns }, m_Height{ BLOCK_SIZE * m_Rows };
	int m_CurrentPiece{}, m_PieceMovement{}, m_PieceRotation{};
	TetrisPieceData m_CurrentPieceData;
	TetrisMove m_LatestPredictionMove;

	TetrisBlocksContainer m_PredictionPlayfield;
	SystemView_Playfield* m_pView_Playfield{};
	
public:
	View_BlockPrediction() : EView("HV - Block Prediction", ImGuiWindowFlags_NoCollapse)
	{
		m_pDataTexture = SDL_CreateTexture(Renderer::GetInstance().GetSDLRenderer(),
			SDL_PIXELFORMAT_RGBA4444,
			SDL_TEXTUREACCESS_TARGET,
			m_Width, m_Height);

		m_PredictionPlayfield.resize(m_Columns, std::vector<bool>(m_Rows));
	}

	~View_BlockPrediction()
	{
		SDL_DestroyTexture(m_pDataTexture);
	}

	void SetViewPlayfield(SystemView_Playfield* pView) { m_pView_Playfield = pView; }
	
	void Update() override
	{
		m_LatestPredictionMove = SystemUtils::GetTetrisMove(m_PredictionPlayfield,
			m_CurrentPieceData[m_PieceRotation], 
			m_PieceMovement, m_PieceRotation);
		
		if(!m_LatestPredictionMove.valid)
			return;

		SDL_SetRenderTarget(Renderer::GetInstance().GetSDLRenderer(), m_pDataTexture);
		SDL_SetRenderDrawColor(Renderer::GetInstance().GetSDLRenderer(), 0, 0, 0, 255);
		SDL_RenderClear(Renderer::GetInstance().GetSDLRenderer());

		Renderer::GetInstance().DrawTetrisBlocks(m_PredictionPlayfield);
		Renderer::GetInstance().DrawTetrisBlocks(m_LatestPredictionMove.blockOnly, { 0, 0, 255, 255 });

		SDL_SetRenderTarget(Renderer::GetInstance().GetSDLRenderer(), nullptr);
	}

	void DrawGUI() override
	{
		ImGui::Begin(m_Name.c_str(), nullptr, m_Flags);
		ImGui::Image(m_pDataTexture, { float(m_Width), float(m_Height) });

		ImGui::SameLine();

		ImGui::PushItemWidth(120);
		ImGui::BeginGroup();
		
		if(ImGui::BeginTabBar("##PredictionTabBar"))
		{
			if(ImGui::BeginTabItem("Prediction"))
			{
				ImGui::Text("Current Piece: ");
				ImGui::SameLine();
				ImGui::Combo("##CurentPieceCombo", &m_CurrentPiece, "O-Piece\0I-Piece\0S-Piece\0Z-Piece\0L-Piece\0J-Piece\0T-Piece\0");
				m_CurrentPieceData = m_pSystem->GetPiecesData().at(TetrisPiece(m_CurrentPiece));

				ImGui::Text("Rotation: ");
				ImGui::SameLine();
				ImGui::DragInt("##CurrentPieceRotation", &m_PieceRotation, 1, 0, int(m_CurrentPieceData.size()) - 1);
				m_PieceRotation = Clamp<int>(0, int(m_CurrentPieceData.size()) - 1, m_PieceRotation);

				ImGui::Text("Movement: ");
				ImGui::SameLine();
				ImGui::DragInt("##CurrentPieceMovement", &m_PieceMovement, 1, -m_CurrentPieceData[m_PieceRotation].nrMoveLeft, m_CurrentPieceData[m_PieceRotation].nrMoveRight);

				if (ImGui::Button("Update Playfield", { -1, 0 }))
					m_PredictionPlayfield = m_pView_Playfield->GetPlayfield();

				if (ImGui::Button("Update Prediction", { -1, 0 }))
					Update();			
				
				ImGui::EndTabItem();
			}

			if(ImGui::BeginTabItem("Prediction Data"))
			{
				if (m_LatestPredictionMove.valid)
					ImGui::Text("Valid Move: true");
				else
					ImGui::Text("Valid Move: false");

				ImGui::Text(std::string("Score: " + std::to_string(m_LatestPredictionMove.hScore)).c_str());
				ImGui::Separator();
				ImGui::Text(std::string("Aggregate Height: " + std::to_string(m_LatestPredictionMove.hAggregateHeight)).c_str());
				ImGui::Text(std::string("Completed Lines: " + std::to_string(m_LatestPredictionMove.hCompleteLines)).c_str());
				ImGui::Text(std::string("Holes: " + std::to_string(m_LatestPredictionMove.hHoles)).c_str());
				ImGui::Text(std::string("Bumpiness: " + std::to_string(m_LatestPredictionMove.hBumpiness)).c_str());
				
				ImGui::EndTabItem();
			}
		}

		ImGui::EndTabBar();	
		ImGui::EndGroup();
		
		ImGui::End();
	}
};