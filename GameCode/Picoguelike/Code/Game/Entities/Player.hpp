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

    //CONSTRUCTORS//////////////////////////////////////////////////////////////////////////
    Player();
    virtual ~Player();

    //FUNCTIONS//////////////////////////////////////////////////////////////////////////
    virtual float Update(float deltaSeconds);
    virtual void Render() const;
    virtual bool IsReadyToUpdate();
    void MoveInCurrentDirection();
    void QueueMove(Direction dir);

    //MEMBER VARIABLES//////////////////////////////////////////////////////////////////////////
    Action m_action;
    Direction m_direction;
};