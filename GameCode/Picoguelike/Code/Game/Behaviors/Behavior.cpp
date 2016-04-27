#include "Game/Behaviors/Behavior.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"


BehaviorRegistrationMap* BehaviorRegistration::s_behaviorRegistrationMap = nullptr;

//-----------------------------------------------------------------------------------
BehaviorRegistration::BehaviorRegistration(const std::string& name, BehaviorCreationFunction* creationFunction)
	: m_name(name)
	, m_creationFunction(creationFunction)
{
	if (!s_behaviorRegistrationMap)
	{
		s_behaviorRegistrationMap = new BehaviorRegistrationMap();
	}
	//Find, verify that this hasn't been already inserted into our map    
	//s_generatorRegistrationMap->find(name);
	s_behaviorRegistrationMap->insert(BehaviorRegistrationMapPair(name, this));
}

//-----------------------------------------------------------------------------------
Behavior* BehaviorRegistration::CreateBehaviorByNameAndXML(const std::string& name, const XMLNode& dataNode)
{
	BehaviorRegistration* behaviorRegistration = nullptr;
	ASSERT_OR_DIE(s_behaviorRegistrationMap, "s_behaviorRegistrationMap doesn't exist");
	BehaviorRegistrationMap::iterator gRegIter = s_behaviorRegistrationMap->find(name);
	if (gRegIter != s_behaviorRegistrationMap->end())
	{
		behaviorRegistration = gRegIter->second;
		Behavior* createdBehavior = nullptr;
		createdBehavior = (*behaviorRegistration->m_creationFunction)(behaviorRegistration->m_name, dataNode);
		return createdBehavior;
	}
	else
	{
		ERROR_RECOVERABLE(Stringf("XML WARNING:\n  Behavior %s didn't exist! No code file was detected, ignoring this attribute.", name.c_str()));
		return nullptr;
	}
}
//-----------------------------------------------------------------------------------
Behavior::Behavior()
    : m_owningNPC(nullptr)
{

}

Behavior::Behavior(NPC* owningNPC)
    : m_owningNPC(owningNPC)
{

}

//-----------------------------------------------------------------------------------
Behavior::~Behavior()
{

}