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
#include <iostream>

namespace
{
    const int ENEMY_COUNT_LEVEL1 = 4;
    const int ENEMY_COUNT_LEVEL2 = 5;
    const int ENEMY_COUNT_LEVEL3 = 6;

    const glm::vec2 LEVEL2_ENEMY_SPAWNS[5] = {
        { 1184.0f, 64.0f },
        { 64.0f, 640.0f },
        { 1184.0f, 640.0f },
        { 640.0f, 352.0f },
        { 640.0f, 640.0f }
    };

    const glm::vec2 LEVEL3_ENEMY_SPAWNS[6] = {
        { 1184.0f, 64.0f },
        { 640.0f, 64.0f },
        { 64.0f, 640.0f },
        { 1184.0f, 640.0f },
        { 640.0f, 640.0f },
        { 640.0f, 352.0f }
    };
}

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
    levelTransition = false;
    levelTransitionTimer = 0.0f;
    victoryDisplayed = false;
    victoryTimer = 0.0f;
    gameOverTimer = 0.0f; 
    gameOverDisplayed = false;
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
    LoadLevel(1);
    return true;
}

void Game::LoadLevel(int level) {
    bullets.clear();
    enemies.clear();

    levelTransition = false;
    levelTransitionTimer = 0.0f;
    victoryDisplayed = false;
    victoryTimer = 0.0f;

    switch (level)
    {
    case 1:
        LoadLevel1();
        break;
    case 2:
        LoadLevel2();
        break;
    case 3:
        LoadLevel3();
        break;
    case 4:
        LoadLevel4();
        break;
    default:
        break;
    }

    levelManager.LoadLevel(level);
}

void Game::LoadLevel1() {
    map.Generate();

    renderer.SetLevel(1);

    SpawnPlayer();

    SpawnEnemiesLevel1();
}

void Game::LoadLevel2() {
    map.Generate();

    renderer.SetLevel(2);

    int tileSize = map.GetTileSize();
    int rows = map.GetRows();
    int columns = map.GetColumns();

    for (int i = 0; i < ENEMY_COUNT_LEVEL2; i++)
    {
        int centerCol = static_cast<int>(LEVEL2_ENEMY_SPAWNS[i].x) / tileSize;
        int centerRow = static_cast<int>(LEVEL2_ENEMY_SPAWNS[i].y) / tileSize;

        for (int row = centerRow - 2; row <= centerRow + 2; row++)
        {
            for (int col = centerCol - 2; col <= centerCol + 2; col++)
            {
                if (row < 0 || row >= rows || col < 0 || col >= columns)
                    continue;

                Tile& tile = map.GetTile(row, col);
                tile.type = TileType::Empty;
                tile.health = 0;
            }
        }
    }
    // Spawn player
    SpawnPlayer();

    // Spawn enemies
    SpawnEnemiesLevel2();
}

void Game::LoadLevel3() {
    map.Generate();

    renderer.SetLevel(3);

    // Increase density
    int tileSize = map.GetTileSize();
    int rows = map.GetRows();
    int columns = map.GetColumns();

    for (int row = 0; row < rows; row++)
    {
        for (int col = 0; col < columns; col++)
        {
            Tile& tile = map.GetTile(row, col);
            if (tile.type != TileType::Empty)
                continue;

            int random = std::rand() % 100;
            if (random < 15)
            {
                tile.type = TileType::Breakable;
                tile.health = 1;
            }
            else if (random < 20)
            {
                tile.type = TileType::Steel;
                tile.health = -1;
            }
        }
    }

    // Clear spawn zones
    for (int s = -1; s < ENEMY_COUNT_LEVEL3; s++)
    {
        glm::vec2 spawn = (s < 0) ? map.GetPlayerSpawn() : LEVEL3_ENEMY_SPAWNS[s];

        int centerCol = static_cast<int>(spawn.x) / tileSize;
        int centerRow = static_cast<int>(spawn.y) / tileSize;

        for (int row = centerRow - 2; row <= centerRow + 2; row++)
        {
            for (int col = centerCol - 2; col <= centerCol + 2; col++)
            {
                if (row < 0 || row >= rows || col < 0 || col >= columns)
                    continue;

                Tile& tile = map.GetTile(row, col);
                tile.type = TileType::Empty;
                tile.health = 0;
            }
        }
    }

    // Spawn player
    SpawnPlayer();

    // Spawn enemies
    SpawnEnemiesLevel3();
}

