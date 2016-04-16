#include "Game/Map/Map.hpp"
#include "Game/Map/Cell.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/AABB2.hpp"
#include "Engine/Input/XMLUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Input/InputSystem.hpp"
#include <algorithm>
#include "AStar.hpp"
#include "../TheGame.hpp"

extern bool g_renderDebug;

//-----------------------------------------------------------------------------------
Map::Map()
	: m_size(Vector2Int::ZERO)
{
	
}

//-----------------------------------------------------------------------------------
Map::Map(const Vector2Int& size)
	: m_size(size)
{
	for (int y = 0; y < m_size.y; ++y)
	{
		for (int x = 0; x < m_size.x; ++x)
		{
			m_cellGrid.emplace_back(Vector2Int(x, y), Cell::Type::STONE);
		}
	}
	InitializeCharacterLookupLegend();
}

//-----------------------------------------------------------------------------------
Map::~Map()
{

}

//-----------------------------------------------------------------------------------
void Map::Render() const
{
	for (unsigned int i = 0; i < m_cellGrid.size(); i++)
	{
		const Cell& cell = m_cellGrid[i];
		Vector2 mapOffset(0.0f, 0.0f);
		RGBA alphaShift = RGBA(0x00000000);
		if (cell.m_knownAs == '\0')
		{
			continue;
		}

		if ((!g_renderDebug && !InputSystem::instance->IsKeyDown('M')) && !cell.m_isVisible)
		{
			if (cell.m_knownAs == ' ')
			{
				continue;
			}
			else 
			{
				alphaShift.alpha = 0x7F;
			}
		}

		switch (cell.m_type)
		{
		case Cell::AIR:
			Renderer::instance->DrawText2D((Vector2(cell.m_position) * 25.0f) + mapOffset, ".", 1.0f, RGBA::FOREST_GREEN - alphaShift, false, BitmapFont::CreateOrGetFontFromGlyphSheet("Runescape"));
			break;
		case Cell::STONE:
			Renderer::instance->DrawText2D((Vector2(cell.m_position) * 25.0f) + mapOffset, "#", 1.0f, RGBA::CHOCOLATE - alphaShift, false, BitmapFont::CreateOrGetFontFromGlyphSheet("Runescape"));
			break;
		default:
			break;
		}
	}
}

//-----------------------------------------------------------------------------------
bool Map::AttemptSetCellType(const Vector2Int& cellLocation, Cell::Type type)
{
	if (IsInBounds(cellLocation))
	{
		GetCellAt(cellLocation).m_type = type;
		return true;
	}
	else
	{
		return false;
	}
}

//-----------------------------------------------------------------------------------
float Map::AttemptSetRectangleOfCells(const AABB2& boxArea, Cell::Type type)
{
	float numCells = 0.0f;
	float numSet = 0.0f;
	Vector2Int mins(boxArea.mins);
	Vector2Int maxs(boxArea.maxs);
	for (int y = static_cast<int>(boxArea.mins.y); y < static_cast<int>(boxArea.maxs.y); y++)
	{
		++numCells;
		for (int x = static_cast<int>(boxArea.mins.x); x < static_cast<int>(boxArea.maxs.x); x++)
		{
			if (AttemptSetCellType(Vector2Int(x, y), type))
			{
				++numSet;
			}
		}
	}
	return numSet / numCells;
}

//-----------------------------------------------------------------------------------
MapRaycastCollision Map::Raycast(const Vector2Int& start, const Vector2Int& direction, int distanceToTravel, Cell::Type type)
{
	MapRaycastCollision collisionInfo;
	collisionInfo.startPosition = start;
	Vector2Int currentPosition = start;
	for (int i = 0; i < distanceToTravel; ++i)
	{
		currentPosition += direction;
		if (!IsInBounds(currentPosition))
		{
			collisionInfo.numCellsTraveled = --i;
			collisionInfo.endPosition = currentPosition - direction;
			collisionInfo.collided = true;
			return collisionInfo;
		}
		if (GetCellAt(currentPosition).m_type == type)
		{
			collisionInfo.numCellsTraveled = i;
			collisionInfo.endPosition = currentPosition;
			collisionInfo.collided = true;
			return collisionInfo;
		}
	}
	collisionInfo.numCellsTraveled = distanceToTravel;
	collisionInfo.endPosition = currentPosition;
	collisionInfo.collided = false;
	return collisionInfo;
}

//-----------------------------------------------------------------------------------
int Map::GetNumberOfCellsOfType(const Vector2Int& location, Cell::Type type, int radius /*= 1*/)
{
	int startX = location.x - radius;
	int endX = location.x + radius + 1;
	int startY = location.y - radius;
	int endY = location.y + radius + 1;
	unsigned int typeCount = 0;
	for (int y = startY; y < endY; ++y)
	{
		for (int x = startX; x < endX; ++x)
		{
			//If that cell is out of bounds, continue.
			if (!IsInBounds(Vector2Int(x, y)) || (x == location.x && y == location.y))
			{
				continue;
			}
			if (GetCellAt(Vector2Int(x, y)).m_type == type)
			{
				++typeCount;
			}
		}
	}
	return typeCount;
}

