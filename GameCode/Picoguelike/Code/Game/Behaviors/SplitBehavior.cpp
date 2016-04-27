#include "Game/Behaviors/SplitBehavior.hpp"
#include "Game/Entities/Agent.hpp"
#include "Game/Entities/NPC.hpp"
#include "Game/Map/Map.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Math/MathUtils.hpp"

BehaviorRegistration SplitBehavior::s_splitBehaviorRegistration("Split", &SplitBehavior::CreationFunction);

//-----------------------------------------------------------------------------------
SplitBehavior::SplitBehavior()
    : Behavior()
    , m_percentageToRun(0.0f)
{

}

//-----------------------------------------------------------------------------------
SplitBehavior::~SplitBehavior()
{

}

//-----------------------------------------------------------------------------------
Behavior* SplitBehavior::Clone()
{
    return new SplitBehavior(*this);
}

//-----------------------------------------------------------------------------------
Behavior* SplitBehavior::CreationFunction(const std::string& name, const XMLNode& dataNode)
{
    SplitBehavior* behavior = new SplitBehavior();
    behavior->m_percentageToRun = (float)std::stod(dataNode.getAttribute("chanceToSplit"));
    return behavior;
}

//-----------------------------------------------------------------------------------
bool SplitBehavior::DoesPassChanceToRun()
{
    return (m_owningNPC->m_health % 2 == 0 && MathUtils::GetRandomFromZeroTo(1.0f) <= m_percentageToRun);
}

//-----------------------------------------------------------------------------------
float SplitBehavior::CalculateUtility()
{
    return Behavior::Utility::HIGH;
}

//-----------------------------------------------------------------------------------
float SplitBehavior::Run()
{
    Vector2Int foundSpawnLocation = -Vector2Int::ONE;
    if (!m_owningNPC->m_map->FindFirstAdjacentEmptyCell(foundSpawnLocation, m_owningNPC->m_position))
    {
        //Don't actually do anything, since we can't spawn.
        //Print a "tried to split and failed message.
        return 1.0f;
    }
    NPC* clonedAgent = new NPC(*m_owningNPC);
    m_owningNPC->m_health /= 2;
    clonedAgent->m_health  = m_owningNPC->m_health;
    TheGame::instance->SpawnInGame((Agent*)clonedAgent, foundSpawnLocation);
    return 1.0f;
}
