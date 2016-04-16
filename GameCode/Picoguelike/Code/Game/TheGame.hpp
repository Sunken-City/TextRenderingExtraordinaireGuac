#pragma once
#include <vector>

class Map;
class Generator;
class EnvironmentBlueprint;
class Agent;
class Player;

class TheGame
{
public:
	//CONSTRUCTORS//////////////////////////////////////////////////////////////////////////
	TheGame();
	~TheGame();

	//UPDATE FUNCTIONS//////////////////////////////////////////////////////////////////////////
	void Update(float deltaSeconds);
	void UpdateMainMenu(float deltaSeconds);
	void UpdateMapSelection(float deltaSeconds);
	void UpdateGeneration(float deltaSeconds);
	void UpdatePlaying(float deltaSeconds);
	void UpdatePaused(float deltaSeconds);

	//RENDER FUNCTIONS//////////////////////////////////////////////////////////////////////////
	void Render();
	void RenderMainMenu();
	void RenderMapSelection();
	void RenderGeneration();
	void RenderPlaying();
	void RenderPaused();
	void UpdatePlayerInput();

	//MEMBER VARIABLES//////////////////////////////////////////////////////////////////////////
	static TheGame* instance;
	Map* m_currentMap;
	Generator* m_currentGenerator;
	EnvironmentBlueprint* m_currentEnvironment;
	bool m_autoGenerate;
	std::vector<Agent*> m_entities;
	Player* m_player;
};