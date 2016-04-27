#include "Game/Entities/NPC.hpp"
#include "Game/Map/Cell.hpp"
#include "Game/Map/Map.hpp"

//-----------------------------------------------------------------------------------
NPC::NPC()
	: Agent()
{

}

NPC::NPC(const NPC& copyFrom)
    : Agent(copyFrom)
{
	for (Behavior* behavior : copyFrom.m_behaviors)
	{
        m_behaviors.push_back(behavior->Clone());
	}
}

//-----------------------------------------------------------------------------------
NPC::~NPC()
{

}

//-----------------------------------------------------------------------------------
float NPC::Update(float deltaSeconds)
{
	Agent::Update(deltaSeconds);
    m_map->UpdateFOVFrom(m_position, m_viewDistance, &NPC::FindEntitiesInFOV, (void*)this);
    Behavior* selectedBehavior = nullptr;
    float bestUtilitySoFar = -1.0f;
    for (int i = 0; i < m_behaviors.size(); ++i)
    {
        m_behaviors[i]->SetAgent(this);
        if (m_behaviors[i]->DoesPassChanceToRun())
        {
            float currentUtility = m_behaviors[i]->CalculateUtility();
            if (bestUtilitySoFar < currentUtility)
            {
                bestUtilitySoFar = currentUtility;
                selectedBehavior = m_behaviors[i];
            }
        }
    }
    if (selectedBehavior)
    {
        return selectedBehavior->Run();
    }
    return 0.5f;
}

//-----------------------------------------------------------------------------------
void NPC::Render() const
{
	Agent::Render();
}

//-----------------------------------------------------------------------------------
void NPC::FindEntitiesInFOV(Cell* cell, void* data)
{
    NPC* callingNPC = (NPC*)data;
    if (cell->m_blockingEntity && cell->m_blockingEntity != callingNPC)
    {
        callingNPC->m_visibleEntities.push_back(cell->m_blockingEntity);
    }
}
