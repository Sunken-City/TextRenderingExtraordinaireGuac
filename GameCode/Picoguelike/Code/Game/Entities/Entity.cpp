#include "Game/Entities/Entity.hpp"
#include "Game/Map/Map.hpp"

unsigned int Entity::s_nextId = 1;

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

Entity::~Entity()
{

}

bool Entity::AttemptStep(const Vector2Int& direction)
{
	if (m_map->IsInBounds(m_position + direction) && !m_map->GetCellAt(m_position + direction).IsSolid())
	{
		m_position += direction;
		return true;
	}
	return false;
}

void Entity::Spawn(Map* map)
{
	m_map = map;
	m_position = m_map->GetRandomCellOfType(Cell::Type::AIR).m_position;
}

