#include "Game/Entities/Agent.hpp"
#include "Game/Map/Map.hpp"
#include "Engine/Math/MathUtils.hpp"

extern bool g_renderDebug;

//-----------------------------------------------------------------------------------
Agent::Agent()
    : Entity()
    , m_viewDistance(10)
    , m_path(&(m_map->FindValidAdjacentPositions))
{

}

// -----------------------------------------------------------------------------------
// Agent::Agent(const Agent& copyFrom)
//     : Entity(copyFrom)
//     , m_viewDistance(copyFrom.m_viewDistance)
//     , m_path(copyFrom.m_path)
//     , m_visibleEntities(copyFrom.m_visibleEntities)
// {
// 
// }

//-----------------------------------------------------------------------------------
Agent::~Agent()
{

}

//-----------------------------------------------------------------------------------
void Agent::Render() const
{
    //if (g_renderDebug)
    {
        m_path.Render();
    }
    Entity::Render();
}

//-----------------------------------------------------------------------------------
Vector2Int Agent::GetRandomDirection()
{
    Agent::Direction randomDirection = (Agent::Direction)MathUtils::GetRandom(0, 8);
    switch (randomDirection)
    {
    case Agent::EAST:
        return Vector2Int::UNIT_X;
    case Agent::NORTH_EAST:
        return Vector2Int::ONE;
    case Agent::NORTH:
        return Vector2Int::UNIT_Y;
    case Agent::NORTH_WEST:
        return Vector2Int(-1, 1);
    case Agent::WEST:
        return -Vector2Int::UNIT_X;
    case Agent::SOUTH_WEST:
        return -Vector2Int::ONE;
    case Agent::SOUTH:
        return -Vector2Int::UNIT_Y;
    case Agent::SOUTH_EAST:
        return Vector2Int(1, -1);
    case Agent::NUM_DIRECTIONS:
        return Vector2Int::ZERO;
    default:
        return Vector2Int::ZERO;
    }
}

//-----------------------------------------------------------------------------------
bool Agent::IsAlive()
{
    return m_health > 0;
}

//-----------------------------------------------------------------------------------
bool Agent::IsReadyToUpdate()
{
    return true;
}

//-----------------------------------------------------------------------------------
void Agent::Attack(AttackData& attackData)
{

}
