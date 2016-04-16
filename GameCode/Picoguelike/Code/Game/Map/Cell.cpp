#include "Game/Map/Cell.hpp"

//-----------------------------------------------------------------------------------
Cell::Cell()
	: m_position(Vector2Int::ZERO)
	, m_type(Type::AIR)
	, m_isVisible(false)
	, m_knownAs(' ')
{

}

//-----------------------------------------------------------------------------------
Cell::Cell(const Vector2Int& position, Type type)
	: m_position(position)
	, m_type(type)
	, m_isVisible(false)
	, m_knownAs(' ')
{

}

//-----------------------------------------------------------------------------------
Cell::~Cell()
{

}
