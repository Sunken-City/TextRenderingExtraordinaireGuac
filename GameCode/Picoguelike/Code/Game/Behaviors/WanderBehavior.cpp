#include "Game/Behaviors/WanderBehavior.hpp"
#include "Game/Entities/NPC.hpp"
#include "Engine/Math/MathUtils.hpp"

BehaviorRegistration WanderBehavior::s_wanderBehaviorRegistration("Wander", &WanderBehavior::CreationFunction);

//-----------------------------------------------------------------------------------
WanderBehavior::WanderBehavior()
	: Behavior()
{

}

//-----------------------------------------------------------------------------------
WanderBehavior::~WanderBehavior()
{

}

//-----------------------------------------------------------------------------------
Behavior* WanderBehavior::CreationFunction(const std::string& name, const XMLNode& dataNode)
{
	return new WanderBehavior();
}

//-----------------------------------------------------------------------------------
bool WanderBehavior::DoesPassChanceToRun()
{
    return true;
}

//-----------------------------------------------------------------------------------
float WanderBehavior::CalculateUtility()
{
    return Behavior::Utility::LOW;
}

//-----------------------------------------------------------------------------------
float WanderBehavior::Run()
{
    m_owningNPC->AttemptStep(Agent::GetRandomDirection());
    return 1.0f;
}

//-----------------------------------------------------------------------------------
Behavior* WanderBehavior::Clone()
{
	return new WanderBehavior();
}
