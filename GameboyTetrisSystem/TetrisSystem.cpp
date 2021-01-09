#include "TetrisSystem.h"


#include <algorithm>
#include <iostream>


#include "EViewHolder.h"
#include "View_Gameboy.h"
#include "View_PixelInspector.h"
#include "View_BlockPrediction.h"

#include "SystemView_Playfield.h"
#include "SystemView_NextPiece.h"
#include "SystemView_CurrentPiece.h"

#include "Renderer.h"
#include "View_BestMovePrediction.h"

#define GuiColor ImVec4{ 0, 118.f / 255.f, 210.f / 255.f, 1.f }

TetrisSystem::TetrisSystem(gbee::Emulator& emulator, const vec2& windowSize)
	: m_WindowSize(windowSize), m_Emulator(emulator)
{
	m_PixelBuffer.resize(GAMEBOY_SCREEN_X, std::vector<uint8_t>(GAMEBOY_SCREEN_Y));
}

void TetrisSystem::Initialize()
{
	Initialize_Backend();
	Initialize_System();
}

void TetrisSystem::Run()
{
	while(!m_Exit)
	{
		//ResetKeys();
		
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			Run_SetKeyState(e);
			ImGuiSDL::UpdateInput(&e);

			if (e.type == SDL_QUIT) 
				m_Exit = true;
		}

		ImGui::NewFrame();

		SDL_SetRenderDrawColor(Renderer::GetInstance().GetSDLRenderer(), 114, 144, 154, 255);
		SDL_RenderClear(Renderer::GetInstance().GetSDLRenderer());

		Run_Internal();
				
		ImGui::Render();
		ImGuiSDL::Render(ImGui::GetDrawData());

		SDL_RenderPresent(Renderer::GetInstance().GetSDLRenderer());
	}
}

void TetrisSystem::Quit()
{
	Renderer::GetInstance().Destroy();
	SDL_DestroyWindow(m_pWindow);
	m_pWindow = nullptr;
	SDL_Quit();
}

void TetrisSystem::Initialize_Backend()
{
	SDL_Init(SDL_INIT_EVERYTHING);

	m_pWindow = SDL_CreateWindow("TetrisSystem - Matthias Seys",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		int(m_WindowSize.x), int(m_WindowSize.y),
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

	Renderer::GetInstance().Init(m_pWindow, m_WindowSize);

	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = 1.0f / m_FPS; //Runs a tick behind.. (Due to while loop condition)
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	Initialize_ImGuiStyle();
}

void TetrisSystem::Initialize_ImGuiStyle()
{
	ImGuiStyle* style = &ImGui::GetStyle();
	ImVec4* colors = style->Colors;

	colors[ImGuiCol_FrameBg] = ImVec4(0.160f, 0.160f, 0.160f, 1.000f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.200f, 0.200f, 0.200f, 1.000f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.280f, 0.280f, 0.280f, 1.000f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.160f, 0.160f, 0.160f, 1.000f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.277f, 0.277f, 0.277f, 1.000f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.300f, 0.300f, 0.300f, 1.000f);
	colors[ImGuiCol_ScrollbarGrabActive] = GuiColor;
	colors[ImGuiCol_CheckMark] = GuiColor;
	colors[ImGuiCol_SliderGrab] = ImVec4(0.391f, 0.391f, 0.391f, 1.000f);
	colors[ImGuiCol_SliderGrabActive] = GuiColor;
	colors[ImGuiCol_Button] = colors[ImGuiCol_FrameBg];
	colors[ImGuiCol_ButtonHovered] = colors[ImGuiCol_FrameBgHovered];
	colors[ImGuiCol_ButtonActive] = colors[ImGuiCol_FrameBgActive];
	colors[ImGuiCol_Header] = ImVec4(0.313f, 0.313f, 0.313f, 1.000f);
	colors[ImGuiCol_HeaderHovered] = GuiColor;
	colors[ImGuiCol_HeaderActive] = GuiColor;
	colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.391f, 0.391f, 0.391f, 1.000f);
	colors[ImGuiCol_SeparatorActive] = GuiColor;
	colors[ImGuiCol_ResizeGrip] = ImVec4(1.000f, 1.000f, 1.000f, 0.250f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.000f, 1.000f, 1.000f, 0.670f);
	colors[ImGuiCol_ResizeGripActive] = GuiColor;
	colors[ImGuiCol_Tab] = colors[ImGuiCol_FrameBg];
	colors[ImGuiCol_TabHovered] = GuiColor;
	colors[ImGuiCol_TabActive] = GuiColor;
	colors[ImGuiCol_TabUnfocused] = colors[ImGuiCol_Tab];
	colors[ImGuiCol_TabUnfocusedActive] = colors[ImGuiCol_TabActive];
	colors[ImGuiCol_DockingPreview] = ImVec4(GuiColor.x, GuiColor.y, GuiColor.z, 0.781f);
	colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.180f, 0.180f, 0.180f, 1.000f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
	colors[ImGuiCol_PlotLinesHovered] = GuiColor;
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.586f, 0.586f, 0.586f, 1.000f);
	colors[ImGuiCol_PlotHistogramHovered] = GuiColor;
	colors[ImGuiCol_TextSelectedBg] = ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
	colors[ImGuiCol_DragDropTarget] = GuiColor;
	colors[ImGuiCol_NavHighlight] = GuiColor;
	colors[ImGuiCol_NavWindowingHighlight] = GuiColor;
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);

	style->ChildRounding = 4.0f;
	style->FrameBorderSize = 1.0f;
	style->FrameRounding = 2.0f;
	style->GrabMinSize = 7.0f;
	style->PopupRounding = 2.0f;
	style->ScrollbarRounding = 12.0f;
	style->ScrollbarSize = 13.0f;
	style->TabBorderSize = 1.0f;
	style->TabRounding = 2.0f;
	style->WindowRounding = 4.0f;
	style->WindowTitleAlign = { 0.5f, 0.5f };
}

