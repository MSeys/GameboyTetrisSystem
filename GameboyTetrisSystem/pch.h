#pragma once
#include "../GameboyEmulator/EmulatorClean.h"
#include "SDL.h"
#include "imgui/imgui.h"
#include "imgui/imgui_sdl.h"

#include "SystemUtils.h"

// Gameboy Emulator
constexpr uint8_t COLOR_WHITE = 0;
constexpr uint8_t COLOR_LIGHTGREY = 1;
constexpr uint8_t COLOR_DARKGREY = 2;
constexpr uint8_t COLOR_BLACK = 3;
constexpr auto GAMEBOY_SCREEN_X = 160;
constexpr auto GAMEBOY_SCREEN_Y = 144;

// Tetris
constexpr auto PIECES = 7;
constexpr auto TETRIS_COLUMNS = 10;
constexpr auto TETRIS_ROWS = 18;
constexpr auto BLOCK_SIZE = 8;
constexpr auto OTHER_SIDE = BLOCK_SIZE - 1;

constexpr auto PLAYFIELD_SIZE_X = TETRIS_COLUMNS * BLOCK_SIZE;
constexpr auto PLAYFIELD_SIZE_Y = (TETRIS_ROWS - 1) * BLOCK_SIZE;

// Block recognition
constexpr auto CORNERS = 4;
constexpr auto REQ_CORNERS = 2;

// Algorithm Values
constexpr auto GA_ParamA = -0.510066;
constexpr auto GA_ParamB = 0.760666;
constexpr auto GA_ParamC = -0.35663;
constexpr auto GA_ParamD = -0.184483;