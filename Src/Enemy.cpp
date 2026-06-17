/*
    ============================================================
    Enemy.cpp

    Author: Leonardo Moura
    Date: 6/12/2026

    Description:

    Implements the Enemy tank AI used in the Tank Battle game.

    The enemy operates using a simple state machine:

        • Patrol State
            Random autonomous movement around the map.

        • Chase State
            Pursues the player when detected.

    Features:

        • Autonomous patrol behavior
        • Player detection system
        • Pursuit behavior
        • Shooting cooldown management
        • Random direction changes
        • Screen boundary avoidance
        • State-based AI

    ============================================================
*/

#include "Enemy.h"

#include <cmath>
#include <cstdlib>

#include <glm.hpp>

/*
    Constructor

    Initializes all AI variables and gameplay
    parameters for the enemy tank.

    Default configuration:

        Detection Range:
            350 pixels

        Shooting Range:
            250 pixels

        Fire Cooldown:
            1.2 seconds

        Movement Speed:
            100 pixels/sec

        Rotation Speed:
            2.5 radians/sec

        Initial State:
            Patrol
*/
Enemy::Enemy()
    : Tank()
{
    //--------------------------------------------------
    // Target position initially set to origin.
    //
    // This value will later be replaced with
    // the player's current position.
    //--------------------------------------------------

    targetPosition =
    {
        0.0f,
        0.0f
    };

    //--------------------------------------------------
    // Maximum distance at which the enemy can
    // detect the player.
    //--------------------------------------------------

    detectionRange =
        350.0f;

    //--------------------------------------------------
    // Maximum distance at which the enemy can
    // fire projectiles.
    //--------------------------------------------------

    shootingRange =
        250.0f;

    //--------------------------------------------------
    // Time required between shots.
    //--------------------------------------------------

    fireCooldown =
        1.2f;

    //--------------------------------------------------
    // Tracks elapsed time since the last shot.
    //--------------------------------------------------

    fireTimer =
        0.0f;

    //--------------------------------------------------
    // Enemy starts in patrol mode.
    //--------------------------------------------------

    state =
        EnemyState::Patrol;

    //--------------------------------------------------
    // Default patrol direction.
    //--------------------------------------------------

    patrolDirection =
    {
        1.0f,
        0.0f
    };

    //--------------------------------------------------
    // Tracks patrol timing.
    //--------------------------------------------------

    patrolTimer =
        0.0f;

    //--------------------------------------------------
    // Duration before selecting a new patrol
    // direction.
    //--------------------------------------------------

    patrolDuration =
        2.0f;

    //--------------------------------------------------
    // Movement configuration inherited from Tank.
    //--------------------------------------------------

    moveSpeed =
        100.0f;

    rotationSpeed =
        2.5f;

    //--------------------------------------------------
    // Select a random initial patrol direction.
    //--------------------------------------------------

    ChooseNewPatrolDirection();
}

//--------------------------------------------------
// Target Management
//
// Responsible for storing and retrieving the
// current player position.
//--------------------------------------------------

void Enemy::SetTargetPosition(
    const glm::vec2& position)
{
    targetPosition =
        position;
}

glm::vec2 Enemy::GetTargetPosition() const
{
    return targetPosition;
}

//--------------------------------------------------
// Detection Configuration
//
// Controls how far the enemy can "see" the player.
//--------------------------------------------------

void Enemy::SetDetectionRange(
    float range)
{
    detectionRange =
        range;
}

float Enemy::GetDetectionRange() const
{
    return detectionRange;
}

//--------------------------------------------------
// Shooting Configuration
//
// Controls the attack distance and fire rate.
//--------------------------------------------------

void Enemy::SetShootingRange(
    float range)
{
    shootingRange =
        range;
}

float Enemy::GetShootingRange() const
{
    return shootingRange;
}

void Enemy::SetFireCooldown(
    float cooldown)
{
    fireCooldown =
        cooldown;
}

float Enemy::GetFireCooldown() const
{
    return fireCooldown;
}

//--------------------------------------------------
// Returns true when enough time has elapsed
// since the last shot.
//--------------------------------------------------

bool Enemy::CanShoot() const
{
    return
        fireTimer >=
        fireCooldown;
}

//--------------------------------------------------
// Resets the firing timer after a shot.
//--------------------------------------------------

void Enemy::ResetFireTimer()
{
    fireTimer = 0.0f;
}

//--------------------------------------------------
// AI Helper Functions
//--------------------------------------------------

/*
    Calculates the Euclidean distance between
    the enemy and the target.
*/
float Enemy::DistanceToTarget() const
{
    return glm::distance(
        position,
        targetPosition);
}

/*
    Returns a normalized direction vector
    pointing from the enemy toward the player.
*/
glm::vec2 Enemy::DirectionToTarget() const
{
    glm::vec2 direction =
        targetPosition -
        position;

    //--------------------------------------------------
    // Prevent normalization of a zero vector.
    //--------------------------------------------------

    if (
        glm::dot(
            direction,
            direction)
        <
        0.0001f)
    {
        return
        {
            0.0f,
            0.0f
        };
    }

    return glm::normalize(
        direction);
}