void TetrisSystem::Initialize_System()
{
	EViewHolder::GetInstance().AddView<View_Gameboy>()->SetSystem(this);
	EViewHolder::GetInstance().AddView<View_PixelInspector>()->SetSystem(this);
	m_pView_Playfield = EViewHolder::GetInstance().AddView<SystemView_Playfield>();
	m_pView_Playfield->SetSystem(this);

	m_pView_NextPiece = EViewHolder::GetInstance().AddView<SystemView_NextPiece>();
	m_pView_NextPiece->SetSystem(this);

	m_pView_CurrentPiece = EViewHolder::GetInstance().AddView<SystemView_CurrentPiece>();
	m_pView_CurrentPiece->SetSystem(this);

	auto pView_BlockPrediction = EViewHolder::GetInstance().AddView<View_BlockPrediction>();
	pView_BlockPrediction->SetSystem(this);
	pView_BlockPrediction->SetViewPlayfield(m_pView_Playfield);

	auto pView_BestMovePrediction = EViewHolder::GetInstance().AddView<View_BestMovePrediction>();
	pView_BestMovePrediction->SetSystem(this);
	pView_BestMovePrediction->SetViewPlayfield(m_pView_Playfield);
	
	Initialize_PieceO();
	Initialize_PieceI();
	Initialize_PieceS();
	Initialize_PieceZ();
	Initialize_PieceL();
	Initialize_PieceJ();
	Initialize_PieceT();
}

void TetrisSystem::Run_SetKeyState(const SDL_Event& event) const
{
	if (event.type != SDL_KEYDOWN && event.type != SDL_KEYUP) return;

	gbee::Key key;
	switch (event.key.keysym.sym) {
	case SDLK_a:
		key = gbee::aButton;
		break;
	case SDLK_d:
		key = gbee::bButton;
		break;
	case SDLK_RETURN:
		key = gbee::start;
		break;
	case SDLK_SPACE:
		key = gbee::select;
		break;
	case SDLK_RIGHT:
		key = gbee::right;
		break;
	case SDLK_LEFT:
		key = gbee::left;
		break;
	case SDLK_UP:
		key = gbee::up;
		break;
	case SDLK_DOWN:
		key = gbee::down;
		break;
	default:
		return;
	}
	m_Emulator.SetKeyState(key, event.type == SDL_KEYDOWN, 0);
}

void TetrisSystem::Run_Internal()
{
	UpdatePixelBuffer();
	UpdateSystem();
	DrawSystemData();
}

