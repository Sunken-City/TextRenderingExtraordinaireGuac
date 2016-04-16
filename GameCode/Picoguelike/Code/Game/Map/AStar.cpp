#include "Game/Map/AStar.hpp"
#include "Engine/Renderer/MeshRenderer.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/AABB2.hpp"
#include "Engine/Renderer/BitmapFont.hpp"

//-----------------------------------------------------------------------------------
PathNode::PathNode(const MapPosition& Position, PathNode* Parent, float G, float H)
	: position(Position)
	, parent(Parent)
	, g(G)
	, h(H)
	, f(H + G)
{

}

//-----------------------------------------------------------------------------------
void PathNode::UpdateVariables(PathNode* Parent, float G, float H)
{
	g = G;
	h = H;
	parent = Parent;
	f = G + H;
}

//-----------------------------------------------------------------------------------
Path::Path(FindValidAdjacentPositionsFunction* FindValidAdjacentPositionsFunctionPointer)
	: FindValidAdjacentPositions(FindValidAdjacentPositionsFunctionPointer)
	, m_hasBegun(false)
	, m_currentGoal(-MapPosition::ONE)
{

}

//-----------------------------------------------------------------------------------
void Path::InitializePathForStep(const MapPosition& start, const MapPosition& goal)
{
	if (m_hasBegun)
	{
		m_hasBegun = false;
		m_openList.clear();
		m_closedList.clear();
	}
	Vector2Int distance = MapPosition::Manhattan(start, goal);
	PathNode* startNode = new PathNode(start, nullptr, 0.0f, abs((float)distance.x) + abs((float)distance.y));
	m_openList.push_back(startNode);
	m_currentGoal = goal;
	m_hasBegun = true;
}

//-----------------------------------------------------------------------------------
bool Path::FindPathStep()
{
	if (m_openList.empty() && m_hasBegun)
	{
		return false;
	}
	PathNode* activeNode = FindLowestFNodeOnOpenListAndRemoveIt();
	m_closedList.push_back(activeNode);
	if (activeNode->position == m_currentGoal)
	{
		RecursivelyBuildPathToStartFromNode(activeNode);
		m_hasBegun = false;
		m_openList.clear();
		m_closedList.clear();
		m_currentGoal = -Vector2Int::ONE;
		return true;
	}
	std::vector<PathNode*> validPositions;
	FindValidAdjacentPositions(validPositions, activeNode, m_currentGoal);
	for (PathNode* positionNode : validPositions)
	{
		MapPosition position = positionNode->position;
		if (IsPositionOnClosedList(positionNode->position))
			continue;
		float localG = positionNode->g; //The computed local G.
		float parentG = activeNode->g; //The parent's actual G, which includes the entire cost to get here.
		float h = positionNode->h; //The computed H for the proposed posiion
		PathNode* existingNode = FindOpenNodeWithPosInOpenList(position);
		if (existingNode)
		{
			if (localG + parentG < existingNode->g)
			{
				existingNode->UpdateVariables(activeNode, localG + parentG, h);
			}
			continue;
		}
		PathNode* newNode = new PathNode(position, activeNode, localG + parentG, h);
		m_openList.push_back(newNode);
	}
	return false;
}

//-----------------------------------------------------------------------------------
void Path::Render() const
{
	MeshBuilder builder;
	builder.Begin();
	const float TILE_SIZE = 25.0f;
	for (PathNode* node : m_openList)
	{
		Vector2 bottomLeft = Vector2(node->position) * TILE_SIZE;
		builder.AddTexturedAABB(AABB2(bottomLeft, bottomLeft + (Vector2::ONE * TILE_SIZE)), Vector2::ZERO, Vector2::ONE, RGBA(0x0000FF7F));
		Renderer::instance->DrawText2D(bottomLeft, std::to_string(node->f).substr(0, 4), 0.5f, RGBA::WHITE, false, BitmapFont::CreateOrGetFontFromGlyphSheet("Runescape"));
	}
	for (PathNode* node : m_closedList)
	{
		Vector2 bottomLeft = Vector2(node->position) * TILE_SIZE;
		builder.AddTexturedAABB(AABB2(bottomLeft, bottomLeft + (Vector2::ONE * TILE_SIZE)), Vector2::ZERO, Vector2::ONE, RGBA(0xFF00007F));
		Renderer::instance->DrawText2D(bottomLeft, std::to_string(node->f).substr(0, 4), 0.5f, RGBA::WHITE, false, BitmapFont::CreateOrGetFontFromGlyphSheet("Runescape"));
	}
	for (PathNode* node : m_resultantPath)
	{
		Vector2 bottomLeft = Vector2(node->position) * TILE_SIZE;
		builder.AddTexturedAABB(AABB2(bottomLeft, bottomLeft + (Vector2::ONE * TILE_SIZE)), Vector2::ZERO, Vector2::ONE, RGBA(0x00FF007F));
	}

	Vector2 bottomLeft = (Vector2(m_currentGoal) * TILE_SIZE);
	builder.AddTexturedAABB(AABB2(bottomLeft, bottomLeft + (Vector2::ONE * TILE_SIZE)), Vector2::ZERO, Vector2::ONE, RGBA(0x00FF007F));
	
	builder.End();
	Mesh* mesh = new Mesh();
	builder.CopyToMesh(mesh, &Vertex_PCUTB::Copy);
	MeshRenderer* renderer = new MeshRenderer(mesh, Renderer::instance->m_defaultMaterial);
	renderer->Render();
	delete mesh;
}

//-----------------------------------------------------------------------------------
PathNode* Path::FindLowestFNodeOnOpenListAndRemoveIt()
{
	auto lowestNodeIterator = m_openList.begin();
	for (auto nodeIterator = m_openList.begin(); nodeIterator != m_openList.end(); ++nodeIterator)
	{
		if ((*nodeIterator)->f < (*lowestNodeIterator)->f)
		{
			lowestNodeIterator = nodeIterator;
		}
	}
	PathNode* returnNode = *lowestNodeIterator;
	m_openList.erase(lowestNodeIterator);
	return returnNode;
}

//-----------------------------------------------------------------------------------
PathNode* Path::FindOpenNodeWithPosInOpenList(const MapPosition& position)
{
	for (PathNode* node : m_openList)
	{
		if (node->position == position)
		{
			return node;
		}
	}
	return nullptr;
}

//-----------------------------------------------------------------------------------
void Path::RecursivelyBuildPathToStartFromNode(PathNode* activeNode)
{
	if (activeNode->parent != nullptr)
	{
		m_resultantPath.push_back(activeNode);
		RecursivelyBuildPathToStartFromNode(activeNode->parent);
	}
}

//-----------------------------------------------------------------------------------
bool Path::IsPositionOnClosedList(const MapPosition& position)
{
	for (PathNode* node : m_closedList)
	{
		if (node->position == position)
		{
			return true;
		}
	}
	return false;
}