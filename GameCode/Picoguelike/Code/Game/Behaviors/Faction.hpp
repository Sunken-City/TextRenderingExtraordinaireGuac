#pragma once
#include <string>
#include <map>

class Agent;

//-----------------------------------------------------------------------------------
struct FactionRelationship
{
	int value;
	std::string name;
};

//-----------------------------------------------------------------------------------
class Faction
{
public:
	//TYPEDEFS//////////////////////////////////////////////////////////////////////////
	typedef std::map<std::string, Faction> FactionMap;
	typedef std::map<int, FactionRelationship> FactionRelationshipMap;

	//ENUMS//////////////////////////////////////////////////////////////////////////
	enum FactionStatus
	{
		HATES = -20,
		DISLIKES = -10,
		NEUTRAL = 0,
		LIKES = 10,
		LOVES = 20
	};

	enum FactionAction
	{
		ATTACKS = -5,
		TRIES_TO_ATTACK = -3,
		KILLS = -10,
		HEALS = 8,
	};

	//FUNCTIONS//////////////////////////////////////////////////////////////////////////
	void AdjustFactionStatus(Agent* agentWhoDidSomething, FactionAction whatTheyDid);

	//MEMBER FUNCTIONS//////////////////////////////////////////////////////////////////////////
	static FactionMap s_globalFactions;
	std::string m_name;
	int m_factionID;
	FactionRelationshipMap m_factionRelations;
	FactionRelationshipMap m_agentRelations;
};

