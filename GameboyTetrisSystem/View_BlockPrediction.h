#pragma once
#include "EView.h"
#include "Renderer.h"
#include "SystemView_Playfield.h"

class View_BlockPrediction : public EView
{
	SDL_Texture* m_pDataTexture;

	int m_NrPieces{ 1 }, m_NrMaxPieces{ 5 }, m_CurrentPieceID{ 0 };
	
	std::vector<TetrisPiece> m_Pieces{};
	std::vector<int> m_PieceMovements{}, m_PieceRotations{};
	std::vector<TetrisMove> m_Moves{};
	std::vector<SDL_Color> m_PieceColors{};

	TetrisBlocksContainer m_PredictionPlayfield;
	SystemView_Playfield* m_pView_Playfield{};
	
public:
	View_BlockPrediction() : EView("HV - Block Prediction", ImGuiWindowFlags_NoCollapse)
	{
		m_pDataTexture = SDL_CreateTexture(Renderer::GetInstance().GetSDLRenderer(),
			SDL_PIXELFORMAT_RGBA4444,
			SDL_TEXTUREACCESS_TARGET,
			PLAYFIELD_SIZE_X, PLAYFIELD_SIZE_Y);

		m_PredictionPlayfield.resize(PLAYFIELD_SIZE_X / BLOCK_SIZE, std::vector<bool>(PLAYFIELD_SIZE_Y / BLOCK_SIZE));

		m_Pieces.resize(m_NrMaxPieces);
		m_PieceMovements.resize(m_NrMaxPieces);
		m_PieceRotations.resize(m_NrMaxPieces);
		m_Moves.resize(m_NrMaxPieces, { false });
		m_PieceColors.resize(m_NrMaxPieces);
		
		for (SDL_Color& color : m_PieceColors)
			color = { Uint8(rand() % 255), Uint8(rand() % 255), Uint8(rand() % 255), 255 };
	}

	~View_BlockPrediction()
	{
		SDL_DestroyTexture(m_pDataTexture);
	}

	void SetViewPlayfield(SystemView_Playfield* pView) { m_pView_Playfield = pView; }
	
	void Update() override
	{
		if(!m_Moves[m_CurrentPieceID].valid)
			return;

		SDL_SetRenderTarget(Renderer::GetInstance().GetSDLRenderer(), m_pDataTexture);
		SDL_SetRenderDrawColor(Renderer::GetInstance().GetSDLRenderer(), 0, 0, 0, 255);
		SDL_RenderClear(Renderer::GetInstance().GetSDLRenderer());

		Renderer::GetInstance().DrawTetrisBlocks(m_PredictionPlayfield);
		for (int i{}; i <= m_CurrentPieceID; i++)
		{
			if (!m_Moves[i].valid)
				continue;
			
			Renderer::GetInstance().DrawTetrisBlocks(m_Moves[i].blockOnly, m_PieceColors[i]);
		}

		SDL_SetRenderTarget(Renderer::GetInstance().GetSDLRenderer(), nullptr);
	}

	void UpdateMove(int i)
	{
		TetrisBlocksContainer playfield{ m_PredictionPlayfield };

		for (int id{ i - 1 }; id >= 0; id--)
		{
			if (!m_Moves[id].valid)
				continue;
			
			playfield = playfield + m_Moves[id].blockOnly;
		}
		
		m_Moves[i] = SystemUtils::GetTetrisMove(playfield,
			m_pSystem->GetPiecesData().at(m_Pieces[i])[m_PieceRotations[i]],
			m_PieceMovements[i], m_PieceRotations[i]);
	}

