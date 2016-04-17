#include "Game/TheGame.hpp"
#include "Engine/Renderer/RGBA.hpp"
#include "Engine/Renderer/AABB2.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/MeshRenderer.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Game/Map/Map.hpp"
#include "Game/StateMachine.hpp"
#include "Game/Entities/Agent.hpp"
#include "Game/Entities/Entity.hpp"
#include "Game/Entities/Player.hpp"
#include "Game/Generators/Generator.hpp"
#include "Game/Environments/EnvironmentBlueprint.hpp"

TheGame* TheGame::instance = nullptr;
extern bool g_isQuitting;
bool g_renderDebug = false;

//-----------------------------------------------------------------------------------
TheGame::TheGame()
	: m_currentMap(nullptr)
	, m_currentGenerator(GeneratorRegistration::CreateGeneratorByName("CellularAutomata"))
	, m_autoGenerate(true)
	, m_player(nullptr)
	, m_mainMenuText(nullptr)
{
	SetGameState(GameState::MAIN_MENU);
}

//-----------------------------------------------------------------------------------
TheGame::~TheGame()
{
	SetGameState(GameState::SHUTDOWN);
}

//-----------------------------------------------------------------------------------
void TheGame::Update(float deltaSeconds)
{
	if (InputSystem::instance->WasKeyJustPressed('D'))
	{
		g_renderDebug = !g_renderDebug;
	}
	if(InputSystem::instance->WasKeyJustPressed('1'))
	{
		SetGameState(GameState::MAIN_MENU);
	}
	else if (InputSystem::instance->WasKeyJustPressed('2'))
	{
		SetGameState(GameState::MAP_SELECTION);
	}
	else if (InputSystem::instance->WasKeyJustPressed('3'))
	{
		SetGameState(GameState::GENERATION);
	}
	else if (InputSystem::instance->WasKeyJustPressed('4'))
	{
		SetGameState(GameState::PLAYING);
	}
	else if (InputSystem::instance->WasKeyJustPressed('5'))
	{
		SetGameState(GameState::PAUSED);
	}

	switch(GetGameState())
	{
	case GameState::MAIN_MENU:
		UpdateMainMenu(deltaSeconds);
		break;
	case GameState::MAP_SELECTION:
		EnvironmentBlueprint::LoadEnvironmentBlueprints();
		UpdateMapSelection(deltaSeconds);
		break;
	case GameState::GENERATION:
		UpdateGeneration(deltaSeconds);
		break;
	case GameState::PLAYING:
		if (m_player == nullptr)
		{
			m_player = new Player();
			m_entities.push_back(m_player);
			m_player->Spawn(m_currentMap);
			m_currentMap->UpdateFOVFrom(m_player->m_position, m_player->m_viewDistance);
		}
		UpdatePlaying(deltaSeconds);
		break;
	case GameState::PAUSED:
		UpdatePaused(deltaSeconds);
		break;
	}
}

//-----------------------------------------------------------------------------------
void TheGame::UpdateMainMenu(float deltaSeconds)
{
	UNUSED(deltaSeconds);
	if (InputSystem::instance->WasKeyJustPressed('N'))
	{
		SetGameState(GameState::MAP_SELECTION);
	}
	else if (InputSystem::instance->WasKeyJustPressed('Q'))
	{
		g_isQuitting = true;
	}
}

//-----------------------------------------------------------------------------------
void TheGame::UpdateMapSelection(float deltaSeconds)
{
	UNUSED(deltaSeconds);
	if (InputSystem::instance->WasKeyJustPressed(InputSystem::ExtraKeys::ESC))
	{
		SetGameState(GameState::MAIN_MENU);
	}
	if (InputSystem::instance->WasKeyJustPressed(InputSystem::ExtraKeys::TAB))
	{
		m_autoGenerate = !m_autoGenerate;
	}
	char indexCharacter = 'A';
	for (auto iterator = EnvironmentBlueprint::s_envirnomentBlueprints.begin(); iterator != EnvironmentBlueprint::s_envirnomentBlueprints.end(); ++iterator)
	{
		if (InputSystem::instance->IsKeyDown(indexCharacter++))
		{
			EnvironmentBlueprint* environmentBlueprint = iterator->second;
			GenerationProcessData* genProcessData = environmentBlueprint->m_generationProcesses.at(0);
			m_currentGenerator = GeneratorRegistration::CreateGeneratorByName(genProcessData->m_generatorName);
			m_currentMap = new Map(environmentBlueprint->m_size);
			m_currentGenerator->InitializeMap(m_currentMap);
			m_currentEnvironment = iterator->second;
			SetGameState(GameState::GENERATION);
			return;
		}
	}

}

