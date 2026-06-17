/*
    ============================================================
    Game.cpp

    Author: Leonardo Moura
    Date: 6/12/2026

    Description:

    Implements the central game controller.

    The Game class coordinates all gameplay systems:

        • Player management
        • Enemy management
        • Projectile management
        • Collision detection
        • Map generation
        • Input processing
        • Rendering
        • Object cleanup

    This file acts as the bridge between the
    gameplay systems and the rendering engine.

    ============================================================
*/

#include "Game.h"

#include <algorithm>

/*
    Constructor

    Stores references to the game window and
    screen dimensions.

    These values are later used for:

        • Input processing
        • Boundary checks
        • Rendering limits
*/
Game::Game(
    GLFWwindow* window,
    int width,
    int height)
{
    this->window =
        window;

    screenWidth =
        width;

    screenHeight =
        height;
}

//--------------------------------------------------
// Game Initialization
//
// Initializes all major systems:
//
//      Renderer
//      Map
//      Player
//      Enemies
//
// Returns true when initialization succeeds.
//--------------------------------------------------

bool Game::Initialize(
    Shader* shader)
{
    //--------------------------------------------------
    // Initialize rendering subsystem.
    //--------------------------------------------------

    if (!renderer.Initialize())
    {
        return false;
    }

    //--------------------------------------------------
    // Attach shader program to renderer.
    //--------------------------------------------------

    renderer.SetShader(
        shader);

    //--------------------------------------------------
    // Generate a procedural map.
    //--------------------------------------------------

    // load level 1
    //map.Generate();
    //SpawnPlayer();
    //SpawnEnemies();
    
    
    // load level 2
    //LoadLevel2();

    // load level 3
    LoadLevel3();
    return true;
}

//--------------------------------------------------
// Player Spawn
//
// Creates the player tank using values
// provided by the map.
//
// Initial configuration:
//
//      Position
//      Rotation
//      Speed
//      Size
//--------------------------------------------------

void Game::SpawnPlayer()
{
    player.SetPosition(
        map.GetPlayerSpawn());

    player.SetRotation(
        0.0f);

    player.SetMoveSpeed(
        220.0f);

    player.SetRotationSpeed(
        3.0f);

    player.SetSize(
        32.0f,
        32.0f);
}

//--------------------------------------------------
// Enemy Spawn
//
// Creates all enemy tanks and places them
// at predefined spawn positions.
//
// Current level configuration:
//
//      4 Enemy Tanks
//--------------------------------------------------

void Game::SpawnEnemies()
{
    enemies.clear();

    const int ENEMY_COUNT = 4;

    for (int i = 0;
        i < ENEMY_COUNT;
        i++)
    {
        Enemy enemy;

        enemy.SetPosition(
            map.GetEnemySpawn(
                i));

        enemy.SetMoveSpeed(
            120.0f);

        enemy.SetSize(
            32.0f,
            32.0f);

        enemies.push_back(
            enemy);
    }
}

//--------------------------------------------------
// Input Processing
//
// Delegates player input handling.
//--------------------------------------------------

void Game::ProcessInput(
    float deltaTime)
{
    HandlePlayerInput(
        deltaTime);
}

//--------------------------------------------------
// Player Input
//
// Handles:
//
//      W / S
//          Forward / Backward
//
//      A / D
//          Rotation
//
//      Arrow Keys
//          Alternative controls
//
//      Space
//          Fire projectile
//--------------------------------------------------