void TetrisSystem::UpdatePixelBuffer()
{
	auto bitBuffer = m_Emulator.GetFrameBuffer(0);
	for(int pw{}; pw < GAMEBOY_SCREEN_X; pw++)
	{
		for(int ph{}; ph < GAMEBOY_SCREEN_Y; ph++)
		{
			int i = pw + ph * GAMEBOY_SCREEN_X;
			const uint8_t val{ uint8_t((bitBuffer[i * 2] << 1) | uint8_t(bitBuffer[i * 2 + 1])) };
			m_PixelBuffer[pw][ph] = val;
		}
	}
}

void TetrisSystem::UpdateSystem()
{
	m_CurrentMenu = CheckMenu();
	if (m_CurrentMenu == TetrisMenu::START || m_CurrentMenu == TetrisMenu::GAME_SELECT)
	{
		UseKey(gbee::Key::start, 4);
	}
	
	if (m_CurrentMenu == TetrisMenu::PLAY)
	{
		if (m_CanUpdatePlayData)
		{
			m_pView_Playfield->Update();
			m_pView_NextPiece->Update();
			m_pView_CurrentPiece->Update();

			if (m_pView_CurrentPiece->GetCurrentPiece() != TetrisPiece::NO_PIECE)
			{
				m_CanUpdatePlayData = false;
				m_CalculateMove = true;
			}
		}

		else
		{
			if(m_CalculateMove && m_Frames % 3 == 0)
			{
				const TetrisPiece currentPiece{ m_pView_CurrentPiece->GetCurrentPiece() }, nextPiece{ m_pView_NextPiece->GetNextPiece() };
				m_CurrentMove = SystemUtils::GetBestTetrisMove(m_pView_Playfield->GetPlayfield(), { m_PiecesData[currentPiece], m_PiecesData[nextPiece] });
				m_CalculateMove = false;
			}
			
			m_CanUpdatePlayData = CheckScore();

			// if(m_CurrentMove.valid)
			// {
			// 	if (m_CurrentMove.moveSet.empty())
			// 		SetKey(gbee::down, true);
			// 	
			// 	else if (UseKey(SystemUtils::TetrisMoveSetToKey(m_CurrentMove.moveSet[0]), 4))
			// 		m_CurrentMove.moveSet.pop_front();
			// 		
			// }
		}
	}

	m_Frames++;
	if (m_Frames == 60)
		m_Frames = 0;
}

void TetrisSystem::DrawSystemData() const
{
	EViewHolder::GetInstance().DrawViews();

	ImGui::Begin("SV - System Control", nullptr, ImGuiWindowFlags_NoCollapse);

	ImGui::Text("System Data");
	ImGui::Separator();
	ImGui::Text(std::string("Current Piece: " + SystemUtils::TetrisPieceToString(m_pView_CurrentPiece->GetCurrentPiece())).c_str());
	ImGui::Text(std::string("Next Piece: " + SystemUtils::TetrisPieceToString(m_pView_NextPiece->GetNextPiece())).c_str());

	ImGui::Spacing();

	ImGui::Text("Screen Checking");
	ImGui::Separator();
	ImGui::Text(std::string("Current Screen: " + SystemUtils::TetrisMenuToString(m_CurrentMenu)).c_str());
	ImGui::Text(std::string("Score Pos: " + m_ScoreStart.ToString() + " ==> " + m_ScoreEnd.ToString()).c_str());
	ImGui::Text(std::string("Game Menu Pos: " + m_GameMenuStart.ToString() + " ==> " + m_GameMenuEnd.ToString()).c_str());
	ImGui::Text(std::string("Game Over Pos: " + m_GameOverStart.ToString() + " ==> " + m_GameOverEnd.ToString()).c_str());

	ImGui::Spacing();

	ImGui::Text("Block Checking");
	ImGui::Separator();
	ImGui::Text(std::string("Corner Checks: " + std::to_string(CORNERS)).c_str());
	ImGui::Text(std::string("Req Corner: " + std::to_string(REQ_CORNERS)).c_str());

	ImGui::End();
}

