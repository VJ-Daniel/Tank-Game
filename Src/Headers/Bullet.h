/*
    ============================================================
    Bullet.h

    Author: Leonardo Moura
    Date: 6/12/2026

    Description:

    Defines the Bullet class used by both player
    and enemy tanks.

    Responsibilities:

        • Store projectile position
        • Store projectile direction
        • Handle projectile movement
        • Track projectile ownership
        • Provide collision bounds
        • Track active state
        • Detect screen exits

    Gameplay Usage:

        Player Tank
              ↓
           Bullet

        Enemy Tank
              ↓
           Bullet

    Bullet Lifecycle:

        Spawned
            ↓

        Moving
            ↓

        Collision
            OR
        Leaves Screen
            ↓

        Deactivated
            ↓

        Removed by Game Cleanup

    ============================================================
*/

#ifndef BULLET_H
#define BULLET_H

#include <glm.hpp>

class Bullet
{
private:

    //--------------------------------------------------
    // Transform
    //
    // Stores the bullet's position and
    // movement direction in world space.
    //--------------------------------------------------

    glm::vec2 position;

    glm::vec2 direction;

    //--------------------------------------------------
    // Movement
    //
    // Controls projectile speed.
    //
    // Unit:
    //      Pixels per second
    //--------------------------------------------------

    float speed;

    //--------------------------------------------------
    // Size
    //
    // Used for:
    //
    //      Rendering
    //      Collision Detection
    //      Bounding Box Calculations
    //--------------------------------------------------

    float width;

    float height;

    //--------------------------------------------------
    // Ownership
    //
    // Identifies who fired the projectile.
    //
    // true
    //      Player bullet
    //
    // false
    //      Enemy bullet
    //
    // Used to determine valid collision
    // targets.
    //--------------------------------------------------

    bool fromPlayer;

    //--------------------------------------------------
    // State
    //
    // Active bullets participate in:
    //
    //      Rendering
    //      Updates
    //      Collision Detection
    //
    // Inactive bullets are removed by
    // the Game class.
    //--------------------------------------------------

    bool active;

public:

    //--------------------------------------------------
    // Constructor
    //
    // Creates a bullet with default values.
    //--------------------------------------------------

    Bullet();

    //--------------------------------------------------
    // Position Management
    //
    // Controls the bullet's world position.
    //--------------------------------------------------

    void SetPosition(
        const glm::vec2& newPosition);

    glm::vec2 GetPosition() const;

    //--------------------------------------------------
    // Direction Management
    //
    // Controls the bullet's movement direction.
    //
    // Direction vectors are normalized before
    // movement calculations.
    //--------------------------------------------------

    void SetDirection(
        const glm::vec2& newDirection);

    glm::vec2 GetDirection() const;

    //--------------------------------------------------
    // Speed Management
    //
    // Controls how fast the projectile
    // travels through the battlefield.
    //--------------------------------------------------

    void SetSpeed(
        float newSpeed);

    float GetSpeed() const;

    //--------------------------------------------------
    // Size Management
    //
    // Controls projectile dimensions.
    //--------------------------------------------------

    void SetSize(
        float newWidth,
        float newHeight);

    float GetWidth() const;

    float GetHeight() const;

    //--------------------------------------------------
    // Ownership Management
    //
    // Identifies the bullet owner.
    //--------------------------------------------------

    void SetFromPlayer(
        bool value);

    bool IsFromPlayer() const;

    //--------------------------------------------------
    // State Management
    //
    // Used to activate or deactivate
    // projectiles during gameplay.
    //--------------------------------------------------

    bool IsActive() const;

    void Deactivate();

    //--------------------------------------------------
    // Collision Bounds
    //
    // Returns the minimum and maximum corners
    // of the bullet's Axis-Aligned Bounding
    // Box (AABB).
    //
    // Used by collision detection.
    //--------------------------------------------------

    glm::vec2 GetMinBounds() const;

    glm::vec2 GetMaxBounds() const;

    //--------------------------------------------------
    // Update
    //
    // Advances projectile movement using:
    //
    //      Position +=
    //          Direction *
    //          Speed *
    //          DeltaTime
    //--------------------------------------------------

    void Update(
        float deltaTime);

    //--------------------------------------------------
    // Screen Boundary Test
    //
    // Determines whether the projectile
    // has left the playable area.
    //
    // Returns:
    //
    //      true
    //          Outside screen
    //
    //      false
    //          Inside screen
    //--------------------------------------------------

    bool IsOutsideScreen(
        int screenWidth,
        int screenHeight) const;

    //--------------------------------------------------
    // Destructor
    //
    // Releases resources owned by the bullet.
    //
    // Current implementation does not allocate
    // dynamic memory.
    //--------------------------------------------------

    ~Bullet();
};

#endif