//-----------------------------------------------------------------------------------
Vector2Int Map::GetWallDirection(const Vector2Int& cellLocation)
{
	Vector2Int East = cellLocation + Vector2Int::UNIT_X;
	Vector2Int West = cellLocation - Vector2Int::UNIT_X;
	Vector2Int North = cellLocation + Vector2Int::UNIT_Y;
	Vector2Int South = cellLocation - Vector2Int::UNIT_Y;
	if (IsInBounds(East) && GetCellAt(East).m_type == Cell::STONE)
	{
		return East - cellLocation;
	}
	else if (IsInBounds(West) && GetCellAt(West).m_type == Cell::STONE)
	{
		return West - cellLocation;
	}
	else if (IsInBounds(North) && GetCellAt(North).m_type == Cell::STONE)
	{
		return North - cellLocation;
	}
	else if (IsInBounds(South) && GetCellAt(South).m_type == Cell::STONE)
	{
		return South - cellLocation;
	}
	return Vector2Int::ZERO;
}

//-----------------------------------------------------------------------------------
Cell* Map::FindCellAt(const Vector2Int& cellLocation)
{
	unsigned int index = 0;
	index = cellLocation.x + (cellLocation.y * m_size.x);
	if (!(index < m_cellGrid.size()))
	{
		return nullptr;
	}
	return &m_cellGrid[index];
}

//-----------------------------------------------------------------------------------
Cell& Map::GetCellAt(const Vector2Int& cellLocation)
{
	unsigned int index = 0;
	index = cellLocation.x + (cellLocation.y * m_size.x);
	ASSERT_OR_DIE(index < m_cellGrid.size(), "Attempted to access an out of bounds cell with GetCellAt, index was too high");
	return m_cellGrid[index];
}

//-----------------------------------------------------------------------------------
Cell& Map::GetRandomCell()
{
	int randomX = MathUtils::GetRandom(0, m_size.x);
	int randomY = MathUtils::GetRandom(0, m_size.y);
	return GetCellAt(Vector2Int(randomX, randomY));
}

//-----------------------------------------------------------------------------------
Cell& Map::GetRandomCellOfType(Cell::Type type)
{
	Cell::Type foundType = Cell::NUM_CELLS;
	Cell* foundCell;
	while (type != foundType)
	{
		int randomX = MathUtils::GetRandom(0, m_size.x);
		int randomY = MathUtils::GetRandom(0, m_size.y);
		foundCell = &GetCellAt(Vector2Int(randomX, randomY));
		foundType = foundCell->m_type;
	}
	return *foundCell;
}

//-----------------------------------------------------------------------------------
bool Map::IsInBounds(const Vector2Int& location)
{
	return !(location.x < 0 || location.x >= m_size.x || location.y < 0 || location.y >= m_size.y);
}

//-----------------------------------------------------------------------------------
void Map::InitializeCharacterLookupLegend()
{
	m_characterLegend[Cell::Type::AIR] = '.';
	m_characterLegend[Cell::Type::STONE] = '#';
}

//-----------------------------------------------------------------------------------
void Map::InitializeLegend(const XMLNode& legendNode)
{
	if (m_legend.size() != 0)
	{
		m_legend.clear();
	}
	std::vector<XMLNode> children = XMLUtils::GetChildren(legendNode);
	for (XMLNode node : children)
	{
		if (!node.IsContentEmpty())
		{
			std::string characterString = XMLUtils::GetAttribute(node, "char");
			std::string typeName = XMLUtils::GetAttribute(node, "type");
			char character = characterString.c_str()[0];
			Cell::Type type = Cell::Type::AIR;
			if (typeName == "stone")
			{
				type = Cell::Type::STONE;
			}
			else if (typeName == "air")
			{
				type = Cell::Type::AIR;
			}
			m_legend[character] = type;
		}
	}
}

//-----------------------------------------------------------------------------------
void Map::LoadMap(const XMLNode& tileDataNode)
{
	//Delete the map if it has trash in it already.
	if (m_cellGrid.size() != 0)
	{
		m_cellGrid.clear();
	}
	std::string tileDataString = tileDataNode.getText();
	std::vector<std::string>* mapRows = SplitString(tileDataString, "\r\n");
	m_size = Vector2Int(mapRows->at(0).length(), mapRows->size());
	reverse(mapRows->begin(), mapRows->end());
	int y = 0;
	for (std::string row : *mapRows)
	{
		int x = 0;
		for (char symbol : row)
		{
			m_cellGrid.push_back(Cell(Vector2Int(x, y), m_legend.at(symbol)));
			++x;
		}
		++y;
	}
	delete mapRows;
}

//-----------------------------------------------------------------------------------
void Map::SetUnrenderable(Cell& cell)
{
	if (GetNumberOfCellsOfType(cell.m_position, Cell::STONE, 1) == 8)
	{
		cell.m_isVisible = false;
		cell.m_knownAs = '\0';
	}
}

//-----------------------------------------------------------------------------------
void Map::SetIsVisible(Cell& cell)
{
	if (GetNumberOfCellsOfType(cell.m_position, Cell::STONE, 1) == 8)
	{
		cell.m_isVisible = false;
	}
}

