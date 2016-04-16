#include "Game/Generators/CellularAutomataGenerator.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Map/Cell.hpp"

GeneratorRegistration CellularAutomataGenerator::s_cellularAutomatonGeneratorRegistration("CellularAutomata", &CellularAutomataGenerator::CreateGenerator, &Generator::CreateGenerationProcessData);

//-----------------------------------------------------------------------------------
CellularAutomataGenerator::CellularAutomataGenerator(const std::string& name)
{
	m_name = name;
}

//-----------------------------------------------------------------------------------
CellularAutomataGenerator::~CellularAutomataGenerator()
{

}

//-----------------------------------------------------------------------------------
void CellularAutomataGenerator::InitializeMap(Map* map)
{
	const float AIR_PERCENTAGE = 0.6f;
	for (Cell& cell : map->m_cellGrid)
	{
		float randomNumber = MathUtils::GetRandomFromZeroTo(1.0f);
		if (randomNumber < AIR_PERCENTAGE)
		{
			cell.m_type = Cell::Type::AIR;
		}
		else
		{
			cell.m_type = Cell::Type::STONE;
		}
	}
}

//-----------------------------------------------------------------------------------
bool CellularAutomataGenerator::GenerateStep(int& currentStepNumber, GenerationProcessData* data, Map* map /*= nullptr*/)
{
	static int numSteps = data->m_rangeSteps.x - data->m_rangeSteps.y == 0 ? data->m_rangeSteps.x : MathUtils::GetRandom(data->m_rangeSteps.x, data->m_rangeSteps.y);

	if (currentStepNumber == 0)
	{
		InitializeMap(map);
		StoreInitialMapState(map);
	}

	//Calculate the changes for the next state.
	int currentIndex = 0;
	for (int y = 0; y < map->m_size.y; ++y)
	{
		for (int x = 0; x < map->m_size.x; ++x)
		{
			if (currentStepNumber < 5)
			{
				int numStoneInRadius1 = map->GetNumberOfCellsOfType(Vector2Int(x, y), Cell::STONE, 1);
				if (numStoneInRadius1 >= 5)
				{
					m_nextState[currentIndex] = Cell::Type::STONE;
				}
				int numStoneInRadius2 = map->GetNumberOfCellsOfType(Vector2Int(x, y), Cell::STONE, 1);
				if (numStoneInRadius2 <= 2)
				{
					m_nextState[currentIndex] = Cell::Type::AIR;
				}
			}
			else
			{
				int numStoneInRadius1 = map->GetNumberOfCellsOfType(Vector2Int(x, y), Cell::STONE, 1);
				if (numStoneInRadius1 >= 5)
				{
					m_nextState[currentIndex] = Cell::Type::STONE;
				}
				else
				{
					m_nextState[currentIndex] = Cell::Type::AIR;
				}
			}

			++currentIndex;
		}
	}
	
	//Make the proposed state changes.
	for (unsigned int i = 0; i < map->m_cellGrid.size(); ++i)
	{
		map->m_cellGrid[i].m_type = m_nextState[i];
	}

	return (++currentStepNumber > numSteps);
}

//-----------------------------------------------------------------------------------
void CellularAutomataGenerator::StoreInitialMapState(Map* map)
{
	int mapLength = map->m_cellGrid.size();
	for (int i = 0; i < mapLength; ++i)
	{
		m_nextState.push_back(map->m_cellGrid[i].m_type);
	}
}
