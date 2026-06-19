/*
    ============================================================
    Game.h

    Author: Leonardo Moura
    Date: 6/12/2026

    Description:

    Defines the Game class which acts as the
    central controller of the Tank Battle game.

    Responsibilities:

        • Initialize game systems
        • Manage player tank
        • Manage enemy tanks
        • Manage projectiles
        • Manage map generation
        • Handle collisions
        • Coordinate rendering
        • Execute gameplay updates

    Architecture:

                    Game
                      |
        --------------------------------
        |              |              |
      Map         Renderer        Entities
                                        |
                    --------------------------------
                    |              |              |
                 Player         Enemies       Bullets

    The Game class serves as the main bridge
    between gameplay logic and rendering.

    ============================================================
*/

#ifndef GAME_H
#define GAME_H

#include <vector>

#include <glew.h>
#include <glfw3.h>

#include "Tank.h"
#include "Enemy.h"
#include "Bullet.h"
#include "Map.h"
#include "Renderer.h"
#include "LevelManager.h" 
#include "Boss.h"

class Game
{
private:

    //--------------------------------------------------
    // Window Information
    //
    // Stores the GLFW window reference and
    // screen dimensions.
    //
    // Used for:
    //
    //      • Input handling
    //      • Boundary checking
    //      • Gameplay constraints
    //--------------------------------------------------

    GLFWwindow* window;

    int screenWidth;

    int screenHeight;

    //--------------------------------------------------
    // Core Game Systems
    //
    // Renderer
    //      Responsible for drawing all objects.
    //
    // Map
    //      Responsible for procedural battlefield
    //      generation and collision queries.
    //--------------------------------------------------

    Renderer renderer;

    Map map;

    LevelManager levelManager;

    //--------------------------------------------------
    // Game Entities
    //
    // player
    //      Human controlled tank.
    //
    // enemies
    //      AI controlled enemy tanks.
    //
    // bullets
    //      Active projectiles fired by both
    //      player and enemies.
    //--------------------------------------------------

    Tank player;

    std::vector<Enemy> enemies;

    std::vector<Bullet> bullets;

    Boss boss;

    //--------------------------------------------------
    // Level State
    //--------------------------------------------------

    bool levelTransition;    
    float levelTransitionTimer;
    bool victoryDisplayed;
    float victoryTimer;
    bool gameOverDisplayed;
    float gameOverTimer;

    //--------------------------------------------------
    // Internal Helper Functions
    //
    // Used exclusively by the Game class to
    // manage gameplay systems.
    //--------------------------------------------------

    /*
        Creates and configures the player tank.

        Responsibilities:

            • Set spawn position
            • Configure speed
            • Configure dimensions
    */
    void SpawnPlayer();

    /*
        Creates and configures enemy tanks for each level.
    */
    void SpawnEnemiesLevel1();
    void SpawnEnemiesLevel2();
    void SpawnEnemiesLevel3();
    void SpawnBoss();

    /*
        Processes player keyboard input.

        Controls:

            • Movement
            • Rotation
            • Shooting
    */
    void HandlePlayerInput(float deltaTime);

    /*
        Updates player state.

        Responsibilities:

            • Movement validation
            • Boundary checks
            • Gameplay state updates
    */
    void UpdatePlayer(float deltaTime);

    /*
        Updates all enemy tanks.
    */
    void UpdateEnemies(float deltaTime);

    /*
        Updates the boss (Level 4).
    */
    void UpdateBoss(float deltaTime);

    /*
        Updates all active projectiles.
    */
    void UpdateBullets(float deltaTime);

    /*
        Performs collision detection.
    */
    void CheckCollisions();

    /*
        Removes inactive gameplay objects.
    */
    void RemoveDestroyedObjects();

public:

    //--------------------------------------------------
    // Constructor
    //--------------------------------------------------

    Game(GLFWwindow* window, int width, int height);

    //--------------------------------------------------
    // Initialization
    //--------------------------------------------------

    bool Initialize(Shader* shader);

    //--------------------------------------------------
    // Main Game Loop Functions
    //--------------------------------------------------

    void ProcessInput(float deltaTime);
    void Update(float deltaTime);
    void Render();

    //--------------------------------------------------
    // Level Management
    //--------------------------------------------------

    void LoadLevel(int level);
    void LoadLevel1();
    void LoadLevel2();
    void LoadLevel3();
    void LoadLevel4();
    void CheckLevelCompletion();

    //--------------------------------------------------
    // Shutdown
    //--------------------------------------------------

    void Shutdown();

    //--------------------------------------------------
    // Accessors (Single set - remove duplicates)
    //--------------------------------------------------
    Tank& GetPlayer() { return player; }
    std::vector<Enemy>& GetEnemies() { return enemies; }
    std::vector<Bullet>& GetBullets() { return bullets; }
    Map& GetMap() { return map; }
    Renderer& GetRenderer() { return renderer; }
};

#endif