/*
    ============================================================
    Bullet.cpp

    Author: Leonardo Moura
    Date: 6/12/2026

    Description:

    Implements the Bullet class used by both the player
    and enemy tanks.

    Responsibilities:

        • Store bullet position
        • Store movement direction
        • Move using delta time
        • Track ownership (player or enemy)
        • Provide collision bounds
        • Detect when leaving the game area
        • Manage active/inactive state

    ============================================================
*/

#include "Bullet.h"

#include <glm.hpp>

/*
    Constructor

    Initializes a bullet with default values.

    Default State:

        Position:
            (0,0)

        Direction:
            Positive X axis

        Speed:
            500 pixels/second

        Size:
            8x8 pixels

        Owner:
            Player

        State:
            Active
*/
Bullet::Bullet()
{
    position =
    {
        0.0f,
        0.0f
    };

    direction =
    {
        1.0f,
        0.0f
    };

    speed =
        500.0f;

    width =
        8.0f;

    height =
        8.0f;

    fromPlayer =
        true;

    active =
        true;
}

//--------------------------------------------------
// Position Management
//
// Responsible for storing and retrieving the
// bullet world position.
//--------------------------------------------------

void Bullet::SetPosition(
    const glm::vec2& newPosition)
{
    position =
        newPosition;
}

glm::vec2 Bullet::GetPosition() const
{
    return position;
}

//--------------------------------------------------
// Direction Management
//
// The bullet direction determines the trajectory
// used during movement.
//
// Directions are normalized to ensure bullets
// always travel at a constant speed regardless
// of the original vector magnitude.
//--------------------------------------------------

void Bullet::SetDirection(
    const glm::vec2& newDirection)
{
    //--------------------------------------------------
    // Prevent normalization of a near-zero vector.
    //--------------------------------------------------

    if (
        glm::dot(
            newDirection,
            newDirection)
        >
        0.0001f)
    {
        //--------------------------------------------------
        // Store a normalized direction vector.
        //--------------------------------------------------

        direction =
            glm::normalize(
                newDirection);
    }
}

glm::vec2 Bullet::GetDirection() const
{
    return direction;
}

//--------------------------------------------------
// Speed Management
//
// Controls how fast the bullet travels across
// the battlefield.
//--------------------------------------------------

void Bullet::SetSpeed(
    float newSpeed)
{
    speed =
        newSpeed;
}

float Bullet::GetSpeed() const
{
    return speed;
}

//--------------------------------------------------
// Size Management
//
// The bullet size is used for:
//
//      Rendering
//      Collision Detection
//      Bounding Box Calculations
//--------------------------------------------------

void Bullet::SetSize(
    float newWidth,
    float newHeight)
{
    width =
        newWidth;

    height =
        newHeight;
}

float Bullet::GetWidth() const
{
    return width;
}

float Bullet::GetHeight() const
{
    return height;
}

//--------------------------------------------------
// Ownership Management
//
// Identifies who fired the bullet.
//
// true  = Player bullet
// false = Enemy bullet
//
// This information is used to prevent enemies
// from damaging themselves and to determine
// valid collision targets.
//--------------------------------------------------

void Bullet::SetFromPlayer(
    bool value)
{
    fromPlayer =
        value;
}

bool Bullet::IsFromPlayer() const
{
    return fromPlayer;
}

//--------------------------------------------------
// State Management
//
// Active bullets are updated, rendered and
// checked for collisions.
//
// Inactive bullets are removed from the game.
//--------------------------------------------------

bool Bullet::IsActive() const
{
    return active;
}

void Bullet::Deactivate()
{
    active =
        false;
}

//--------------------------------------------------
// Collision Bounds
//
// Returns the minimum corner of the bullet's
// axis-aligned bounding box (AABB).
//
// Used for collision detection.
//--------------------------------------------------

glm::vec2 Bullet::GetMinBounds() const
{
    return
    {
        position.x -
        width * 0.5f,

        position.y -
        height * 0.5f
    };
}

//--------------------------------------------------
// Returns the maximum corner of the bullet's
// axis-aligned bounding box (AABB).
//
// Used for collision detection.
//--------------------------------------------------

glm::vec2 Bullet::GetMaxBounds() const
{
    return
    {
        position.x +
        width * 0.5f,

        position.y +
        height * 0.5f
    };
}

//--------------------------------------------------
// Bullet Update
//
// Moves the projectile according to:
//
//      Position +=
//          Direction *
//          Speed *
//          DeltaTime
//
// Delta time ensures frame-rate independent
// movement.
//--------------------------------------------------

void Bullet::Update(
    float deltaTime)
{
    position +=
        direction *
        speed *
        deltaTime;
}

//--------------------------------------------------
// Screen Boundary Test
//
// Determines whether the bullet has left the
// playable area.
//
// Bullets outside the game window are removed
// to avoid unnecessary processing.
//--------------------------------------------------

bool Bullet::IsOutsideScreen(
    int screenWidth,
    int screenHeight) const
{
    //--------------------------------------------------
    // Left border.
    //--------------------------------------------------

    if (position.x < 0.0f)
        return true;

    //--------------------------------------------------
    // Bottom border.
    //--------------------------------------------------

    if (position.y < 0.0f)
        return true;

    //--------------------------------------------------
    // Right border.
    //--------------------------------------------------

    if (
        position.x >
        static_cast<float>(
            screenWidth))
    {
        return true;
    }

    //--------------------------------------------------
    // Top border.
    //--------------------------------------------------

    if (
        position.y >
        static_cast<float>(
            screenHeight))
    {
        return true;
    }

    //--------------------------------------------------
    // Bullet remains inside the playable area.
    //--------------------------------------------------

    return false;
}

//--------------------------------------------------
// Destructor
//
// No dynamic memory is allocated by this class.
//--------------------------------------------------

Bullet::~Bullet()
{
}
