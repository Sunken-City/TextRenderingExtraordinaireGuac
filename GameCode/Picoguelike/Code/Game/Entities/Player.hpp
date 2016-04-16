#pragma once
#include "Game/Entities/Agent.hpp"

class Player : public Agent
{
public:
	//ENUMS//////////////////////////////////////////////////////////////////////////
	enum Action
	{
		NO_ACTION,
		MOVE,
		NUM_ACTIONS
	};

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
	Player();
	virtual ~Player();

	//FUNCTIONS//////////////////////////////////////////////////////////////////////////
	virtual void Update(float deltaSeconds);
	virtual void Render() const;
	void MoveInCurrentDirection();
	void QueueMove(Direction dir);

	//MEMBER VARIABLES//////////////////////////////////////////////////////////////////////////
	Action m_action;
	Direction m_direction;
};