//-----------------------------------------------------------------------------------
void TheGame::UpdateGeneration(float deltaSeconds)
{
	UNUSED(deltaSeconds);
	static int currentStep = 0;
	static int currentGeneratorIndex = 0;
	
	int maxGeneratorSteps = m_currentEnvironment->m_generationProcesses.size();
	bool isMapReady = false;
	if (m_autoGenerate)
	{
		while (!isMapReady) 
		{
			isMapReady = m_currentGenerator->GenerateStep(currentStep, m_currentEnvironment->m_generationProcesses.at(currentGeneratorIndex), m_currentMap);
			if (isMapReady && currentGeneratorIndex < (maxGeneratorSteps - 1))
			{
				m_currentGenerator = GeneratorRegistration::CreateGeneratorByName(m_currentEnvironment->m_generationProcesses.at(++currentGeneratorIndex)->m_generatorName);
				isMapReady = false;
				currentStep = 0;
			}
		}
	}
	else if (InputSystem::instance->WasKeyJustPressed(' ') || InputSystem::instance->IsKeyDown('G'))
	{
		isMapReady = m_currentGenerator->GenerateStep(currentStep, m_currentEnvironment->m_generationProcesses.at(currentGeneratorIndex), m_currentMap);
		if (isMapReady && currentGeneratorIndex < (maxGeneratorSteps - 1))
		{
			m_currentGenerator = GeneratorRegistration::CreateGeneratorByName(m_currentEnvironment->m_generationProcesses.at(++currentGeneratorIndex)->m_generatorName);
			isMapReady = false;
			currentStep = 0;
		}
	}

	if (InputSystem::instance->WasKeyJustPressed(InputSystem::ExtraKeys::ESC))
	{
		currentStep = 0;
		currentGeneratorIndex = 0;
		SetGameState(GameState::MAP_SELECTION);
	}

	if (InputSystem::instance->WasKeyJustPressed(InputSystem::ExtraKeys::ENTER))
	{
		m_autoGenerate = true;
	}

	if (isMapReady)
	{
		currentStep = 0;
		currentGeneratorIndex = 0;
		Generator::FinalizeMap(m_currentMap);
		SetGameState(GameState::PLAYING);
	}
}

//-----------------------------------------------------------------------------------
void TheGame::UpdatePlaying(float deltaSeconds)
{
	UNUSED(deltaSeconds);
	UpdatePlayerInput();
	for (Entity* entity : m_entities)
	{
		entity->Update(deltaSeconds);
	}
	if (InputSystem::instance->WasKeyJustPressed(InputSystem::ExtraKeys::ESC))
	{
		SetGameState(GameState::PAUSED);
	}
}

//-----------------------------------------------------------------------------------
void TheGame::UpdatePaused(float deltaSeconds)
{
	UNUSED(deltaSeconds);
	if (InputSystem::instance->WasKeyJustPressed(InputSystem::ExtraKeys::ESC))
	{
		SetGameState(GameState::PLAYING);
	}
}

//-----------------------------------------------------------------------------------
void TheGame::Render()
{
	Renderer::instance->ClearScreen(0.3f, 0.3f, 0.3f);
	Renderer::instance->BeginOrtho(Vector2::ZERO, Vector2(1600, 900));
	Renderer::instance->DrawText2D(Vector2(0, 0), GetStateString(GetGameState()), 5.0f, RGBA::VAPORWAVE, true, BitmapFont::CreateOrGetFontFromGlyphSheet("Runescape"));
	switch (GetGameState())
	{
	case GENERATION:
		Renderer::instance->ClearScreen(RGBA::BLACK);
		RenderGeneration();
		break;
	case MAIN_MENU:
		Renderer::instance->ClearScreen(RGBA::CORNFLOWER_BLUE);
		RenderMainMenu();
		break;
	case GameState::MAP_SELECTION:
		Renderer::instance->ClearScreen(RGBA::FOREST_GREEN);
		RenderMapSelection();
		break;
	case GameState::PLAYING:
		Renderer::instance->ClearScreen(0.3f, 0.3f, 0.3f);
		RenderPlaying();
		break;
	case GameState::PAUSED:
		Renderer::instance->ClearScreen(RGBA::CORNFLOWER_BLUE);
		RenderPaused();
		break;
	}
	Renderer::instance->EndOrtho();
}