void Game::LoadLevel4()
{
    enemies.clear();
    bullets.clear();

    map.Generate();
    renderer.SetLevel(4);

    int rows = map.GetRows();    // 22
    int columns = map.GetColumns(); // 40

    // Step 1: Fill everything as empty first
    for (int row = 0; row < rows; row++)
    {
        for (int col = 0; col < columns; col++)
        {
            Tile& tile = map.GetTile(row, col);
            tile.type = TileType::Empty;
            tile.health = 0;
        }
    }

    // Step 2: Steel border walls
    for (int row = 0; row < rows; row++)
    {
        for (int col = 0; col < columns; col++)
        {
            if (row == 0 || row == rows - 1 || col == 0 || col == columns - 1)
            {
                Tile& tile = map.GetTile(row, col);
                tile.type = TileType::Steel;
                tile.health = -1;
            }
        }
    }

    // Helper lambda: place a steel block rectangle
    auto PlaceSteel = [&](int startRow, int startCol, int h, int w)
        {
            for (int r = startRow; r < startRow + h; r++)
            {
                for (int c = startCol; c < startCol + w; c++)
                {
                    if (r <= 0 || r >= rows - 1 || c <= 0 || c >= columns - 1)
                        continue;
                    Tile& tile = map.GetTile(r, c);
                    tile.type = TileType::Steel;
                    tile.health = -1;
                }
            }
        };

    // Step 3: Corner pillars (2x2 steel, inset from border)
    // Top-left
    PlaceSteel(2, 2, 2, 2);
    // Top-right
    PlaceSteel(2, columns - 4, 2, 2);
    // Bottom-left
    PlaceSteel(rows - 4, 2, 2, 2);
    // Bottom-right
    PlaceSteel(rows - 4, columns - 4, 2, 2);

    // Step 4: Mid-field side barrier pairs (create lanes)
    // Left-center pair
    PlaceSteel(7, 4, 2, 3);
    PlaceSteel(13, 4, 2, 3);
    // Right-center pair
    PlaceSteel(7, columns - 7, 2, 3);
    PlaceSteel(13, columns - 7, 2, 3);

    // Step 5: Inner pillars near center (force player to maneuver)
    PlaceSteel(7, 14, 2, 3);
    PlaceSteel(7, columns - 17, 2, 3);
    PlaceSteel(13, 14, 2, 3);
    PlaceSteel(13, columns - 17, 2, 3);

    // Clear player spawn zone around (160, 580) → col 3-8, row 16-20
    for (int r = 16; r <= 20; r++)
    {
        for (int c = 3; c <= 8; c++)
        {
            Tile& tile = map.GetTile(r, c);
            tile.type = TileType::Empty;
            tile.health = 0;
        }
    }

    // Step 7: Clear boss spawn zone (center-top, 7x5 tiles)
    // Boss spawns at (640, 96) → tile col=20, row=3
    for (int r = 1; r <= 5; r++)
    {
        for (int c = 17; c <= 23; c++)
        {
            Tile& tile = map.GetTile(r, c);
            tile.type = TileType::Empty;
            tile.health = 0;
        }
    }

    // Spawn entities
    SpawnPlayer();
    SpawnBoss();
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
    // Level 4 boss arena: spawn away from corner walls
    if (levelManager.GetCurrentLevel() == 4)
    {
        player.SetPosition(glm::vec2(160.0f, 580.0f));
    }
    else
    {
        player.SetPosition(map.GetPlayerSpawn());
    }

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

void Game::SpawnEnemiesLevel1()
{
    enemies.clear();

    for (int i = 0;
        i < ENEMY_COUNT_LEVEL1;
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

void Game::SpawnEnemiesLevel2()
{
    enemies.clear();

    for (int i = 0;
        i < ENEMY_COUNT_LEVEL2;
        i++)
    {
        Enemy enemy;

        enemy.SetPosition(LEVEL2_ENEMY_SPAWNS[i]);

        enemy.SetMoveSpeed(
            120.0f);

        enemy.SetSize(
            32.0f,
            32.0f);

        enemies.push_back(
            enemy);
    }
}

void Game::SpawnEnemiesLevel3()
{
    enemies.clear();

    for (int i = 0;
        i < ENEMY_COUNT_LEVEL3;
        i++)
    {
        Enemy enemy;

        enemy.SetPosition(LEVEL3_ENEMY_SPAWNS[i]);

        enemy.SetMoveSpeed(
            120.0f);

        enemy.SetSize(
            32.0f,
            32.0f);

        enemies.push_back(
            enemy);
    }
}

void Game::SpawnBoss()
{
    boss = Boss();
    boss.SetPosition(glm::vec2(640.0f, 96.0f));  // center-top, clear zone
    boss.SetRotation(3.14f);                       // faces downward toward player
    boss.SetTargetPosition(player.GetPosition());
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

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    {
        LoadLevel(1);
    }

    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    {
        LoadLevel(2);
    }

    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
    {
        LoadLevel(3);
    }

    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
    {
        LoadLevel(4);
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
    if (levelManager.GetState() == GameState::GameOver)
    {
        gameOverTimer += deltaTime;
        if (gameOverTimer > 3.0f) 
        {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
        return;
    }

    if (levelManager.GetState() == GameState::Victory)
    {
        victoryTimer += deltaTime;
        if (victoryTimer > 5.0f) 
        {
            
        }
        return;
    }

    if (levelTransition)
    {
        levelTransitionTimer += deltaTime;
        if (levelTransitionTimer > 2.0f) 
        {
            levelTransition = false;
            int nextLevel = levelManager.GetCurrentLevel() + 1;
            if (nextLevel <= 4)
            {
                LoadLevel(nextLevel);
            }
        }
        return;
    }

    UpdatePlayer(
        deltaTime);

    UpdateEnemies(
        deltaTime);

    // ADD THIS:
    if (levelManager.GetCurrentLevel() == 4)
    {
        UpdateBoss(deltaTime);
    }

    UpdateBullets(
        deltaTime);

    CheckCollisions();

    RemoveDestroyedObjects();

    CheckLevelCompletion();
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

void Game::UpdateBoss(float deltaTime)
{
    if (!boss.IsAlive())
        return;

    // Keep the boss tracking the player every frame
    boss.SetTargetPosition(player.GetPosition());

    glm::vec2 oldPosition = boss.GetPosition();

    // Run boss AI (moves toward player, rotates, advances fireTimer)
    boss.Update(deltaTime);

    // Prevent boss from moving through walls
    if (map.CheckTankCollision(boss.GetPosition(), boss.GetWidth(), boss.GetHeight()))
    {
        boss.SetPosition(oldPosition);
    }

    // Clamp to screen bounds (boss is 128px, so half = 64px)
    glm::vec2 pos = boss.GetPosition();
    pos.x = std::clamp(pos.x, 64.0f, static_cast<float>(screenWidth - 64));
    pos.y = std::clamp(pos.y, 64.0f, static_cast<float>(screenHeight - 64));
    boss.SetPosition(pos);

    // Fire triple shot when in range
    if (boss.CanSeeTarget() && boss.DistanceToTarget() <= boss.GetShootingRange())
    {
        boss.FireTripleShot(bullets);
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

            // ADD HERE — after the enemy loop, still inside IsFromPlayer()
            if (bullet.IsActive() && levelManager.GetCurrentLevel() == 4 && boss.IsAlive())
            {
                glm::vec2 bulletMin = bullet.GetMinBounds();
                glm::vec2 bulletMax = bullet.GetMaxBounds();
                glm::vec2 bossMin = boss.GetMinBounds();
                glm::vec2 bossMax = boss.GetMaxBounds();

                bool overlap =
                    bulletMin.x < bossMax.x &&
                    bulletMax.x > bossMin.x &&
                    bulletMin.y < bossMax.y &&
                    bulletMax.y > bossMin.y;

                if (overlap)
                {
                    boss.TakeDamage(1);
                    bullet.Deactivate();
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
                levelManager.SetState(GameState::GameOver);  
                gameOverTimer = 0.0f; 
                std::cout << "Player destroyed! Game Over!" << std::endl;
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

void Game::CheckLevelCompletion()
{
    int currentLevel = levelManager.GetCurrentLevel();

    if (currentLevel < 4)
    {
        // Check if all enemies are destroyed
        bool allEnemiesDestroyed = true;
        for (const Enemy& enemy : enemies)
        {
            if (enemy.IsAlive())
            {
                allEnemiesDestroyed = false;
                break;
            }
        }

        if (allEnemiesDestroyed && !levelTransition)
        {
            levelTransition = true;
            levelTransitionTimer = 0.0f;
            std::cout << "Level " << currentLevel << " Complete! Loading next level..." << std::endl;
        }
    }
    else if (currentLevel == 4)
    {
        if (boss.isDefeated())
        {
            levelManager.SetState(GameState::Victory);
            victoryTimer = 0.0f;
            std::cout << "Boss defeated! Victory!" << std::endl;
        }
    }
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

    if (levelManager.GetCurrentLevel() == 4 && boss.IsAlive())
    {
        renderer.RenderBoss(boss);
    }

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

