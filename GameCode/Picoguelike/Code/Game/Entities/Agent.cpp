#include "Game/Entities/Agent.hpp"
#include "Game/Map/Map.hpp"

extern bool g_renderDebug;

//-----------------------------------------------------------------------------------
Agent::Agent()
	: Entity()
	, m_viewDistance(10)
	, m_path(&(m_map->FindValidAdjacentPositions))
{

}

//-----------------------------------------------------------------------------------
Agent::~Agent()
{

}

//-----------------------------------------------------------------------------------
void Agent::Render() const
{
	Entity::Render();
	//if (g_renderDebug)
	{
		m_path.Render();
	}
}
