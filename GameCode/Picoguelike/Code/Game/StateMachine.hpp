#pragma once

enum GameState
{
	STARTUP,
	MAIN_MENU,
	MAP_SELECTION,
	GENERATION,
	PLAYING,
	PAUSED,
	SHUTDOWN,
	NUM_STATES
};

bool SetGameState(GameState state);
GameState GetGameState();
const char* GetStateString(GameState state);




