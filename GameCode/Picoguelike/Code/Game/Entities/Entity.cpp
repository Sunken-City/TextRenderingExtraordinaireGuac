#include "Game/Entities/Entity.hpp"
#include "Game/Map/Map.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Renderer.hpp"

unsigned int Entity::s_nextId = 1;

//-----------------------------------------------------------------------------------
Entity::Entity()
    : m_position(Vector2Int::ZERO)
    , m_glyph('Q')
    , m_color(RGBA::VAPORWAVE)
    , m_bgColor(RGBA::WHITE)
    , m_name("Uninitialized")
    , m_health(1)
    , m_id(++s_nextId)
    , m_map(nullptr)
{

}

//-----------------------------------------------------------------------------------
Entity::Entity(const Entity& copyFrom)
    : m_id(++s_nextId)
    , m_position(copyFrom.m_position)
    , m_glyph(copyFrom.m_glyph)
    , m_color(copyFrom.m_color)
    , m_bgColor(copyFrom.m_bgColor)
    , m_name(copyFrom.m_name)
    , m_health(copyFrom.m_health)
    , m_map(copyFrom.m_map)
{

}

//-----------------------------------------------------------------------------------
Entity::~Entity()
{

}

//-----------------------------------------------------------------------------------
void Entity::Render() const
{
    Vector2 mapOffset(0.0f, 0.0f);
    char* characterString = new char[2];
    characterString[0] = m_glyph;
    characterString[1] = '\0';
    Renderer::instance->DrawText2D((Vector2(m_position) * 25.0f) + mapOffset, characterString, 1.0f, m_color, false, BitmapFont::CreateOrGetFontFromGlyphSheet("Runescape"));
    delete characterString;
}

//-----------------------------------------------------------------------------------
bool Entity::AttemptStep(const Vector2Int& direction)
{
    Cell& attemptedCell = m_map->GetCellAt(m_position + direction);
    if (abs(direction.x) == 1.0f && abs(direction.y) == 1.0f)
    {
        if (m_map->GetCellAt(m_position + Vector2Int(direction.x, 0.0f)).IsSolid() && m_map->GetCellAt(m_position + Vector2Int(0.0f, direction.y)).IsSolid())
        {
            return false;
        }
    }
    if (m_map->IsInBounds(m_position + direction) && !attemptedCell.IsSolid() && !attemptedCell.m_blockingEntity)
    {
        m_map->GetCellAt(m_position).m_blockingEntity = nullptr;
        m_position += direction;
        attemptedCell.m_blockingEntity = this;
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------------
void Entity::Spawn(Map* map, const Vector2Int& spawnPosition)
{
    m_map = map;
    if (spawnPosition == -Vector2Int::ONE)
    {
        Cell& tempCell = m_map->GetRandomCellOfType(Cell::Type::AIR);
        m_position = tempCell.m_position;
        tempCell.m_blockingEntity = this;
    }
    else
    {
        Cell& tempCell = m_map->GetCellAt(spawnPosition);
        m_position = tempCell.m_position;
        tempCell.m_blockingEntity = this;
    }
}