TetrisMenu TetrisSystem::CheckMenu() const
{
	const auto R1 = SystemUtils::GetPixels({ 0, 0 }, { GAMEBOY_SCREEN_X - 1, 0 }, GetPixelBuffer());
	const auto R2 = SystemUtils::GetPixels({ 0, 1 }, { GAMEBOY_SCREEN_X - 1, 1 }, GetPixelBuffer());

	const int nrBlackR1 = SystemUtils::CountVector2D<uint8_t>(R1, COLOR_BLACK);
	const int nrWhiteR1 = SystemUtils::CountVector2D<uint8_t>(R1, COLOR_WHITE);

	// START - Only start is full black on top row
	if (nrBlackR1 == GAMEBOY_SCREEN_X)
		return TetrisMenu::START;

	// CREDITS - GAME MODE SELECT - LEVEL SELECT
	if (nrWhiteR1 == GAMEBOY_SCREEN_X)
	{
		if (SystemUtils::CountVector2D<uint8_t>(GetPixelBuffer(), COLOR_LIGHTGREY) == 0)
			return TetrisMenu::CREDITS;

		const auto gamemodePart = SystemUtils::GetPixels(m_GameMenuStart, m_GameMenuEnd, GetPixelBuffer());
		if (SystemUtils::CountVector2D<uint8_t>(gamemodePart, COLOR_LIGHTGREY) == 0)
			return TetrisMenu::LEVEL_SELECT;

		return TetrisMenu::GAME_SELECT;
	}

	const int columns{ 10 };
	if (nrWhiteR1 >= columns * BLOCK_SIZE)
	{
		const auto gameOverPart = SystemUtils::GetPixels(m_GameOverStart, m_GameOverEnd, GetPixelBuffer());
		if (SystemUtils::CountVector2D<uint8_t>(gameOverPart, COLOR_BLACK) == 0)
			return TetrisMenu::GAME_OVER;

		return TetrisMenu::PLAY;
	}

	return TetrisMenu::GAME_OVER; // If none of the above are applicable, then there is one case left of the playfield going game over (blocks come from bottom to top)

}

bool TetrisSystem::CheckScore()
{
	const auto currentScoreBuffer = SystemUtils::GetPixels(m_ScoreStart, m_ScoreEnd, GetPixelBuffer());
	if (m_ScoreZoneBuffer != currentScoreBuffer)
	{
		m_ScoreZoneBuffer = currentScoreBuffer;
		return true;
	}

	return false;
}

bool TetrisSystem::UseKey(const gbee::Key& key, int framesDelay) const
{
	const bool value{ m_Frames % framesDelay == 0 };
	m_Emulator.SetKeyState(key, value, 0);
	return value;
}

void TetrisSystem::SetKey(const gbee::Key& key, bool value) const
{
	m_Emulator.SetKeyState(key, value, 0);
}

void TetrisSystem::ResetKeys() const
{
	SetKey(gbee::aButton, false);
	SetKey(gbee::bButton, false);
	SetKey(gbee::start, false);
	SetKey(gbee::select, false);
	SetKey(gbee::left, false);
	SetKey(gbee::right, false);
	SetKey(gbee::up, false);
	SetKey(gbee::down, false);
}


// Initializations of all Piece Rotation Data

void TetrisSystem::Initialize_PieceO()
{
	TetrisPieceData data;

	const TetrisBlocksContainer blocks = 
	{
		{ true, true }, // ■ ■
		{ true, true }	// ■ ■
	};
	
	data.push_back({ 4, 4, blocks });
	
	m_PiecesData[TetrisPiece::O_PIECE] = data;
}
	
void TetrisSystem::Initialize_PieceI()
{
	TetrisPieceData data;

	TetrisBlocksContainer blocks =
	{
		{ true,  true,  true, true } // ■ ■ ■ ■
	};

	data.push_back({ 3, 3, SystemUtils::Transpose(blocks) });

	blocks =
	{
		{ true },	// ■
		{ true },	// ■
		{ true },	// ■
		{ true }	// ■
	};
	data.push_back({ 4, 5, SystemUtils::Transpose(blocks) });

	m_PiecesData[TetrisPiece::I_PIECE] = data;
}

