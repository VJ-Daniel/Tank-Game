/*
    ============================================================
    Tank.h

    Author: Leonardo Moura
    Date: 6/12/2026

    Description:

    Defines the Tank base class used by all tank
    entities in the Tank Battle game.

    Responsibilities:

        • Store world position
        • Store rotation angle
        • Handle movement
        • Handle rotation
        • Manage collision dimensions
        • Track life state
        • Provide collision bounds

    Inheritance Hierarchy:

                Tank
                  |
        ---------------------
        |                   |
      Player            Enemy

    Coordinate System:

        Position:
            World-space coordinates

        Rotation:
            Stored in radians

        Direction:
            Calculated using cosine and sine

    Movement Model:

        Forward Vector
                ↓
        Movement Speed
                ↓
           Delta Time
                ↓
           New Position

    ============================================================
*/

#ifndef TANK_H
#define TANK_H

#include <glm.hpp>

class Tank
{
protected:

    //--------------------------------------------------
    // Transform
    //
    // position
    //      Current world position.
    //
    // rotation
    //      Current tank orientation.
    //
    // Rotation is stored in radians.
    //--------------------------------------------------

    glm::vec2 position;

    float rotation;

    //--------------------------------------------------
    // Movement Configuration
    //
    // moveSpeed
    //      Forward and backward movement speed.
    //
    // rotationSpeed
    //      Turning speed.
    //
    // Units:
    //
    //      Pixels per second
    //      Radians per second
    //--------------------------------------------------

    float moveSpeed;

    float rotationSpeed;

    //--------------------------------------------------
    // Collision Dimensions
    //
    // Defines the size of the tank's
    // collision box.
    //
    // Used for:
    //
    //      • Collision Detection
    //      • Rendering
    //      • Bounds Calculations
    //--------------------------------------------------

    float width;

    float height;

    //--------------------------------------------------
    // Life State
    //
    // true
    //      Tank is active.
    //
    // false
    //      Tank has been destroyed.
    //
    // Destroyed tanks are ignored by
    // gameplay systems and rendering.
    //--------------------------------------------------

    bool alive;

public:

    //--------------------------------------------------
    // Constructor
    //
    // Creates a tank with default values.
    //
    // Default Properties:
    //
    //      Position:
    //          (0,0)
    //
    //      Rotation:
    //          0 radians
    //
    //      Size:
    //          32x32
    //
    //      Alive:
    //          true
    //--------------------------------------------------

    Tank();

    //--------------------------------------------------
    // Position Management
    //
    // Controls the tank's location in
    // world space.
    //--------------------------------------------------

    void SetPosition(
        const glm::vec2& newPosition);

    glm::vec2 GetPosition() const;

    //--------------------------------------------------
    // Rotation Management
    //
    // Controls tank orientation.
    //
    // Rotation Values:
    //
    //      0
    //          Right
    //
    //      PI / 2
    //          Up
    //
    //      PI
    //          Left
    //
    //      3PI / 2
    //          Down
    //--------------------------------------------------

    void SetRotation(
        float angle);

    float GetRotation() const;

    //--------------------------------------------------
    // Movement Functions
    //
    // MoveForward()
    //      Moves in the forward direction.
    //
    // MoveBackward()
    //      Moves opposite the forward direction.
    //
    // RotateLeft()
    //      Counter-clockwise rotation.
    //
    // RotateRight()
    //      Clockwise rotation.
    //
    // GetForwardVector()
    //      Returns a normalized direction
    //      vector based on current rotation.
    //--------------------------------------------------

    void MoveForward(
        float deltaTime);

    void MoveBackward(
        float deltaTime);

    void RotateLeft(
        float deltaTime);

    void RotateRight(
        float deltaTime);

    glm::vec2 GetForwardVector() const;

    //--------------------------------------------------
    // Speed Configuration
    //
    // Controls movement and rotation rates.
    //--------------------------------------------------

    void SetMoveSpeed(
        float speed);

    float GetMoveSpeed() const;

    void SetRotationSpeed(
        float speed);

    float GetRotationSpeed() const;

    //--------------------------------------------------
    // Size Configuration
    //
    // Controls tank dimensions.
    //
    // Used for:
    //
    //      • Rendering
    //      • Collision Detection
    //--------------------------------------------------

    void SetSize(
        float tankWidth,
        float tankHeight);

    float GetWidth() const;

    float GetHeight() const;

    //--------------------------------------------------
    // Collision Bounds
    //
    // Returns the minimum and maximum corners
    // of the tank's Axis-Aligned Bounding Box
    // (AABB).
    //
    // Used by collision detection systems.
    //--------------------------------------------------

    glm::vec2 GetMinBounds() const;

    glm::vec2 GetMaxBounds() const;

    //--------------------------------------------------
    // Life State Management
    //
    // IsAlive()
    //      Returns whether the tank is active.
    //
    // Destroy()
    //      Marks the tank as destroyed.
    //--------------------------------------------------

    bool IsAlive() const;

    void Destroy();

    //--------------------------------------------------
    // Update
    //
    // Base per-frame update function.
    //
    // Intended to be overridden by derived
    // classes such as Enemy.
    //
    // Parameter:
    //
    //      deltaTime
    //          Time elapsed since the previous
    //          frame.
    //--------------------------------------------------

    virtual void Update(
        float deltaTime);

    //--------------------------------------------------
    // Destructor
    //
    // Virtual destructor ensures proper cleanup
    // when deleting derived classes through
    // a Tank pointer.
    //--------------------------------------------------

    virtual ~Tank();
};

#endif