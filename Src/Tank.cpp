/*
    ============================================================
    Tank.cpp

    Author: Leonardo Moura
    Date: 6/12/2026

    Description:

    Implements the Tank base class used by both
    the player and enemy tanks.

    Responsibilities:

        • Store world position
        • Store rotation angle
        • Handle movement
        • Handle rotation
        • Manage speed values
        • Provide collision bounds
        • Track life state

    Inheritance:

        Tank
          ├── Player Tank
          └── Enemy Tank

    Coordinate System:

        Rotation:
            Stored in radians

        Forward Direction:
            Calculated using cosine and sine

        Movement:
            Direction-based

    ============================================================
*/

#include "Tank.h"

#include <cmath>

/*
    Constructor

    Initializes default tank properties.

    Default Configuration:

        Position:
            (0,0)

        Rotation:
            0 radians

        Movement Speed:
            220 pixels/sec

        Rotation Speed:
            3 radians/sec

        Size:
            32x32

        State:
            Alive
*/
Tank::Tank()
{
    //--------------------------------------------------
    // Initial world position.
    //--------------------------------------------------

    position =
    {
        0.0f,
        0.0f
    };

    //--------------------------------------------------
    // Initial rotation.
    //
    // 0 radians points toward the
    // positive X axis.
    //--------------------------------------------------

    rotation =
        0.0f;

    //--------------------------------------------------
    // Default movement speed.
    //--------------------------------------------------

    moveSpeed =
        220.0f;

    //--------------------------------------------------
    // Default turning speed.
    //--------------------------------------------------

    rotationSpeed =
        3.0f;

    //--------------------------------------------------
    // Default tank dimensions.
    //--------------------------------------------------

    width =
        32.0f;

    height =
        32.0f;

    //--------------------------------------------------
    // Tank starts alive.
    //--------------------------------------------------

    alive =
        true;
}

//--------------------------------------------------
// Position Management
//
// Responsible for storing and retrieving
// the tank world position.
//--------------------------------------------------

void Tank::SetPosition(
    const glm::vec2& newPosition)
{
    position =
        newPosition;
}

glm::vec2 Tank::GetPosition() const
{
    return position;
}

//--------------------------------------------------
// Rotation Management
//
// Rotation is stored in radians.
//
// Examples:
//
//      0          -> Right
//      PI / 2     -> Up
//      PI         -> Left
//      3PI / 2    -> Down
//--------------------------------------------------

void Tank::SetRotation(
    float angle)
{
    rotation =
        angle;
}

float Tank::GetRotation() const
{
    return rotation;
}

//--------------------------------------------------
// Movement Functions
//--------------------------------------------------

/*
    Calculates the current forward direction.

    Uses the tank rotation angle to generate
    a unit direction vector.

    Formula:

        x = cos(rotation)
        y = sin(rotation)
*/
glm::vec2 Tank::GetForwardVector() const
{
    return
    {
        std::cos(rotation),
        std::sin(rotation)
    };
}

/*
    Moves the tank forward.

    Movement Formula:

        Position +=
            Direction *
            Speed *
            DeltaTime

    Delta time ensures frame-rate independent
    movement.
*/
void Tank::MoveForward(
    float deltaTime)
{
    position +=
        GetForwardVector()
        *
        moveSpeed
        *
        deltaTime;
}

/*
    Moves the tank backward.

    Movement Formula:

        Position -=
            Direction *
            Speed *
            DeltaTime
*/
void Tank::MoveBackward(
    float deltaTime)
{
    position -=
        GetForwardVector()
        *
        moveSpeed
        *
        deltaTime;
}

/*
    Rotates the tank counter-clockwise.

    Positive rotation direction.
*/
void Tank::RotateLeft(
    float deltaTime)
{
    rotation +=
        rotationSpeed
        *
        deltaTime;
}

/*
    Rotates the tank clockwise.

    Negative rotation direction.
*/
void Tank::RotateRight(
    float deltaTime)
{
    rotation -=
        rotationSpeed
        *
        deltaTime;
}

//--------------------------------------------------
// Speed Configuration
//
// Controls movement and turning rates.
//--------------------------------------------------

void Tank::SetMoveSpeed(
    float speed)
{
    moveSpeed =
        speed;
}

float Tank::GetMoveSpeed() const
{
    return moveSpeed;
}

void Tank::SetRotationSpeed(
    float speed)
{
    rotationSpeed =
        speed;
}

float Tank::GetRotationSpeed() const
{
    return rotationSpeed;
}

//--------------------------------------------------
// Size Configuration
//
// Used for:
//
//      Rendering
//      Collision Detection
//      Bounding Box Calculations
//--------------------------------------------------

void Tank::SetSize(
    float tankWidth,
    float tankHeight)
{
    width =
        tankWidth;

    height =
        tankHeight;
}

float Tank::GetWidth() const
{
    return width;
}

float Tank::GetHeight() const
{
    return height;
}

//--------------------------------------------------
// Collision Bounds
//
// Provides an Axis-Aligned Bounding Box
// (AABB) used by collision detection.
//
// Note:
//
// The bounding box does not rotate with
// the tank. It remains axis-aligned for
// simplicity and performance.
//--------------------------------------------------

/*
    Returns the bottom-left corner of
    the collision box.
*/
glm::vec2 Tank::GetMinBounds() const
{
    return
    {
        position.x -
        width * 0.5f,

        position.y -
        height * 0.5f
    };
}

/*
    Returns the top-right corner of
    the collision box.
*/
glm::vec2 Tank::GetMaxBounds() const
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
// Life State Management
//
// Determines whether the tank is active
// in the game world.
//--------------------------------------------------

bool Tank::IsAlive() const
{
    return alive;
}

/*
    Marks the tank as destroyed.

    Destroyed tanks are ignored by:

        • Rendering
        • Movement
        • AI Updates
        • Gameplay Logic

    The Game class later removes them
    from active collections.
*/
void Tank::Destroy()
{
    alive =
        false;
}

//--------------------------------------------------
// Update
//
// Base implementation.
//
// Intended to be overridden by derived
// classes such as Enemy.
//
// Player tanks currently do not require
// per-frame logic here.
//--------------------------------------------------

void Tank::Update(
    float deltaTime)
{
    (void)deltaTime;
}

//--------------------------------------------------
// Destructor
//
// No dynamic memory is owned directly
// by the Tank class.
//--------------------------------------------------

Tank::~Tank()
{
}