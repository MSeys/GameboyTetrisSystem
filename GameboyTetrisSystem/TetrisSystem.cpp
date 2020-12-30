#include "TetrisSystem.h"

#include "Renderer.h"

#define GuiColor ImVec4{ 0, 118.f / 255.f, 210.f / 255.f, 1.f }

TetrisSystem::TetrisSystem(gbee::Emulator& emulator, const vec2& windowSize)
	: m_WindowSize(windowSize), m_Emulator(emulator), m_SystemControl()
{
	m_PixelBuffer.resize(GAMEBOY_SCREEN_X, std::vector<uint8_t>(GAMEBOY_SCREEN_Y));
}

void TetrisSystem::Initialize()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	
	m_pWindow = SDL_CreateWindow("TetrisSystem - Matthias Seys",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		int(m_WindowSize.x), int(m_WindowSize.y),
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	
	Renderer::GetInstance().Init(m_pWindow, m_WindowSize);
	
	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = 1.0f / m_FPS; //Runs a tick behind.. (Due to while loop condition)

	InitializeImGuiStyle();

	m_SystemControl.Initialize(this);

	InitializePieces();
}

void TetrisSystem::Run()
{
	while(!m_Exit)
	{
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			SetKeyState(e);
			ImGuiSDL::UpdateInput(&e);

			if (e.type == SDL_QUIT) 
				m_Exit = true;
		}

		ImGui::NewFrame();

		SDL_SetRenderDrawColor(Renderer::GetInstance().GetSDLRenderer(), 114, 144, 154, 255);
		SDL_RenderClear(Renderer::GetInstance().GetSDLRenderer());

		UpdatePixelBuffer();

		m_SystemControl.Update();
		m_SystemControl.DrawGUI();
		
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

void TetrisSystem::SetKeyState(const SDL_Event& event) const
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

void TetrisSystem::InitializeImGuiStyle()
{
	ImGuiStyle* style = &ImGui::GetStyle();
	ImVec4* colors = style->Colors;

	colors[ImGuiCol_FrameBg] = ImVec4(0.160f, 0.160f, 0.160f, 1.000f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.200f, 0.200f, 0.200f, 1.000f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.280f, 0.280f, 0.280f, 1.000f);
	colors[ImGuiCol_TitleBg] = GuiColor;
	colors[ImGuiCol_TitleBgActive] = GuiColor;
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
	style->FrameRounding = 0;
	style->GrabMinSize = 7.0f;
	style->PopupRounding = 2.0f;
	style->ScrollbarRounding = 12.0f;
	style->ScrollbarSize = 13.0f;
	style->TabBorderSize = 1.0f;
	style->TabRounding = 2.0f;
	style->WindowRounding = 0;
	style->WindowTitleAlign = { 0.5f, 0.5f };
}

void TetrisSystem::InitializePieces()
{
	Initialize_PieceO();
	Initialize_PieceI();
	Initialize_PieceS();
	Initialize_PieceZ();
	Initialize_PieceL();
	Initialize_PieceJ();
	Initialize_PieceT();
}

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