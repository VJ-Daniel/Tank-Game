/*
    ============================================================
    Enemy.h

    Author: Leonardo Moura
    Date: 6/12/2026

    Description:

    Defines the Enemy tank AI used in the Tank Battle game.

    The Enemy class extends the Tank base class and
    introduces autonomous behavior.

    AI Features:

        • Patrol behavior
        • Player detection
        • Target pursuit
        • Shooting cooldown management
        • State machine logic
        • Direction tracking

    State Machine:

        Patrol
            ↓
        Player Detected
            ↓
        Chase
            ↓
        Player Lost
            ↓
        Patrol

    Inheritance:

        Tank
          ↓
        Enemy

    ============================================================
*/

#ifndef ENEMY_H
#define ENEMY_H

#include "Tank.h"

#include <glm.hpp>

/*
    Enemy AI States

    Patrol
        Enemy wanders around the map.

    Chase
        Enemy actively pursues the player.
*/
enum class EnemyState
{
    Patrol,
    Chase
};

class Enemy : public Tank
{
private:

    //--------------------------------------------------
    // Target Tracking
    //
    // Stores the current player position.
    //
    // Updated every frame by the Game class.
    //--------------------------------------------------

    glm::vec2 targetPosition;

    //--------------------------------------------------
    // Detection System
    //
    // detectionRange
    //      Maximum distance at which the enemy
    //      can detect the player.
    //
    // shootingRange
    //      Maximum distance at which the enemy
    //      can fire projectiles.
    //--------------------------------------------------

    float detectionRange;

    float shootingRange;

    //--------------------------------------------------
    // Shooting System
    //
    // fireCooldown
    //      Minimum time between shots.
    //
    // fireTimer
    //      Tracks elapsed time since the
    //      previous shot.
    //--------------------------------------------------

    float fireCooldown;

    float fireTimer;

    //--------------------------------------------------
    // State Machine
    //
    // Determines current enemy behavior.
    //--------------------------------------------------

    EnemyState state;

    //--------------------------------------------------
    // Patrol System
    //
    // patrolDirection
    //      Current movement direction.
    //
    // patrolTimer
    //      Time spent moving in the current
    //      patrol direction.
    //
    // patrolDuration
    //      Time before selecting a new
    //      patrol direction.
    //--------------------------------------------------

    glm::vec2 patrolDirection;

    float patrolTimer;

    float patrolDuration;

public:

    //--------------------------------------------------
    // Constructor
    //
    // Creates an enemy tank with default
    // AI configuration.
    //--------------------------------------------------

    Enemy();

    //--------------------------------------------------
    // Target Management
    //
    // Controls the position that the enemy
    // is currently tracking.
    //--------------------------------------------------

    void SetTargetPosition(
        const glm::vec2& position);

    glm::vec2 GetTargetPosition() const;

    //--------------------------------------------------
    // Detection Configuration
    //
    // Controls how far the enemy can detect
    // the player.
    //--------------------------------------------------

    void SetDetectionRange(
        float range);

    float GetDetectionRange() const;

    //--------------------------------------------------
    // Shooting Configuration
    //
    // Controls attack distance.
    //--------------------------------------------------

    void SetShootingRange(
        float range);

    float GetShootingRange() const;

    //--------------------------------------------------
    // Fire Rate Configuration
    //
    // Controls the delay between shots.
    //--------------------------------------------------

    void SetFireCooldown(
        float cooldown);

    float GetFireCooldown() const;

    //--------------------------------------------------
    // Shooting Queries
    //
    // CanShoot()
    //      Returns true when the cooldown
    //      period has elapsed.
    //
    // ResetFireTimer()
    //      Resets the cooldown after firing.
    //--------------------------------------------------

    bool CanShoot() const;

    void ResetFireTimer();

    //--------------------------------------------------
    // Target Queries
    //
    // DistanceToTarget()
    //      Returns distance to player.
    //
    // DirectionToTarget()
    //      Returns normalized direction
    //      toward player.
    //
    // CanSeeTarget()
    //      Returns true if the player is
    //      inside detection range.
    //--------------------------------------------------

    float DistanceToTarget() const;

    glm::vec2 DirectionToTarget() const;

    bool CanSeeTarget() const;

    //--------------------------------------------------
    // State Query
    //
    // Returns the current AI state.
    //--------------------------------------------------

    EnemyState GetState() const;

    //--------------------------------------------------
    // Patrol Behavior
    //
    // Selects a new random patrol direction.
    //--------------------------------------------------

    void ChooseNewPatrolDirection();

    //--------------------------------------------------
    // AI Update
    //
    // Executes enemy behavior every frame.
    //
    // Responsibilities:
    //
    //      • State transitions
    //      • Patrol movement
    //      • Player pursuit
    //      • Cooldown updates
    //      • Direction updates
    //--------------------------------------------------

    virtual void Update(
        float deltaTime) override;

    //--------------------------------------------------
    // Destructor
    //
    // Releases resources owned by the enemy.
    //
    // No dynamic memory is currently allocated.
    //--------------------------------------------------

    virtual ~Enemy();
};

#endif