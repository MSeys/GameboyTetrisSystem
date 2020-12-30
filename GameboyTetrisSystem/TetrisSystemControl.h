#pragma once
#include "SystemUtils.h"
class TetrisSystem;
class SystemView_Playfield;
class SystemView_NextPiece;
class SystemView_CurrentPiece;

class TetrisSystemControl
{
public:
	TetrisSystemControl() = default;

	void Initialize(TetrisSystem* pSystem);
	void Update();
	void DrawGUI() const;

private:
	TetrisSystem* m_pSystem{};
	
	SystemView_Playfield* m_pView_Playfield{};
	SystemView_NextPiece* m_pView_NextPiece{};
	SystemView_CurrentPiece* m_pView_CurrentPiece{};

	bool m_CanUpdate{ true };
	TetrisMenu m_CurrentMenu;
	
	// Score Checking
	GameboyBuffer m_ScoreBuffer;
	const ivec2 m_ScoreStart{ 135, 22 }, m_ScoreEnd{ 159, 31 };

	// Game Menu Check
	const ivec2 m_GameMenuStart{ 8, 7 }, m_GameMenuEnd{ 71, 14 };

	// Game Over Check
	const ivec2 m_GameOverStart{ 120, 111 }, m_GameOverEnd{ 151, 126 };

	
	TetrisMenu CheckMenu() const;
	bool CheckScore();
};