void Game::HandlePlayerInput(
    float deltaTime)
{
    //--------------------------------------------------
    // Dead players cannot move.
    //--------------------------------------------------

    if (!player.IsAlive())
    {
        return;
    }

    //--------------------------------------------------
    // Rotate Left
    //--------------------------------------------------

    if (
        glfwGetKey(
            window,
            GLFW_KEY_A) ==
        GLFW_PRESS ||

        glfwGetKey(
            window,
            GLFW_KEY_LEFT) ==
        GLFW_PRESS)
    {
        player.RotateLeft(
            deltaTime);
    }

    //--------------------------------------------------
    // Rotate Right
    //--------------------------------------------------

    if (
        glfwGetKey(
            window,
            GLFW_KEY_D) ==
        GLFW_PRESS ||

        glfwGetKey(
            window,
            GLFW_KEY_RIGHT) ==
        GLFW_PRESS)
    {
        player.RotateRight(
            deltaTime);
    }

    glm::vec2 oldPosition =
        player.GetPosition();

    //--------------------------------------------------
    // Forward Movement
    //--------------------------------------------------

    if (
        glfwGetKey(
            window,
            GLFW_KEY_W) ==
        GLFW_PRESS ||

        glfwGetKey(
            window,
            GLFW_KEY_UP) ==
        GLFW_PRESS)
    {
        player.MoveForward(
            deltaTime);

        //--------------------------------------------------
        // Revert movement if collision occurs.
        //--------------------------------------------------

        if (
            map.CheckTankCollision(
                player.GetPosition(),
                player.GetWidth(),
                player.GetHeight()))
        {
            player.SetPosition(
                oldPosition);
        }
    }

    //--------------------------------------------------
    // Backward Movement
    //--------------------------------------------------

    oldPosition =
        player.GetPosition();

    if (
        glfwGetKey(
            window,
            GLFW_KEY_S) ==
        GLFW_PRESS ||

        glfwGetKey(
            window,
            GLFW_KEY_DOWN) ==
        GLFW_PRESS)
    {
        player.MoveBackward(
            deltaTime);

        if (
            map.CheckTankCollision(
                player.GetPosition(),
                player.GetWidth(),
                player.GetHeight()))
        {
            player.SetPosition(
                oldPosition);
        }
    }

    //--------------------------------------------------
    // Fire projectile once per key press.
    //--------------------------------------------------

    static bool previousSpace =
        false;

    bool currentSpace =
        glfwGetKey(
            window,
            GLFW_KEY_SPACE) ==
        GLFW_PRESS;

    if (
        currentSpace &&
        !previousSpace)
    {
        Bullet bullet;

        //--------------------------------------------------
        // Spawn bullet at player position.
        //--------------------------------------------------

        bullet.SetPosition(
            player.GetPosition());

        //--------------------------------------------------
        // Shoot in the direction the tank
        // is currently facing.
        //--------------------------------------------------

        bullet.SetDirection(
            player.GetForwardVector());

        bullet.SetSpeed(
            500.0f);

        bullet.SetFromPlayer(
            true);

        bullets.push_back(
            bullet);
    }

    previousSpace =
        currentSpace;
}

//--------------------------------------------------
// Main Game Update
//
// Updates all active systems every frame.
//--------------------------------------------------

void Game::Update(
    float deltaTime)
{
    UpdatePlayer(
        deltaTime);

    UpdateEnemies(
        deltaTime);

    UpdateBullets(
        deltaTime);

    CheckCollisions();

    RemoveDestroyedObjects();
}

//--------------------------------------------------
// Player Update
//
// Updates player logic and prevents the
// tank from leaving the playable area.
//--------------------------------------------------

void Game::UpdatePlayer(
    float deltaTime)
{
    player.Update(
        deltaTime);

    glm::vec2 position =
        player.GetPosition();

    //--------------------------------------------------
    // Clamp player position to screen bounds.
    //--------------------------------------------------

    position.x =
        std::clamp(
            position.x,
            16.0f,
            static_cast<float>(
                screenWidth - 16));

    position.y =
        std::clamp(
            position.y,
            16.0f,
            static_cast<float>(
                screenHeight - 16));

    player.SetPosition(
        position);
}

//--------------------------------------------------
// Enemy Update
//
// Responsibilities:
//
//      AI updates
//      Wall avoidance
//      Boundary handling
//      Shooting behavior
//--------------------------------------------------