/*
    Determines whether the player is within
    detection range.
*/
bool Enemy::CanSeeTarget() const
{
    return
        DistanceToTarget()
        <=
        detectionRange;
}

/*
    Returns the current AI state.
*/
EnemyState Enemy::GetState() const
{
    return state;
}

//--------------------------------------------------
// Patrol System
//--------------------------------------------------

/*
    Selects a new random patrol direction.

    Possible directions:

        Right
        Left
        Up
        Down

    A random patrol duration is also selected
    to create less predictable movement.
*/
void Enemy::ChooseNewPatrolDirection()
{
    int random =
        std::rand() % 4;

    switch (random)
    {
    case 0:

        //--------------------------------------------------
        // Move Right
        //--------------------------------------------------

        patrolDirection =
        {
            1.0f,
            0.0f
        };
        break;

    case 1:

        //--------------------------------------------------
        // Move Left
        //--------------------------------------------------

        patrolDirection =
        {
            -1.0f,
            0.0f
        };
        break;

    case 2:

        //--------------------------------------------------
        // Move Up
        //--------------------------------------------------

        patrolDirection =
        {
            0.0f,
            1.0f
        };
        break;

    default:

        //--------------------------------------------------
        // Move Down
        //--------------------------------------------------

        patrolDirection =
        {
            0.0f,
            -1.0f
        };
        break;
    }

    //--------------------------------------------------
    // Random patrol duration between
    // approximately 1.5 and 4.5 seconds.
    //--------------------------------------------------

    patrolDuration =
        1.5f +
        static_cast<float>(
            std::rand() % 300)
        / 100.0f;
}

//--------------------------------------------------
// Main AI Update
//--------------------------------------------------

/*
    Executes enemy behavior every frame.

    Responsibilities:

        • Update cooldown timers
        • Change AI state
        • Patrol
        • Chase player
        • Rotate toward movement direction
        • Stay inside map boundaries
*/
void Enemy::Update(
    float deltaTime)
{
    //--------------------------------------------------
    // Dead enemies do not update.
    //--------------------------------------------------

    if (!alive)
    {
        return;
    }

    //--------------------------------------------------
    // Advance shooting cooldown timer.
    //--------------------------------------------------

    fireTimer +=
        deltaTime;

    //--------------------------------------------------
    // State Transition Logic
    //
    // If the player is visible:
    //      Chase
    //
    // Otherwise:
    //      Patrol
    //--------------------------------------------------

    if (CanSeeTarget())
    {
        state =
            EnemyState::Chase;
    }
    else
    {
        state =
            EnemyState::Patrol;
    }

    //--------------------------------------------------
    // Chase State
    //--------------------------------------------------

    if (
        state ==
        EnemyState::Chase)
    {
        glm::vec2 direction =
            DirectionToTarget();

        //--------------------------------------------------
        // Move only if a valid direction exists.
        //--------------------------------------------------

        if (
            glm::dot(
                direction,
                direction)
        >
            0.0001f)
        {
            //--------------------------------------------------
            // Rotate toward the target.
            //--------------------------------------------------

            rotation =
                std::atan2(
                    direction.y,
                    direction.x);

            //--------------------------------------------------
            // Advance toward the player.
            //--------------------------------------------------

            position +=
                direction *
                moveSpeed *
                deltaTime;
        }

        return;
    }

    //--------------------------------------------------
    // Patrol State
    //--------------------------------------------------

    patrolTimer +=
        deltaTime;

    //--------------------------------------------------
    // Change direction after the patrol
    // duration expires.
    //--------------------------------------------------

    if (
        patrolTimer >=
        patrolDuration)
    {
        patrolTimer =
            0.0f;

        ChooseNewPatrolDirection();
    }

    //--------------------------------------------------
    // Rotate to face the patrol direction.
    //--------------------------------------------------

    rotation =
        std::atan2(
            patrolDirection.y,
            patrolDirection.x);

    //--------------------------------------------------
    // Move in the selected patrol direction.
    //--------------------------------------------------

    position +=
        patrolDirection *
        moveSpeed *
        deltaTime;

    //--------------------------------------------------
    // Map Boundary Handling
    //
    // Prevent enemies from leaving the
    // playable area.
    //
    // When a border is reached, the enemy
    // immediately selects a new direction.
    //--------------------------------------------------

    if (position.x < 32.0f)
    {
        position.x = 32.0f;
        ChooseNewPatrolDirection();
    }

    if (position.x > 1248.0f)
    {
        position.x = 1248.0f;
        ChooseNewPatrolDirection();
    }

    if (position.y < 32.0f)
    {
        position.y = 32.0f;
        ChooseNewPatrolDirection();
    }

    if (position.y > 688.0f)
    {
        position.y = 688.0f;
        ChooseNewPatrolDirection();
    }
}

//--------------------------------------------------
// Destructor
//
// No dynamic memory is allocated directly by
// this class.
//--------------------------------------------------

Enemy::~Enemy()
{
}
