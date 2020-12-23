#pragma once
#include "../GameboyEmulator/EmulatorClean.h"
#include "SDL.h"
#include "imgui/imgui.h"
#include "imgui/imgui_sdl.h"

#include "SystemUtils.h"

constexpr auto BLOCK_SIZE = 8;
constexpr auto OTHER_SIDE = BLOCK_SIZE - 1;
constexpr auto GAMEBOY_SCREEN_X = 160;
constexpr auto GAMEBOY_SCREEN_Y = 144;
constexpr auto CORNERS = 4;
constexpr auto REQ_CORNERS = 2;
constexpr auto PIECES = 7;
constexpr uint8_t COLOR_BLACK = 3;