void Game::UpdateEnemies(
    float deltaTime)
{
    for (Enemy& enemy : enemies)
    {
        if (!enemy.IsAlive())
        {
            continue;
        }

        //--------------------------------------------------
        // Continuously track player position.
        //--------------------------------------------------

        enemy.SetTargetPosition(
            player.GetPosition());

        glm::vec2 oldPosition =
            enemy.GetPosition();

        //--------------------------------------------------
        // Execute AI behavior.
        //--------------------------------------------------

        enemy.Update(
            deltaTime);

        //--------------------------------------------------
        // Prevent enemies from moving through
        // map obstacles.
        //--------------------------------------------------

        if (
            map.CheckTankCollision(
                enemy.GetPosition(),
                enemy.GetWidth(),
                enemy.GetHeight()))
        {
            enemy.SetPosition(
                oldPosition);

            enemy.ChooseNewPatrolDirection();
        }

        //--------------------------------------------------
        // Screen boundary handling.
        //--------------------------------------------------

        glm::vec2 position =
            enemy.GetPosition();

        bool hitBorder =
            false;

        if (position.x < 16.0f)
        {
            position.x = 16.0f;
            hitBorder = true;
        }

        if (position.x >
            static_cast<float>(
                screenWidth - 16))
        {
            position.x =
                static_cast<float>(
                    screenWidth - 16);

            hitBorder = true;
        }

        if (position.y < 16.0f)
        {
            position.y = 16.0f;
            hitBorder = true;
        }

        if (position.y >
            static_cast<float>(
                screenHeight - 16))
        {
            position.y =
                static_cast<float>(
                    screenHeight - 16);

            hitBorder = true;
        }

        enemy.SetPosition(
            position);

        //--------------------------------------------------
        // Change patrol direction if a border
        // is reached.
        //--------------------------------------------------

        if (hitBorder)
        {
            enemy.ChooseNewPatrolDirection();
        }

        //--------------------------------------------------
        // Enemy Shooting Logic
        //--------------------------------------------------

        if (
            enemy.CanSeeTarget() &&
            enemy.DistanceToTarget()
            <= enemy.GetShootingRange())
        {
            if (
                enemy.CanShoot())
            {
                Bullet bullet;

                bullet.SetPosition(
                    enemy.GetPosition());

                bullet.SetDirection(
                    enemy.DirectionToTarget());

                bullet.SetSpeed(
                    400.0f);

                bullet.SetFromPlayer(
                    false);

                bullets.push_back(
                    bullet);

                enemy.ResetFireTimer();
            }
        }
    }
}

//--------------------------------------------------
// Bullet Update
//
// Updates all active projectiles.
//
// Responsibilities:
//
//      Movement
//      Boundary checks
//      Block collisions
//      Block destruction
//--------------------------------------------------

void Game::UpdateBullets(
    float deltaTime)
{
    for (Bullet& bullet : bullets)
    {
        //--------------------------------------------------
        // Skip inactive projectiles.
        //--------------------------------------------------

        if (!bullet.IsActive())
        {
            continue;
        }

        //--------------------------------------------------
        // Move projectile using its direction,
        // speed and delta time.
        //--------------------------------------------------

        bullet.Update(
            deltaTime);

        //--------------------------------------------------
        // Remove bullets that leave the
        // playable area.
        //--------------------------------------------------

        if (
            bullet.IsOutsideScreen(
                screenWidth,
                screenHeight))
        {
            bullet.Deactivate();
            continue;
        }

        //--------------------------------------------------
        // Check collision against map blocks.
        //--------------------------------------------------

        if (
            map.IsBlocked(
                bullet.GetPosition()))
        {
            //--------------------------------------------------
            // Damage breakable blocks.
            //
            // Steel blocks remain intact.
            //--------------------------------------------------

            map.DamageTile(
                bullet.GetPosition());

            //--------------------------------------------------
            // Destroy projectile after impact.
            //--------------------------------------------------

            bullet.Deactivate();
        }
    }
}

//--------------------------------------------------
// Collision Detection
//
// Handles:
//
//      Player bullets -> Enemies
//
//      Enemy bullets -> Player
//
// Uses Axis-Aligned Bounding Box (AABB)
// collision detection.
//--------------------------------------------------

void Game::CheckCollisions()
{
    //--------------------------------------------------
    // Check every active projectile.
    //--------------------------------------------------

    for (Bullet& bullet : bullets)
    {
        if (!bullet.IsActive())
        {
            continue;
        }

        //--------------------------------------------------
        // Player Bullet vs Enemy
        //--------------------------------------------------

        if (bullet.IsFromPlayer())
        {
            for (Enemy& enemy : enemies)
            {
                //--------------------------------------------------
                // Ignore dead enemies.
                //--------------------------------------------------

                if (!enemy.IsAlive())
                {
                    continue;
                }

                //--------------------------------------------------
                // Retrieve bullet bounding box.
                //--------------------------------------------------

                glm::vec2 bulletMin =
                    bullet.GetMinBounds();

                glm::vec2 bulletMax =
                    bullet.GetMaxBounds();

                //--------------------------------------------------
                // Retrieve enemy bounding box.
                //--------------------------------------------------

                glm::vec2 enemyMin =
                    enemy.GetMinBounds();

                glm::vec2 enemyMax =
                    enemy.GetMaxBounds();

                //--------------------------------------------------
                // Perform AABB overlap test.
                //--------------------------------------------------

                bool overlap =
                    bulletMin.x < enemyMax.x &&
                    bulletMax.x > enemyMin.x &&
                    bulletMin.y < enemyMax.y &&
                    bulletMax.y > enemyMin.y;

                //--------------------------------------------------
                // Destroy enemy if hit.
                //--------------------------------------------------

                if (overlap)
                {
                    enemy.Destroy();

                    bullet.Deactivate();

                    break;
                }
            }
        }

        //--------------------------------------------------
        // Enemy Bullet vs Player
        //--------------------------------------------------

        else
        {
            //--------------------------------------------------
            // Retrieve bullet bounds.
            //--------------------------------------------------

            glm::vec2 bulletMin =
                bullet.GetMinBounds();

            glm::vec2 bulletMax =
                bullet.GetMaxBounds();

            //--------------------------------------------------
            // Retrieve player bounds.
            //--------------------------------------------------

            glm::vec2 playerMin =
                player.GetMinBounds();

            glm::vec2 playerMax =
                player.GetMaxBounds();

            //--------------------------------------------------
            // Perform overlap test.
            //--------------------------------------------------

            bool overlap =
                bulletMin.x < playerMax.x &&
                bulletMax.x > playerMin.x &&
                bulletMin.y < playerMax.y &&
                bulletMax.y > playerMin.y;

            //--------------------------------------------------
            // Destroy player if hit.
            //--------------------------------------------------

            if (overlap)
            {
                player.Destroy();

                bullet.Deactivate();
            }
        }
    }
}

