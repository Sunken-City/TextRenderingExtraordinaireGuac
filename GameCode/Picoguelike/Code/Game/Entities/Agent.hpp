#pragma once
#include "Game/Entities/Entity.hpp"
#include "Game/Map/AStar.hpp"
#include "Game/CombatSystem.hpp"
#include <vector>

class Agent : public Entity
{
public:
    //ENUMS//////////////////////////////////////////////////////////////////////////
    enum Direction
    {
        EAST,
        NORTH_EAST,
        NORTH,
        NORTH_WEST,
        WEST,
        SOUTH_WEST,
        SOUTH,
        SOUTH_EAST,
        NUM_DIRECTIONS
    };

	//CONSTRUCTORS//////////////////////////////////////////////////////////////////////////
	Agent();
	virtual ~Agent();

	//FUNCTIONS//////////////////////////////////////////////////////////////////////////
    virtual float Update(float deltaSeconds) { Entity::Update(deltaSeconds); return 1.0f; };
	virtual void Render() const;
    virtual bool IsReadyToUpdate();
    virtual void Attack(AttackData& attackData);

    static Vector2Int GetRandomDirection();
    bool IsAlive();

    //MEMBER VARIABLES//////////////////////////////////////////////////////////////////////////
	int m_viewDistance;
    Path m_path;
    std::vector<Entity*> m_visibleEntities;
};