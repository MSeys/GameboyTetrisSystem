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