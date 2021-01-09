#include <iostream>
#include "SDL.h"
#include "imgui/imgui.h"
#include "imgui/imgui_sdl.h"
#include "../GameboyEmulator/EmulatorClean.h"
#include "TetrisSystem.h"

#define INSTANCECOUNT 1

gbee::Emulator emu{"Tetris(JUE)(V1.1)[!].gb", INSTANCECOUNT};

int main( int argc, char *argv[] ) {
	srand(int(time(nullptr)));
	
	emu.Start();
	
	TetrisSystem system{ emu };
	system.Initialize();
	system.Run();
	system.Quit();
	
	return 0;
}