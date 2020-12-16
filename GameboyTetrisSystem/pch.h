#pragma once
#include "../GameboyEmulator/EmulatorClean.h"
#include "SDL.h"
#include "imgui/imgui.h"
#include "imgui/imgui_sdl.h"

#include "Structs.h"
#include "TetrisSystem.h"

template<class T>
void SafeDelete(T& pObjectToDelete)
{
	if (pObjectToDelete != nullptr)
	{
		delete(pObjectToDelete);
		pObjectToDelete = nullptr;
	}
}
template<class T>
T Clamp(T min, T max, T value)
{
	if (value < min) return min;
	if (value > max) return max;

	return value;
}