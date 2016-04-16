#include "Game/StateMachine.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

static GameState m_state = STARTUP;

bool SetGameState(GameState newState)
{
	if (m_state != newState)
	{
		DebuggerPrintf("Changed State from %s to %s", GetStateString(m_state), GetStateString(newState));
		m_state = newState;
		return true;
	}
	else
	{
		ERROR_RECOVERABLE("State machine was set to the same state.");
		return false;
	}
}

GameState GetGameState()
{
	return m_state;
}

const char* GetStateString(GameState state)
{
	switch (state)
	{
	case STARTUP:
		return "Startup";
	case MAIN_MENU:
		return  "Main Menu";
	case MAP_SELECTION:
		return "Map Selection";
	case GENERATION:
		return "Generation";
	case PLAYING:
		return "Playing";
	case PAUSED:
		return "Paused";
	case SHUTDOWN:
		return "Shutdown";
	case NUM_STATES:
		return "Num States (Bruh you have a bug)";
	default:
		return "Default (Bruh you have a bug)";
	}
}
