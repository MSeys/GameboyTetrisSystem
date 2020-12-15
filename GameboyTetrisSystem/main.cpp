#include <iostream>
#include "imgui/SDL2-2.0.10/include/SDL.h"
#include "imgui/imgui.h"
#include "imgui/imgui_sdl.h"
#include "../GameboyEmulator/EmulatorClean.h"
#include "TetrisSystem.h"

/*
	This is just a proof of concept, a quick example as to how you'd use the library
*/

#define INSTANCECOUNT 1

gbee::Emulator emu{"Tetris(JUE)(V1.1)[!].gb", INSTANCECOUNT};

int main( int argc, char *argv[] ) {
	TetrisSystem system{ emu };
	system.Initialize();
	system.Run();
	system.Quit();
	
	return 0;
}