	void DrawPiecePrediction(int i)
	{		
		if (ImGui::BeginTabBar("##PredictionPiece"))
		{
			if (ImGui::BeginTabItem("Prediction"))
			{
				ImGui::Text("Piece: ");
				ImGui::SameLine();

				int value = int(m_Pieces[i]);
				ImGui::Combo(std::string("##PieceCombo" + std::to_string(i)).c_str(), &value, "O-Piece\0I-Piece\0S-Piece\0Z-Piece\0L-Piece\0J-Piece\0T-Piece\0");
				m_Pieces[i] = TetrisPiece(value);

				const auto pieceData{ m_pSystem->GetPiecesData().at(m_Pieces[i]) };
				
				ImGui::Text("Rotation: ");
				ImGui::SameLine();
				ImGui::DragInt("##CurrentPieceRotation", &m_PieceRotations[i], 1, 0, int(pieceData.size()) - 1);
				m_PieceRotations[i] = Clamp<int>(0, int(pieceData.size()) - 1, m_PieceRotations[i]);

				ImGui::Text("Movement: ");
				ImGui::SameLine();
				ImGui::DragInt("##CurrentPieceMovement", &m_PieceMovements[i], 1, -pieceData[m_PieceRotations[i]].nrMoveLeft, pieceData[m_PieceRotations[i]].nrMoveRight);
				m_PieceMovements[i] = Clamp<int>(-pieceData[m_PieceRotations[i]].nrMoveLeft, pieceData[m_PieceRotations[i]].nrMoveRight, m_PieceMovements[i]);
				
				float colorF[3]{ m_PieceColors[i].r / 255.f, m_PieceColors[i].g / 255.f, m_PieceColors[i].b / 255.f };
				ImGui::Text("Piece Color: ");
				ImGui::SameLine();
				ImGui::ColorEdit3("##PieceColor", colorF, ImGuiColorEditFlags_NoInputs);
				m_PieceColors[i] = { Uint8(colorF[0] * 255), Uint8(colorF[1] * 255), Uint8(colorF[2] * 255), 255 };
				
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Prediction Data"))
			{
				if (m_Moves[i].valid)
					ImGui::Text("Valid Move: true");
				else
					ImGui::Text("Valid Move: false");

				ImGui::Text(std::string("Score: " + std::to_string(m_Moves[i].hScore)).c_str());
				ImGui::Separator();
				ImGui::Text(std::string("Aggregate Height: " + std::to_string(m_Moves[i].hAggregateHeight)).c_str());
				ImGui::Text(std::string("Completed Lines: " + std::to_string(m_Moves[i].hCompleteLines)).c_str());
				ImGui::Text(std::string("Holes: " + std::to_string(m_Moves[i].hHoles)).c_str());
				ImGui::Text(std::string("Bumpiness: " + std::to_string(m_Moves[i].hBumpiness)).c_str());

				ImGui::EndTabItem();
			}
		}

		ImGui::EndTabBar();
	}

	void DrawGUI() override
	{
		if (!ImGui::Begin(m_Name.c_str(), nullptr, m_Flags))
		{
			ImGui::End();
			return;
		}

		ImGui::Image(m_pDataTexture, { float(PLAYFIELD_SIZE_X), float(PLAYFIELD_SIZE_Y) });

		ImGui::SameLine();

		ImGui::PushItemWidth(120);
		ImGui::BeginGroup();

		ImGui::Text("Pieces: ");
		ImGui::SameLine();
		if (ImGui::DragInt("##NrPieces", &m_NrPieces, 1, 1, m_NrMaxPieces))
		{
			if(m_CurrentPieceID >= m_NrPieces)
				m_CurrentPieceID = m_NrPieces - 1;
		}
		
		ImGui::BeginTabBar("##PredictionPieces");
		
		for (int i{}; i < m_NrPieces; i++)
		{
			if (ImGui::BeginTabItem(std::string("P" + std::to_string(i)).c_str()))
			{
				m_CurrentPieceID = i;
				
				ImGui::PushID(i);
				DrawPiecePrediction(i);
				ImGui::PopID();

				UpdateMove(i);
				ImGui::EndTabItem();
			}
		}

		ImGui::EndTabBar();
		ImGui::EndGroup();

		if (ImGui::Button("Update Playfield", { -1, 0 }))
			m_PredictionPlayfield = m_pView_Playfield->GetPlayfield();

		
		ImGui::End();

		Update();
	}
};
