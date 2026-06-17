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
        Creates and configures enemy tanks.

        Responsibilities:

            • Set spawn positions
            • Configure AI properties
            • Configure dimensions
    */
    void SpawnEnemies();
    void SpawnEnemiesLevel2();
    void SpawnEnemiesLevel3();
    /*
        Processes player keyboard input.

        Controls:

            • Movement
            • Rotation
            • Shooting
    */
    void HandlePlayerInput(
        float deltaTime);

    /*
        Updates player state.

        Responsibilities:

            • Movement validation
            • Boundary checks
            • Gameplay state updates
    */
    void UpdatePlayer(
        float deltaTime);

    /*
        Updates all enemy tanks.

        Responsibilities:

            • AI state transitions
            • Patrol behavior
            • Chase behavior
            • Shooting behavior
    */
    void UpdateEnemies(
        float deltaTime);

    /*
        Updates all active projectiles.

        Responsibilities:

            • Movement
            • Collision tests
            • Screen boundary checks
    */
    void UpdateBullets(
        float deltaTime);

    /*
        Performs collision detection.

        Checks:

            • Bullet vs Enemy
            • Bullet vs Player
            • Bullet vs Map
    */
    void CheckCollisions();

    /*
        Removes inactive gameplay objects.

        Objects Removed:

            • Destroyed bullets
            • Destroyed enemies
    */
    void RemoveDestroyedObjects();

public:

    //--------------------------------------------------
    // Constructor
    //
    // Creates the game controller.
    //
    // Parameters:
    //
    //      window
    //          GLFW window reference
    //
    //      width
    //          Screen width
    //
    //      height
    //          Screen height
    //--------------------------------------------------

    Game(
        GLFWwindow* window,
        int width,
        int height);

    //--------------------------------------------------
    // Initialization
    //
    // Initializes all major game systems.
    //
    // Responsibilities:
    //
    //      • Renderer setup
    //      • Map generation
    //      • Player creation
    //      • Enemy creation
    //
    // Returns:
    //
    //      true
    //          Initialization succeeded
    //
    //      false
    //          Initialization failed
    //--------------------------------------------------

    bool Initialize(
        Shader* shader);

    //--------------------------------------------------
    // Main Game Loop Functions
    //
    // ProcessInput()
    //      Reads keyboard input.
    //
    // Update()
    //      Executes gameplay logic.
    //
    // Render()
    //      Draws the current frame.
    //--------------------------------------------------

    void ProcessInput(
        float deltaTime);

    void Update(
        float deltaTime);

    void Render();

    //--------------------------------------------------
    // Shutdown
    //
    // Releases resources and prepares the
    // game for application termination.
    //--------------------------------------------------

    void Shutdown();

    //--------------------------------------------------
    // Accessors
    //
    // Provides controlled access to
    // internal game systems.
    //
    // Useful for:
    //
    //      • Debugging
    //      • Future UI systems
    //      • Level management
    //      • Boss systems
    //--------------------------------------------------

    Tank& GetPlayer();

    std::vector<Enemy>&
        GetEnemies();

    std::vector<Bullet>&
        GetBullets();

    Map& GetMap();

    Renderer& GetRenderer();

    void LoadLevel2();
    void LoadLevel3();
};

#endif