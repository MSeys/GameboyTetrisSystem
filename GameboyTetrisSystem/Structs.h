#pragma once
struct vec2
{
	float x;
	float y;

	vec2(float x = 0, float y = 0) : x(x), y(y) {}
};

struct ivec2
{
	int x;
	int y;

	ivec2(int x = 0, int y = 0) : x(x), y(y) {}
};

struct rectf
{
	float x;
	float y;
	float w;
	float h;

	rectf(float x = 0, float y = 0, float w = 0, float h = 0) : x(x), y(y), w(w), h(h) {}
};