//-----------------------------------------------------------------------------------
void Map::ResetVisibility()
{
	for (Cell& cell : m_cellGrid)
	{
		cell.m_isVisible = false;
	}
}

//-----------------------------------------------------------------------------------
void Map::UpdateFOVFrom(const Vector2Int& location, int radius /*= 10*/)
{
	ResetVisibility();
	int startX = location.x - radius;
	int endX = location.x + radius + 1;
	int startY = location.y - radius;
	int endY = location.y + radius + 1;
	for (int y = startY; y < endY; ++y)
	{
		for (int x = startX; x < endX; ++x)
		{
			if (x != startX && x != (endX - 1) && y != startY && y != (endY - 1))
			{
				continue;
			}
			MapRaycastCollision result = RaycastAmanatidesWoo(Vector2(location) + Vector2::ONE * 0.5, Vector2(x + 0.5f, y + 0.5f));
		}
	}
	
}

//-----------------------------------------------------------------------------------
void Map::FindValidAdjacentPositions(std::vector<PathNode*>& outValidPositionsVector, PathNode* start, const Vector2Int& end)
{
	for (int y = -1; y < 2; ++y)
	{
		for (int x = -1; x < 2; ++x)
		{
			Vector2Int directionOffset(x, y);
			Cell* foundCell = TheGame::instance->m_currentMap->FindCellAt(start->position + directionOffset);
			if (foundCell != nullptr && !foundCell->IsSolid() && foundCell->m_position != start->position)
			{
				float gCostForMove = 1.0f;
				if (abs(x) + abs(y) > 1)
				{
					gCostForMove = 1.4f;
				}
				Vector2Int hVector = Vector2Int::Manhattan(foundCell->m_position, end);
				outValidPositionsVector.push_back(new PathNode(foundCell->m_position, start, (gCostForMove), (hVector.x + hVector.y)));
			}
		}
	}
}

//-----------------------------------------------------------------------------------
MapRaycastCollision Map::RaycastAmanatidesWoo(const Vector2& start, const Vector2& end)
{
	MapRaycastCollision result;
	result.collided = false;
	result.startPosition = start;
	result.endPosition = end;
	result.numCellsTraveled = 0;

	Cell* startCell = FindCellAt(Vector2Int(start));
	if (!startCell)
	{
		ERROR_AND_DIE("startCell was null!");
	}
	startCell->m_isVisible = true;
	startCell->m_knownAs = m_characterLegend[startCell->m_type];

	Vector2 rayDisplacement = end - start;
	Vector2Int rayPosition = startCell->m_position;
	float tDeltaX = abs(1.0f / rayDisplacement.x);
	int tileStepX = rayDisplacement.x > 0 ? 1 : -1;
	int offsetToLeadingEdgeX = (tileStepX + 1) / 2;
	float firstVerticalIntersectionX = (float)(rayPosition.x + offsetToLeadingEdgeX);
	float tOfNextXCrossing = abs(firstVerticalIntersectionX - start.x) * tDeltaX;

	float tDeltaY = abs(1.0f / rayDisplacement.y);
	int tileStepY = rayDisplacement.y > 0 ? 1 : -1;
	int offsetToLeadingEdgeY = (tileStepY + 1) / 2;
	float firstVerticalIntersectionY = (float)(rayPosition.y + offsetToLeadingEdgeY);
	float tOfNextYCrossing = abs(firstVerticalIntersectionY - start.y) * tDeltaY;

	while (true)
	{
		if (tOfNextXCrossing < tOfNextYCrossing)
		{
			if (tOfNextXCrossing > 1)
			{
				result.collided = false;
				result.endPosition = rayPosition;
				return result;
			}
			rayPosition.x += tileStepX;
			Cell* nextFoundCell = FindCellAt(Vector2Int(rayPosition));
			if (!nextFoundCell)
			{
				result.collided = false;
				result.endPosition = rayPosition;
				return result;
			}
			nextFoundCell->m_isVisible = true;
			nextFoundCell->m_knownAs = m_characterLegend[nextFoundCell->m_type];
			if (nextFoundCell->IsSolid())
			{
				result.collided = true;
				result.startPosition = start;
				result.endPosition = nextFoundCell->m_position;
				return result;
			}
			tOfNextXCrossing += tDeltaX;
		}
		else
		{
			if (tOfNextYCrossing > 1)
			{
				result.collided = false;
				result.endPosition = rayPosition;
				return result;
			}
			rayPosition.y += tileStepY;
			Cell* nextFoundCell = FindCellAt(Vector2Int(rayPosition));
			if (!nextFoundCell)
			{
				result.collided = false;
				result.endPosition = rayPosition;
				return result;
			}
			nextFoundCell->m_isVisible = true;
			nextFoundCell->m_knownAs = m_characterLegend[nextFoundCell->m_type];
			if (nextFoundCell->IsSolid())
			{
				result.collided = true;
				result.startPosition = start;
				result.endPosition = nextFoundCell->m_position;
				return result;
			}
			tOfNextYCrossing += tDeltaY;
		}
	}
}
