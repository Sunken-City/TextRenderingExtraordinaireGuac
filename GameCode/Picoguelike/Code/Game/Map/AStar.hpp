#pragma once
#include <vector>
#include "Engine/Math/Vector2Int.hpp"

struct PathNode;

//TYPEDEFS//////////////////////////////////////////////////////////////////////////
typedef Vector2Int MapPosition;
typedef void (FindValidAdjacentPositionsFunction)(std::vector<PathNode*>& outValidPositionsVector, PathNode* start, const Vector2Int& end);

//-----------------------------------------------------------------------------------
struct PathNode
{
	//CONSTRUCTORS//////////////////////////////////////////////////////////////////////////
	//Z gets automatically set to G + H
	PathNode(const MapPosition& position, PathNode* parent, float g, float h);

	//Z gets automatically set to G + H
	void UpdateVariables(PathNode* parent, float g, float h);

	//MEMBER VARIABLES//////////////////////////////////////////////////////////////////////////
	MapPosition position;
	PathNode* parent;
	float g;
	float h;
	float f;
};

//-----------------------------------------------------------------------------------
class Path
{
public:
	//CONSTRUCTORS//////////////////////////////////////////////////////////////////////////
	Path(FindValidAdjacentPositionsFunction* FindValidAdjacentPositions);

	//FUNCTIONS//////////////////////////////////////////////////////////////////////////
	void InitializePathForStep(const MapPosition& start, const MapPosition& goal);
	bool FindPathStep();
	void Render() const;

	//MEMBER VARIABLES//////////////////////////////////////////////////////////////////////////
	std::vector<PathNode*> m_openList;
	std::vector<PathNode*> m_closedList;
	std::vector<PathNode*> m_resultantPath;
	MapPosition m_currentGoal;
	bool m_hasBegun;
	FindValidAdjacentPositionsFunction* FindValidAdjacentPositions;

private:
	//HELPER FUNCTIONS//////////////////////////////////////////////////////////////////////////
	PathNode* FindLowestFNodeOnOpenListAndRemoveIt();
	PathNode* FindOpenNodeWithPosInOpenList(const MapPosition& position);
	void RecursivelyBuildPathToStartFromNode(PathNode* activeNode);
	bool IsPositionOnClosedList(const MapPosition& position);
};