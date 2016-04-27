#include "Game/Behaviors/ChaseBehavior.hpp"
#include "Game/Entities/NPC.hpp"
#include "Game/Map/Map.hpp"

BehaviorRegistration ChaseBehavior::s_chaseBehaviorRegistration("Chase", &ChaseBehavior::CreationFunction);

//-----------------------------------------------------------------------------------
ChaseBehavior::ChaseBehavior()
    : Behavior()
    , m_targetToChase(nullptr)
    , m_numberOfStepsChased(0)
    , m_maxDistanceChased(10)
    , m_cooldownTurns(5)
{

}

//-----------------------------------------------------------------------------------
ChaseBehavior::~ChaseBehavior()
{

}

//-----------------------------------------------------------------------------------
Behavior* ChaseBehavior::Clone()
{
    return new ChaseBehavior();
}

//-----------------------------------------------------------------------------------
Behavior* ChaseBehavior::CreationFunction(const std::string& name, const XMLNode& dataNode)
{
    return new ChaseBehavior();
}

//-----------------------------------------------------------------------------------
bool ChaseBehavior::DoesPassChanceToRun()
{
    return true;
}

//-----------------------------------------------------------------------------------
float ChaseBehavior::CalculateUtility()
{
    if (m_numberOfStepsChased < 0)
    {
        ++m_numberOfStepsChased;
        return 0.0f;
    }
    if (m_owningNPC->m_visibleEntities.size() > 0)
    {
        int manhattanDistance = INT_MAX;
        for (Entity* entity : m_owningNPC->m_visibleEntities)
        {
            Vector2Int distance = Vector2Int::Manhattan(m_owningNPC->m_position, entity->m_position);
            int newDistance = distance.x + distance.y;
            if (manhattanDistance > newDistance)
            {
                manhattanDistance = newDistance;
                m_targetToChase = entity;
            }
        }
        if (Map::IsAdjacent(m_owningNPC->m_position, m_targetToChase->m_position))
        {
            return static_cast<float>(Behavior::Utility::NONE);
        }
        return static_cast<float>(Behavior::Utility::MEDIUM);
    }
    else
    {
        return static_cast<float>(Behavior::Utility::NONE);
    }
}

//-----------------------------------------------------------------------------------
float ChaseBehavior::Run()
{
    if (++m_numberOfStepsChased > m_maxDistanceChased)
    {
        m_numberOfStepsChased = -m_cooldownTurns;
        m_owningNPC->m_path.ClearPath();
        return 1.0f;
    }
    m_owningNPC->m_path.FindPath(m_owningNPC->m_position, m_targetToChase->m_position);
    int firstStep = m_owningNPC->m_path.m_resultantPath.size() - 1;
    m_owningNPC->AttemptStep(m_owningNPC->m_path.m_resultantPath[firstStep]->position - m_owningNPC->m_position);
    return 1.0f;
}
