#include "TetrisSystemControl.h"

#include "TetrisSystem.h"
#include "EViewHolder.h"
#include "View_Gameboy.h"
#include "View_PixelInspector.h"
#include "SystemView_Playfield.h"
#include "SystemView_NextPiece.h"
#include "SystemView_CurrentPiece.h"

void TetrisSystemControl::Initialize(TetrisSystem* pSystem)
{
	m_pSystem = pSystem;
	
	EViewHolder::GetInstance().AddView<View_Gameboy>()->SetSystem(pSystem);
	EViewHolder::GetInstance().AddView<View_PixelInspector>()->SetSystem(pSystem);
	m_pView_Playfield = EViewHolder::GetInstance().AddView<SystemView_Playfield>();
	m_pView_Playfield->SetSystem(pSystem);

	m_pView_NextPiece = EViewHolder::GetInstance().AddView<SystemView_NextPiece>();
	m_pView_NextPiece->SetSystem(pSystem);

	m_pView_CurrentPiece = EViewHolder::GetInstance().AddView<SystemView_CurrentPiece>();
	m_pView_CurrentPiece->SetSystem(pSystem);
}

void TetrisSystemControl::Update()
{
	m_CurrentMenu = CheckMenu();

	if (m_CurrentMenu == TetrisMenu::PLAY)
	{
		if (m_CanUpdate)
		{
			m_pView_Playfield->Update();
			m_pView_NextPiece->Update();
			m_pView_CurrentPiece->Update();

			if (m_pView_CurrentPiece->GetCurrentPiece() != TetrisPiece::NO_PIECE)
				m_CanUpdate = false;
		}

		else
		{
			m_CanUpdate = CheckScore();
		}
	}
}

void TetrisSystemControl::DrawGUI()
{
	EViewHolder::GetInstance().DrawViews();
	
	ImGui::Begin("System View - System Control", nullptr, ImGuiWindowFlags_NoCollapse);

	ImGui::Text("Screen Checking");
	ImGui::Separator();
	ImGui::Text(std::string("Current Screen: " + SystemUtils::TetrisMenuToString(m_CurrentMenu)).c_str());
	ImGui::Text(std::string("Score Pos: " + m_ScoreStart.ToString() + " ==> " + m_ScoreEnd.ToString()).c_str());
	
	ImGui::Spacing();
	
	ImGui::Text("Block Checking");
	ImGui::Separator();
	ImGui::Text(std::string("Corner Checks: " + std::to_string(CORNERS)).c_str());
	ImGui::Text(std::string("Req Corner: " + std::to_string(REQ_CORNERS)).c_str());
	
	ImGui::End();
}

TetrisMenu TetrisSystemControl::CheckMenu() const
{
	const auto R1 = SystemUtils::GetPixels({ 0, 0 }, { GAMEBOY_SCREEN_X - 1, 0 }, m_pSystem->GetPixelBuffer());
	const auto R2 = SystemUtils::GetPixels({ 0, 1 }, { GAMEBOY_SCREEN_X - 1, 1 }, m_pSystem->GetPixelBuffer());

	
	const int nrBlackR1 = SystemUtils::CountVector2D<uint8_t>(R1, COLOR_BLACK);
	const int nrWhiteR1 = SystemUtils::CountVector2D<uint8_t>(R1, COLOR_WHITE);

	// START - Only start is full black on top row
	if(nrBlackR1 == GAMEBOY_SCREEN_X)
		return TetrisMenu::START;

	// CREDITS - GAME MODE SELECT - LEVEL SELECT
	if(nrWhiteR1 == GAMEBOY_SCREEN_X)
	{
		if (SystemUtils::CountVector2D<uint8_t>(m_pSystem->GetPixelBuffer(), COLOR_LIGHTGREY) == 0)
			return TetrisMenu::CREDITS;

		const auto gamemodePart = SystemUtils::GetPixels(m_GameMenuStart, m_GameMenuEnd, m_pSystem->GetPixelBuffer());
		if (SystemUtils::CountVector2D<uint8_t>(gamemodePart, COLOR_LIGHTGREY) == 0)
			return TetrisMenu::LEVEL_SELECT;
		
		return TetrisMenu::GAME_SELECT;
	}

	const int columns{ 10 };
	if(nrWhiteR1 >= columns * BLOCK_SIZE)
	{
		const auto gameOverPart = SystemUtils::GetPixels(m_GameOverStart, m_GameOverEnd, m_pSystem->GetPixelBuffer());
		if (SystemUtils::CountVector2D<uint8_t>(gameOverPart, COLOR_BLACK) == 0)
			return TetrisMenu::GAME_OVER;

		return TetrisMenu::PLAY;
	}

	return TetrisMenu::GAME_OVER; // If none of the above are applicable, then there is one case left of the playfield going game over (blocks come from bottom to top)
}

bool TetrisSystemControl::CheckScore()
{
	auto currentScoreBuffer = SystemUtils::GetPixels(m_ScoreStart, m_ScoreEnd, m_pSystem->GetPixelBuffer());
	if(m_ScoreBuffer != currentScoreBuffer)
	{
		m_ScoreBuffer = currentScoreBuffer;
		return true;
	}

	return false;
}
