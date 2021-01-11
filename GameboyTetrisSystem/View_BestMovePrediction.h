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
	int m_MovesDepth{ 2 }, m_NrMaxDepth{ 5 };
	std::vector<TetrisPiece> m_Pieces;
	std::vector<SDL_Color> m_PieceColors;
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
		m_Pieces.resize(m_NrMaxDepth);
		m_PieceColors.resize(m_NrMaxDepth);

		for (SDL_Color& color : m_PieceColors)
			color = { Uint8(rand() % 255), Uint8(rand() % 255), Uint8(rand() % 255), 255 };
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
		for (int i{}; i < int(m_BestMove.movesDepth.size()); i++)
			Renderer::GetInstance().DrawTetrisBlocks(m_BestMove.movesDepth[i].blockOnly, m_PieceColors[i]);

		SDL_SetRenderTarget(Renderer::GetInstance().GetSDLRenderer(), nullptr);
	}

	void DrawGUI() override
	{
		if (!ImGui::Begin(m_Name.c_str(), nullptr, m_Flags))
		{
			ImGui::End();
			return;
		}
		
		ImGui::Image(m_pDataTexture, { float(m_Width), float(m_Height) });

		ImGui::SameLine();

		ImGui::PushItemWidth(120);
		ImGui::BeginGroup();

		if (ImGui::BeginTabBar("##PredictionTabBar"))
		{
			if (ImGui::BeginTabItem("Prediction"))
			{
				ImGui::Text("Moves Depth: ");
				ImGui::SameLine();
				ImGui::DragInt("##MovesDepth", &m_MovesDepth, 1, 1, m_NrMaxDepth);

				for (int i{}; i < m_MovesDepth; i++)
				{
					ImGui::PushID(i);
					
					int value = int(m_Pieces[i]);
					ImGui::Combo("##BestMovePiece", &value, "O-Piece\0I-Piece\0S-Piece\0Z-Piece\0L-Piece\0J-Piece\0T-Piece\0");
					m_Pieces[i] = TetrisPiece(value);

					float colorF[3]{ m_PieceColors[i].r / 255.f, m_PieceColors[i].g / 255.f, m_PieceColors[i].b / 255.f };
					ImGui::SameLine();
					ImGui::ColorEdit3("##PieceColor", colorF, ImGuiColorEditFlags_NoInputs);
					m_PieceColors[i] = { Uint8(colorF[0] * 255), Uint8(colorF[1] * 255), Uint8(colorF[2] * 255), 255 };

					ImGui::PopID();
				}

				if (ImGui::Button("Update Playfield", { -1, 0 }))
					m_PredictionPlayfield = m_pView_Playfield->GetPlayfield();

				if (ImGui::Button("Update Prediction", { -1, 0 }))
					Update();

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Prediction Data"))
			{
				if (m_BestMove.valid)
					ImGui::Text("Valid Move: true");
				else
					ImGui::Text("Valid Move: false");

				ImGui::Text(std::string("Score: " + std::to_string(m_BestMove.hScore)).c_str());
				ImGui::Separator();
				ImGui::Text(std::string("Aggregate Height: " + std::to_string(m_BestMove.hAggregateHeight)).c_str());
				ImGui::Text(std::string("Completed Lines: " + std::to_string(m_BestMove.hCompleteLines)).c_str());
				ImGui::Text(std::string("Holes: " + std::to_string(m_BestMove.hHoles)).c_str());
				ImGui::Text(std::string("Bumpiness: " + std::to_string(m_BestMove.hBumpiness)).c_str());

				
				ImGui::EndTabItem();
			}
		}

		ImGui::EndTabBar();
		ImGui::EndGroup();

		ImGui::End();
	}
};