void TetrisSystem::Initialize_PieceS()
{
	TetrisPieceData data;

	TetrisBlocksContainer blocks =
	{
		{ false, true, true  }, //   ■ ■
		{ true,  true, false }	// ■ ■
	};

	data.push_back({ 3, 4, SystemUtils::Transpose(blocks) });

	blocks =
	{
		{ true,  false },	// ■
		{ true,  true  },	// ■ ■
		{ false, true  }	//   ■
	};
	data.push_back({ 3, 5, SystemUtils::Transpose(blocks) });

	m_PiecesData[TetrisPiece::S_PIECE] = data;
}

void TetrisSystem::Initialize_PieceZ()
{
	TetrisPieceData data;

	TetrisBlocksContainer blocks =
	{
		{ true,  true, false }, // ■ ■
		{ false, true, true  }	//   ■ ■
	};

	data.push_back({ 3, 4, SystemUtils::Transpose(blocks) });

	blocks =
	{
		{ false, true  },	//   ■
		{ true,  true  },	// ■ ■
		{ true,  false }	// ■
	};
	data.push_back({ 3, 5, SystemUtils::Transpose(blocks) });

	m_PiecesData[TetrisPiece::Z_PIECE] = data;
}

void TetrisSystem::Initialize_PieceL()
{
	TetrisPieceData data;

	TetrisBlocksContainer blocks =
	{
		{ true,  true,  true }, // ■ ■ ■
		{ true, false, false }  // ■
	};
	data.push_back({ 3, 4, SystemUtils::Transpose(blocks) });

	blocks =
	{
		{ true, false  },		// ■
		{ true, false  },		// ■
		{ true, true   }		// ■ ■
	};
	data.push_back({ 4, 4, SystemUtils::Transpose(blocks) });

	blocks =
	{
		{ false, false, true }, //     ■
		{ true, true,   true }	// ■ ■ ■
	};

	data.push_back({ 3, 4, SystemUtils::Transpose(blocks) });

	blocks =
	{
		{ true,  true },		// ■ ■
		{ false, true },		//   ■
		{ false, true }			//   ■
	};
	data.push_back({ 3, 5, SystemUtils::Transpose(blocks) });

	m_PiecesData[TetrisPiece::L_PIECE] = data;
}

void TetrisSystem::Initialize_PieceJ()
{
	TetrisPieceData data;
	
	TetrisBlocksContainer blocks =
	{
		{ true,  true,  true }, // ■ ■ ■
		{ false, false, true }  //     ■
	};
	data.push_back({ 3, 4, SystemUtils::Transpose(blocks) });

	blocks =
	{
		{ true, true  },		// ■ ■
		{ true, false },		// ■
		{ true, false }			// ■
	};
	data.push_back({ 4, 4, SystemUtils::Transpose(blocks) });

	blocks =
	{
		{ true, false, false }, // ■
		{ true, true,  true  }	// ■ ■ ■
	};

	data.push_back({ 3, 4, SystemUtils::Transpose(blocks) });

	blocks =
	{
		{ false, true },		//   ■
		{ false, true },		//   ■
		{ true,  true }			// ■ ■
	};
	data.push_back({ 3, 5, SystemUtils::Transpose(blocks) });

	m_PiecesData[TetrisPiece::J_PIECE] = data;
}

void TetrisSystem::Initialize_PieceT()
{
	TetrisPieceData data;
	
	TetrisBlocksContainer blocks =
	{
		{ true,  true, true  }, // ■ ■ ■
		{ false, true, false }	//   ■
	};

	data.push_back({ 3, 4, SystemUtils::Transpose(blocks) });

	blocks =
	{
		{ true, false },	// ■
		{ true, true  },	// ■ ■
		{ true, false }		// ■
	};
	data.push_back({ 4, 4, SystemUtils::Transpose(blocks) });
	
	blocks =
	{
		{ false, true, false },	//   ■
		{ true,  true, true  }	// ■ ■ ■
	};

	data.push_back({ 3, 4, SystemUtils::Transpose(blocks) });

	blocks =
	{
		{ false, true },	//   ■
		{ true,  true },	// ■ ■
		{ false, true }		//   ■
	};
	data.push_back({ 3, 5, SystemUtils::Transpose(blocks) });

	m_PiecesData[TetrisPiece::T_PIECE] = data;
}