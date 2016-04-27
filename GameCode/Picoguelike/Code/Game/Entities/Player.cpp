#include "Game/Entities/Player.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Game/Map/Map.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/ProfilingUtils.h"

//-----------------------------------------------------------------------------------
Player::Player()
    : Agent()
    , m_action(NO_ACTION)
    , m_direction(EAST)
{
    m_name = "Player";
    m_glyph = '@';
    m_color = RGBA::TURQUOISE;
}

//-----------------------------------------------------------------------------------
Player::~Player()
{

}

//-----------------------------------------------------------------------------------
float Player::Update(float deltaSeconds)
{
    Agent::Update(deltaSeconds);
    switch (m_action)
    {
    case NO_ACTION:
        break;
    case MOVE:
        MoveInCurrentDirection();
        m_map->ResetVisibility();
        m_map->UpdateFOVFrom(m_position, m_viewDistance);
        m_action = NO_ACTION;
        return 1.0f;
        break;
    default:
        break;
    }
    m_action = NO_ACTION;

    //Stress-test pathfinding.
    if (InputSystem::instance->WasKeyJustPressed('P'))
    {
        m_path.m_resultantPath.clear();
        if (InputSystem::instance->IsKeyDown(InputSystem::ExtraKeys::SHIFT))
        {
            std::vector<Vector2Int> startingPositions;
            std::vector<Vector2Int> endingPositions;
            for (int i = 0; i < 10000; ++i)
            {
                startingPositions.push_back(m_map->GetRandomCellOfType(Cell::AIR).m_position);
                endingPositions.push_back(m_map->GetRandomCellOfType(Cell::AIR).m_position);
            }

            StartTiming();
            for (int i = 0; i < 10000; ++i)
            {
                m_path.InitializePathForStep(startingPositions[i], endingPositions[i]);
                while (!m_path.FindPathStep())
                {
                }
                m_path.m_resultantPath.clear();
            }
            DebuggerPrintf("\n10000 steps took %f seconds to run.\n", EndTiming());
        }
        else
        {
            if (!m_path.m_hasBegun)
            {
                Vector2Int goalPosition = m_map->GetRandomCellOfType(Cell::AIR).m_position;
                m_path.InitializePathForStep(m_position, goalPosition);
            }
            bool isFinished = m_path.FindPathStep();
            if (isFinished)
            {
                Vector2Int goalPosition = m_map->GetRandomCellOfType(Cell::AIR).m_position;
                m_path.InitializePathForStep(m_position, goalPosition);
            }
        }
    }
}

//-----------------------------------------------------------------------------------
void Player::Render() const
{
    Agent::Render();
}

//-----------------------------------------------------------------------------------
bool Player::IsReadyToUpdate()
{
    return !(m_action == NO_ACTION);
}

//-----------------------------------------------------------------------------------
void Player::MoveInCurrentDirection()
{
    switch (m_direction)
    {
    case EAST:
        AttemptStep(Vector2Int::UNIT_X);
        break;
    case NORTH_EAST:
        AttemptStep(Vector2Int::ONE);
        break;
    case NORTH:
        AttemptStep(Vector2Int::UNIT_Y);
        break;
    case NORTH_WEST:
        AttemptStep(Vector2Int(-1, 1));
        break;
    case WEST:
        AttemptStep(-Vector2Int::UNIT_X);
        break;
    case SOUTH_WEST:
        AttemptStep(-Vector2Int::ONE);
        break;
    case SOUTH:
        AttemptStep(-Vector2Int::UNIT_Y);
        break;
    case SOUTH_EAST:
        AttemptStep(Vector2Int(1, -1));
        break;
    case NUM_DIRECTIONS:
        ERROR_AND_DIE("Invalid Direction Attempted");
        break;
    default:
        break;
    }
}

//-----------------------------------------------------------------------------------
void Player::QueueMove(Direction dir)
{
    m_direction = dir;
    m_action = Action::MOVE;
}
