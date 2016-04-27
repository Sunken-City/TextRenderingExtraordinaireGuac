#pragma once
#include "Engine/Math/Vector2Int.hpp"

class Entity;

class Cell
{
public:
    //ENUMS//////////////////////////////////////////////////////////////////////////
    enum Type
    {
        AIR,
        STONE,
        NUM_CELLS
    };

    //CONSTRUCTORS//////////////////////////////////////////////////////////////////////////
    Cell();
    Cell(const Vector2Int& position, Type type);
    ~Cell();

    //FUNCTIONS//////////////////////////////////////////////////////////////////////////
    inline bool IsSolid() { return m_type == Cell::Type::STONE; };
    bool IsOccupied();
    //MEMBER VARIABLES//////////////////////////////////////////////////////////////////////////
    Vector2Int m_position;
    Type m_type;
    bool m_isVisible;
    char m_knownAs;
    Entity* m_blockingEntity;
};