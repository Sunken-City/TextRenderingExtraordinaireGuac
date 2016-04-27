#include "Game/Behaviors/Faction.hpp"
#include "Game/Entities/Agent.hpp"

void Faction::AdjustFactionStatus(Agent* agentWhoDidSomething, FactionAction whatTheyDid)
{
	//m_factionID = agentWhoDidSomething->m_faction;
	int entityID = agentWhoDidSomething->m_id;

	//auto agentIter = m_agentRelations(entityID);
}

