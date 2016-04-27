#pragma once
#include "Engine/Math/Vector2Int.hpp"
#include "Engine/Renderer/RGBA.hpp"
#include <string>

class Map;

class Entity
{
public:
    //ENUMS//////////////////////////////////////////////////////////////////////////
    enum Type
    {
        PLAYER,
        ENEMY,
        ITEM,
        NUM_ENTITIES
    };

    //CONSTRUCTORS//////////////////////////////////////////////////////////////////////////
    Entity();
    Entity(const Entity& copyFrom);
    virtual ~Entity();

    //FUNCTIONS//////////////////////////////////////////////////////////////////////////
    virtual float Update(float deltaSeconds) { return 0.0f; };
    virtual void Render() const;
    virtual bool AttemptStep(const Vector2Int& direction);
    virtual void Spawn(Map* map, const Vector2Int& spawnPosition = Vector2Int(-1, -1));

    //QUERIES//////////////////////////////////////////////////////////////////////////
    virtual inline bool IsPlayer() { return false; };

    //MEMBER VARIABLES//////////////////////////////////////////////////////////////////////////
    Vector2Int m_position;
    char m_glyph;
    RGBA m_color;
    RGBA m_bgColor;
    std::string m_name;
    unsigned int m_health;
    unsigned int m_maxHealth;
    unsigned int m_id;
    Map* m_map;

    static unsigned int s_nextId;
};