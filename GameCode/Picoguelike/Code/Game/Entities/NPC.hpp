#pragma once
#include "Game/Entities/Agent.hpp"
#include "Game/Behaviors/Behavior.hpp"

class Cell;

class NPC : public Agent 
{
public:
	//CONSTRUCTORS//////////////////////////////////////////////////////////////////////////
	NPC();
	NPC(const NPC& copyFrom);
	virtual ~NPC();

	//FUNCTIONS//////////////////////////////////////////////////////////////////////////
	virtual float Update(float deltaSeconds);
    virtual void Render() const;

    static void FindEntitiesInFOV(Cell* cell, void* data);

	//MEMBER FUNCTIONS//////////////////////////////////////////////////////////////////////////
	std::vector<Behavior*> m_behaviors;
};