#include "Game/Generators/HiveGenerator.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/AABB2.hpp"

GeneratorRegistration HiveGenerator::s_hiveGeneratorRegistration("Hive", &HiveGenerator::CreateGenerator, &Generator::CreateGenerationProcessData);

HiveGenerator::HiveGenerator(const std::string& name)
{
	m_name = name;
}

HiveGenerator::~HiveGenerator()
{

}

void HiveGenerator::InitializeMap(Map* map)
{
	m_nextFeelers = new std::vector<Feeler>();
	map->AttemptSetRectangleOfCells(AABB2(Vector2Int::ONE, map->m_size - Vector2Int::ONE), Cell::AIR); 
	Cell* randomCell = &map->GetRandomCell();
	while (randomCell->m_type != Cell::AIR)
	{
		randomCell = &map->GetRandomCell();
	}
	SpawnFeelersAt(randomCell->m_position);
}

bool HiveGenerator::GenerateStep(int& currentStepNumber, GenerationProcessData* data, Map* map /*= nullptr*/)
{
	m_currentFeelers = m_nextFeelers;
	m_nextFeelers = new std::vector<Feeler>();
	static int numSteps = data->m_rangeSteps.x - data->m_rangeSteps.y == 0 ? data->m_rangeSteps.x : MathUtils::GetRandom(data->m_rangeSteps.x, data->m_rangeSteps.y);
	bool isFinished = true;
	for (Feeler& feeler : (*m_currentFeelers))
	{
		if (!feeler.TakeStep(map, data, this))
		{
			isFinished = false;
			m_nextFeelers->push_back(feeler);
		}
	}
	delete m_currentFeelers;
	return isFinished || (++currentStepNumber > numSteps);
}

void HiveGenerator::SpawnFeelersAt(const Vector2Int& location)
{
	m_nextFeelers->push_back(Feeler(location, Vector2Int::ONE, Feeler::FeelerState::UP2));
	m_nextFeelers->push_back(Feeler(location, -Vector2Int::ONE, Feeler::FeelerState::DOWN2));
	m_nextFeelers->push_back(Feeler(location, Vector2Int(-1, 1), Feeler::FeelerState::UP2));
	m_nextFeelers->push_back(Feeler(location, Vector2Int(1, -1), Feeler::FeelerState::DOWN2));
}

bool HiveGenerator::Feeler::TakeStep(Map* map, GenerationProcessData* data, HiveGenerator* generator)
{
	//TODO: Grab this from the data file:
	const int SIDE_LENGTH = 3;
	if (isFinished)
	{
		return true;
	}
	MapRaycastCollision collision = map->Raycast(currentPosition, currentDirection, SIDE_LENGTH, Cell::STONE);
	isFinished = collision.collided;
	for (int i = 0; i < collision.numCellsTraveled; ++i)
	{
		currentPosition += currentDirection;
		if ((i + 1 == SIDE_LENGTH) && (stateNumber == FeelerState::UP || stateNumber == FeelerState::DOWN || stateNumber == FeelerState::UP2 || stateNumber == FeelerState::DOWN2))
		{
			generator->SpawnFeelersAt(currentPosition);
		}
		else
		{
			map->AttemptSetCellType(currentPosition, Cell::STONE);
		}
	}
	IncrementState(generator);
	return isFinished;
}

void HiveGenerator::Feeler::IncrementState(HiveGenerator* generator)
{
	stateNumber = (HiveGenerator::Feeler::FeelerState)((int)stateNumber + 1);
	if (stateNumber == FeelerState::NUM_STATES)
	{
		stateNumber = UP;
	}
	switch (stateNumber)
	{
	case UP:
		currentDirection.y = 1;
		break;
	case UP2:
		currentDirection.y = 1;
		break;
	case CENTER:
		currentDirection.y = 0;
		break;
	case DOWN:
		currentDirection.y = -1;
		break;
	case DOWN2:
		currentDirection.y = -1;
		break;
	case CENTER2:
		currentDirection.y = 0;
		break;
	default:
		break;
	}
}
