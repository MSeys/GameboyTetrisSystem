#include "Renderer.h"

#include <stdexcept>


#include "imgui/imgui_sdl.h"

void Renderer::Init(SDL_Window* window, const vec2& windowSize)
{
	m_pRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);

	if (!m_pRenderer)
		throw std::runtime_error(std::string("SDL_CreateRenderer Error: ") + SDL_GetError());

	ImGui::CreateContext();
	ImGuiSDL::Initialize(m_pRenderer, int(windowSize.x), int(windowSize.y));
}

void Renderer::Destroy()
{
	if (m_pRenderer != nullptr)
	{
		ImGuiSDL::Deinitialize();
		SDL_DestroyRenderer(m_pRenderer);
		m_pRenderer = nullptr;

		ImGui::DestroyContext();
	}
}

void Renderer::RenderLineRect(const rectf& rect, const SDL_Color& color) const
{
	SDL_Rect sdlRect{ int(rect.x), int(rect.y), int(rect.w), int(rect.h) };
	SDL_SetRenderDrawColor(GetSDLRenderer(), color.r, color.g, color.b, color.a);
	SDL_RenderDrawRect(GetSDLRenderer(), &sdlRect);
	SDL_SetRenderDrawColor(GetSDLRenderer(), 0, 0, 0, 255);
}

void Renderer::RenderLine(const vec2& pointA, const vec2& pointB, const SDL_Color& color) const
{
	SDL_SetRenderDrawColor(GetSDLRenderer(), color.r, color.g, color.b, color.a);
	SDL_RenderDrawLine(GetSDLRenderer(), int(pointA.x), int(pointA.y), int(pointB.x), int(pointB.y));
	SDL_SetRenderDrawColor(GetSDLRenderer(), 0, 0, 0, 255);
}

void Renderer::RenderFilledRect(const rectf& rect, const SDL_Color& color) const
{
	SDL_Rect sdlRect{ int(rect.x), int(rect.y), int(rect.w), int(rect.h) };
	SDL_SetRenderDrawColor(GetSDLRenderer(), color.r, color.g, color.b, color.a);
	SDL_RenderFillRect(GetSDLRenderer(), &sdlRect);
	SDL_SetRenderDrawColor(GetSDLRenderer(), 0, 0, 0, 255);
}

void Renderer::DrawTetrisBlocks(const TetrisBlocksContainer& blocks, const SDL_Color& color) const
{
	const int columns{ int(blocks.size()) }, rows{ int(blocks[0].size()) };

	for (int col{}; col < columns; col++)
	{
		for (int row{}; row < rows; row++)
		{
			if (blocks[col][row])
			{
				RenderFilledRect(
					{ float(col * BLOCK_SIZE), float(row * BLOCK_SIZE), float(BLOCK_SIZE), float(BLOCK_SIZE) },
					color);
			}
		}
	}
}
