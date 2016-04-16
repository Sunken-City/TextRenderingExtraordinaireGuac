#pragma once
#include "Game/Map/Cell.hpp"
#include "Engine/Math/Vector2Int.hpp"
#include <vector>
#include <map>

struct XMLNode;
struct PathNode;
class AABB2;

//-----------------------------------------------------------------------------------
struct MapRaycastCollision
{
	Vector2Int startPosition;
	Vector2Int endPosition;
	int numCellsTraveled;
	bool collided;
};

//-----------------------------------------------------------------------------------
class Map
{
public:
	//CONSTRUCTORS//////////////////////////////////////////////////////////////////////////
	Map();
	Map(const Vector2Int& size);
	~Map();

	//FUNCTIONS//////////////////////////////////////////////////////////////////////////
	void Render() const;

	//HELPER FUNCTIONS//////////////////////////////////////////////////////////////////////////
	//Attemtps to set the cell at that location to the specified type, returns false if the tile didn't exist.
	bool AttemptSetCellType(const Vector2Int& cellLocation, Cell::Type type);
	float AttemptSetRectangleOfCells(const AABB2& boxArea, Cell::Type type);
	MapRaycastCollision Raycast(const Vector2Int& start, const Vector2Int& direction, int distanceToTravel, Cell::Type type);
	MapRaycastCollision RaycastAmanatidesWoo(const Vector2& start, const Vector2& end);

	//QUERIES//////////////////////////////////////////////////////////////////////////
	int Map::GetNumberOfCellsOfType(const Vector2Int& location, Cell::Type type, int radius = 1);
	Vector2Int GetWallDirection(const Vector2Int& cellLocation);
	Cell* FindCellAt(const Vector2Int& cellLocation);
	Cell& GetCellAt(const Vector2Int& cellLocation);
	Cell& GetRandomCell();
	Cell& GetRandomCellOfType(Cell::Type type);
	bool IsInBounds(const Vector2Int& location);
	void InitializeLegend(const XMLNode& legendNode);
	void InitializeCharacterLookupLegend();
	void LoadMap(const XMLNode& tileDataNode);
	void SetUnrenderable(Cell& cell);
	void SetIsVisible(Cell& cell);
	void ResetVisibility();
	void UpdateFOVFrom(const Vector2Int& location, int radius = 10);
	static void FindValidAdjacentPositions(std::vector<PathNode*>& outValidPositionsVector, PathNode* start, const Vector2Int& end);

	//MEMBER VARIABLES//////////////////////////////////////////////////////////////////////////
	std::vector<Cell> m_cellGrid;
	Vector2Int m_size;
	std::map<char, Cell::Type> m_legend;
	std::map<Cell::Type, char> m_characterLegend;
};