//--------------------------------------------------
// Object Cleanup
//
// Removes destroyed objects from the game.
//
// This prevents inactive entities from
// consuming memory and processing time.
//
// Objects Removed:
//
//      Destroyed Bullets
//      Destroyed Enemies
//--------------------------------------------------

void Game::RemoveDestroyedObjects()
{
    //--------------------------------------------------
    // Remove inactive bullets.
    //--------------------------------------------------

    bullets.erase(
        std::remove_if(
            bullets.begin(),
            bullets.end(),

            [](const Bullet& bullet)
            {
                return
                    !bullet.IsActive();
            }),

        bullets.end());

    //--------------------------------------------------
    // Remove destroyed enemies.
    //--------------------------------------------------

    enemies.erase(
        std::remove_if(
            enemies.begin(),
            enemies.end(),

            [](const Enemy& enemy)
            {
                return
                    !enemy.IsAlive();
            }),

        enemies.end());
}

//--------------------------------------------------
// Rendering
//
// Draws the entire game world.
//
// Draw Order:
//
//      1. Map
//      2. Player
//      3. Enemies
//      4. Bullets
//
// Maintaining a consistent draw order
// ensures proper visual layering.
//--------------------------------------------------

void Game::Render()
{
    //--------------------------------------------------
    // Prepare rendering frame.
    //--------------------------------------------------

    renderer.BeginFrame();

    //--------------------------------------------------
    // Draw map tiles.
    //--------------------------------------------------

    renderer.RenderMap(
        map);

    //--------------------------------------------------
    // Draw player tank.
    //--------------------------------------------------

    renderer.RenderPlayer(
        player);

    //--------------------------------------------------
    // Draw enemy tanks.
    //--------------------------------------------------

    renderer.RenderEnemies(
        enemies);

    //--------------------------------------------------
    // Draw active projectiles.
    //--------------------------------------------------

    renderer.RenderBullets(
        bullets);

    //--------------------------------------------------
    // Finalize rendering operations.
    //--------------------------------------------------

    renderer.EndFrame();
}

//--------------------------------------------------
// Shutdown
//
// Releases all game resources.
//
// This function is called before the
// application terminates.
//--------------------------------------------------

void Game::Shutdown()
{
    //--------------------------------------------------
    // Shutdown rendering subsystem.
    //--------------------------------------------------

    renderer.Shutdown();

    //--------------------------------------------------
    // Remove all active projectiles.
    //--------------------------------------------------

    bullets.clear();

    //--------------------------------------------------
    // Remove all active enemies.
    //--------------------------------------------------

    enemies.clear();
}

//--------------------------------------------------
// Accessors
//
// Provides controlled access to the
// internal game systems.
//
// Useful for:
//
//      Debugging
//      Future UI systems
//      Level management
//      Boss systems
//--------------------------------------------------

Tank& Game::GetPlayer()
{
    return player;
}

std::vector<Enemy>&
Game::GetEnemies()
{
    return enemies;
}

std::vector<Bullet>&
Game::GetBullets()
{
    return bullets;
}

Map& Game::GetMap()
{
    return map;
}

Renderer& Game::GetRenderer()
{
    return renderer;
}