//-----------------------------------------------------------------------------------
void TheGame::RenderMainMenu()
{
	if (m_mainMenuText == nullptr)
	{
		MeshBuilder builder;
		BitmapFont* bmFont = BitmapFont::CreateOrGetFontFromGlyphSheet("Runescape");
		builder.AddStringEffectFragment("Picougelike", bmFont, 7.0f, 5.0f, Vector3(450.0f, 600.0f, 0.0f), Vector3::UP, Vector3::RIGHT,);
		builder.AddStringEffectFragment(Vector2(450, 300), "New Game (N)", 5.0f, RGBA::CYAN, true, bmFont);
		builder.AddStringEffectFragment(Vector2(450, 150), "Quit (Q)", 5.0f, RGBA::CYAN, true, bmFont);
		m_mainMenuText = new MeshRenderer(new Mesh(), bmFont->GetMaterial());
		builder.CopyToMesh(m_mainMenuText->m_mesh, Vertex_TextPCT::Copy, sizeof(Vertex_TextPCT), Vertex_TextPCT::BindMeshToVAO);
	}
	m_mainMenuText->Render();
}

//-----------------------------------------------------------------------------------
void TheGame::RenderMapSelection()
{
	Vector2 offset = Vector2(0.0f, 750.0f);
	Renderer::instance->DrawText2D(offset, "Available Quests:", 3.0f, RGBA::VAPORWAVE, true, BitmapFont::CreateOrGetFontFromGlyphSheet("Runescape"));
	char indexCharacter = 'A';
	for (auto iterator = EnvironmentBlueprint::s_envirnomentBlueprints.begin(); iterator != EnvironmentBlueprint::s_envirnomentBlueprints.end(); ++iterator)
	{
		offset += Vector2(0, -120);
		std::string str = iterator->first;
		Renderer::instance->DrawText2D(offset, Stringf("%c) %s", indexCharacter++, str.c_str()), 3.0f, RGBA::VAPORWAVE, true, BitmapFont::CreateOrGetFontFromGlyphSheet("Runescape"));
	}
	if (m_autoGenerate)
	{
		Renderer::instance->DrawText2D(Vector2(1000, 0.0f), "Auto Generate", 3.0f, RGBA::SEA_GREEN, true, BitmapFont::CreateOrGetFontFromGlyphSheet("Runescape"));
	}
	else
	{
		Renderer::instance->DrawText2D(Vector2(1000, 0.0f), "Manual Generate", 3.0f, RGBA::RED, true, BitmapFont::CreateOrGetFontFromGlyphSheet("Runescape"));
	}
}

//-----------------------------------------------------------------------------------
void TheGame::RenderGeneration()
{
	if (m_autoGenerate)
	{
		Renderer::instance->ClearScreen(RGBA::BLACK);
		Renderer::instance->DrawText2D(Vector2(50, 250), "Generating...", 10.0f, RGBA::TURQUOISE, true, BitmapFont::CreateOrGetFontFromGlyphSheet("Runescape"));
	}
	else
	{
		m_currentMap->Render();
	}
}

//-----------------------------------------------------------------------------------
void TheGame::RenderPlaying()
{
	m_currentMap->Render();
	for (Entity* entity : m_entities)
	{
		entity->Render();
	}
}

//-----------------------------------------------------------------------------------
void TheGame::RenderPaused()
{
	RenderPlaying();
	Renderer::instance->DrawAABB(AABB2(Vector2::ZERO, Vector2(1600.0f, 900.0f)), RGBA(0x00000033));
	Renderer::instance->DrawText2D(Vector2(350, 250), "Paused", 10.0f, RGBA::VAPORWAVE, true, BitmapFont::CreateOrGetFontFromGlyphSheet("Runescape"));
}

//-----------------------------------------------------------------------------------
void TheGame::UpdatePlayerInput()
{
	if (InputSystem::instance->WasKeyJustPressed('H'))
	{
		m_player->QueueMove(Player::Direction::WEST);
	}
	else if (InputSystem::instance->WasKeyJustPressed('J'))
	{
		m_player->QueueMove(Player::Direction::SOUTH);
	}
	else if (InputSystem::instance->WasKeyJustPressed('K'))
	{
		m_player->QueueMove(Player::Direction::NORTH);
	}
	else if (InputSystem::instance->WasKeyJustPressed('L'))
	{
		m_player->QueueMove(Player::Direction::EAST);
	}
	else if (InputSystem::instance->WasKeyJustPressed('Y'))
	{
		m_player->QueueMove(Player::Direction::NORTH_WEST);
	}
	else if (InputSystem::instance->WasKeyJustPressed('U'))
	{
		m_player->QueueMove(Player::Direction::NORTH_EAST);
	}
	else if (InputSystem::instance->WasKeyJustPressed('B'))
	{
		m_player->QueueMove(Player::Direction::SOUTH_WEST);
	}
	else if (InputSystem::instance->WasKeyJustPressed('N'))
	{
		m_player->QueueMove(Player::Direction::SOUTH_EAST);
	}
}
