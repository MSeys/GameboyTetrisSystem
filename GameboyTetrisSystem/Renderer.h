#pragma once

#include "pch.h"
#include "Singleton.h"

struct SDL_Window;
struct SDL_Renderer;

// Adjusted class from https://matthiasseys.me/Projects/protoengine.html

class Renderer final : public Singleton<Renderer>
{
public:
	void Init(SDL_Window* window, const vec2& windowSize);
	void Destroy();

	void RenderLineRect(const rectf& rect, const SDL_Color& color) const;
	void RenderLine(const vec2& pointA, const vec2& pointB, const SDL_Color& color) const;
	void RenderFilledRect(const rectf& rect, const SDL_Color& color) const;

	void DrawTetrisBlocks(const TetrisBlocksContainer& blocks, const SDL_Color& color = { 255, 255, 255, 255 }) const;
	
	SDL_Renderer* GetSDLRenderer() const { return m_pRenderer; }

private:
	SDL_Renderer* m_pRenderer{};
};

