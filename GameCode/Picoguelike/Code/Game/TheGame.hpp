#pragma once
#include <vector>
#include <map>
#include "Engine/Math/Vector2Int.hpp"

class Map;
class Generator;
class EnvironmentBlueprint;
class Agent;
class Player;
class MeshRenderer;
class TextBox;

typedef std::multimap <float, Agent*> TurnOrderMap; //Multimap gets re-sorted whenever the float gets updated.
typedef std::pair <float, Agent*> TurnOrderMapPair; //Multimap gets re-sorted whenever the float gets updated.

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

    //FUNCTIONS//////////////////////////////////////////////////////////////////////////
    void SpawnInGame(Agent* enemyPointer, const Vector2Int& spawnPosition = Vector2Int(-1, -1));
    void UpdatePlayerInput();
    void CleanUpDeadEntities();

    //MEMBER VARIABLES//////////////////////////////////////////////////////////////////////////
    static TheGame* instance;
    Map* m_currentMap;
    Generator* m_currentGenerator;
    EnvironmentBlueprint* m_currentEnvironment;
    std::vector<Agent*> m_entities;
    Player* m_player;

private:
    bool m_autoGenerate;
    MeshRenderer* m_mainMenuText;
    std::vector<TextBox*> m_textData;
    float simulationClock = 0.0f;
    float simulationDelta = 0.5f;
    TurnOrderMap m_agentsWaitingForTurn;
};