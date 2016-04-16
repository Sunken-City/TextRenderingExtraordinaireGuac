#include "Game/Generators/DungeonGenerator.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/AABB2.hpp"

GeneratorRegistration DungeonGenerator::s_dungeonGeneratorRegistration("Dungeon", &DungeonGenerator::CreateGenerator, &Generator::CreateGenerationProcessData);

//-----------------------------------------------------------------------------------
DungeonGenerator::DungeonGenerator(const std::string& name)
{
	m_name = name;
}

DungeonGenerator::~DungeonGenerator()
{

}

void DungeonGenerator::InitializeMap(Map* map)
{
	//m_currentRootCellLocation = Vector2Int(map->m_size.x / 2, map->m_size.y / 2);
	Cell& randomCell = map->GetRandomCell();
	randomCell.m_type = Cell::AIR;
	m_currentRootCellLocation = randomCell.m_position;
}

bool DungeonGenerator::GenerateStep(int& currentStepNumber, GenerationProcessData* data, Map* map /*= nullptr*/)
{
	static int numSteps = data->m_rangeSteps.x - data->m_rangeSteps.y == 0 ? data->m_rangeSteps.x : MathUtils::GetRandom(data->m_rangeSteps.x, data->m_rangeSteps.y);

	bool madeRoom = false;
	int numIterations = 0;
	while (!madeRoom)
	{
		if (++numIterations > 50)
		{
			return true;
		}

		Cell* randomCell = &map->GetRandomCell();
		while (!(randomCell->m_type == Cell::AIR && map->GetWallDirection(randomCell->m_position) != Vector2Int::ZERO))
		{
			randomCell = &map->GetRandomCell();
		}
		m_currentRootCellLocation = randomCell->m_position;

		int hallwayLength = MathUtils::GetRandom(5, 10);
		int roomHeight = MathUtils::GetRandom(5, 7);
		int roomWidth = MathUtils::GetRandom(7, 10);
		Vector2Int hallwayDirection = map->GetWallDirection(m_currentRootCellLocation);
		Vector2Int selectedCell = m_currentRootCellLocation;
	
		MapRaycastCollision collision = map->Raycast(selectedCell, hallwayDirection, hallwayLength, Cell::AIR);
		if (collision.numCellsTraveled < 5)
		{
			continue;
		}
		for (int i = 0; i < collision.numCellsTraveled; ++i)
		{
			selectedCell += hallwayDirection;
			map->AttemptSetCellType(selectedCell, Cell::AIR);
		}

		Vector2Int cursor = selectedCell;
		Vector2Int perpendicularDirection = Vector2Int(hallwayDirection.y, hallwayDirection.x);
		int aToBRatio = MathUtils::GetRandom(0, roomWidth - 1);

		//These names are relative to a diagram that I had to draw out for myself.
		//I would have used left and right, but since the hallways could be coming from
		//Any direction, I had to use something arbitrary.
		//Assuming that the hallway is pointing along positive Y, the directions are as follows:
		//A - Negative X, B & E -  Positive X, C & D - Positive Y
		MapRaycastCollision A = map->Raycast(cursor, -perpendicularDirection, aToBRatio, Cell::AIR);
		MapRaycastCollision B = map->Raycast(cursor, perpendicularDirection, roomWidth - aToBRatio, Cell::AIR);
		MapRaycastCollision C = map->Raycast(A.endPosition, hallwayDirection, roomHeight, Cell::AIR);
		MapRaycastCollision D = map->Raycast(B.endPosition, hallwayDirection, roomHeight, Cell::AIR);
		//Back up if we hit a wall.
		if (A.numCellsTraveled != aToBRatio)
		{
			A.endPosition += perpendicularDirection;
			--A.numCellsTraveled;
		}
		if (B.numCellsTraveled != roomWidth - aToBRatio)
		{
			B.endPosition -= perpendicularDirection;
			--B.numCellsTraveled;
		}
		if (C.numCellsTraveled != roomHeight)
		{
			C.endPosition -= hallwayDirection;
			--C.numCellsTraveled;
		}
		if (D.numCellsTraveled != roomHeight)
		{
			D.endPosition -= hallwayDirection;
			--D.numCellsTraveled;
		}
		MapRaycastCollision E = map->Raycast(C.endPosition, perpendicularDirection, roomWidth, Cell::AIR);;
		
		AABB2 room;
		if (A.endPosition < E.endPosition)
		{
			room = AABB2(A.endPosition, E.endPosition);
		}
		else
		{
			room = AABB2(E.endPosition, A.endPosition);
		}

		map->AttemptSetRectangleOfCells(room, Cell::AIR);
		madeRoom = true;
	}
	return (++currentStepNumber > numSteps);
}

Vector2Int DungeonGenerator::GetRandomDirection()
{
	int random = MathUtils::GetRandom(0, 4);
	switch (random)
	{
	case 0:
		return Vector2Int::UNIT_X;
	case 1:
		return Vector2Int::UNIT_X * -1;
	case 2:
		return Vector2Int::UNIT_Y;
	case 3:
		return Vector2Int::UNIT_Y * -1;
	}
	return Vector2Int::ONE;
}
