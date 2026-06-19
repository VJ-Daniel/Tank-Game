/*
    ============================================================
    Renderer.h

    Author: Leonardo Moura
    Date: 6/12/2026

    Description:

    Defines the rendering subsystem used by the
    Tank Battle game.

    Responsibilities:

        • Create renderable meshes
        • Manage shader programs
        • Manage transformation matrices
        • Render tanks
        • Render bullets
        • Render map tiles
        • Manage rendering resources

    Rendering Architecture:

                    Renderer
                         |
        -------------------------------------
        |          |          |            |
      Tanks     Bullets     Map        Shader
        |
    ----------------
    |              |
   Player       Enemies

    Rendering Pipeline:

        Game Objects
              ↓
        Model Matrix
              ↓
        View Matrix
              ↓
        Projection Matrix
              ↓
        MVP Matrix
              ↓
        Shader Program
              ↓
        GPU Rendering
              ↓
           Screen

    ============================================================
*/

#ifndef RENDERER_H
#define RENDERER_H

#include <glew.h>

#include <vector>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include "Shader.h"
#include "Mesh.h"

#include "Tank.h"
#include "Enemy.h"
#include "Bullet.h"
#include "Map.h"
#include "Boss.h"

class Renderer
{
private:

    //--------------------------------------------------
    // Shader Program
    //
    // Active shader used for all rendering
    // operations.
    //
    // Responsible for:
    //
    //      • Vertex transformations
    //      • Color output
    //      • MVP processing
    //--------------------------------------------------

    Shader* shader;

    //--------------------------------------------------
    // Shader Uniforms
    //
    // Cached uniform locations used during
    // rendering.
    //
    // mvpLocation
    //      Location of the MVP matrix uniform
    //      inside the shader program.
    //--------------------------------------------------

    int mvpLocation;

    //--------------------------------------------------
    // Transformation Matrices
    //
    // projection
    //      Orthographic projection matrix.
    //
    // view
    //      Camera matrix.
    //
    // Current implementation uses a fixed
    // top-down camera.
    //--------------------------------------------------

    glm::mat4 projection;

    glm::mat4 view;

    //--------------------------------------------------
    // Player Tank Meshes
    //
    // playerBodyMesh
    //      Main tank body.
    //
    // playerTurretMesh
    //      Tank cannon/turret.
    //--------------------------------------------------

    Mesh* playerBodyMesh;

    Mesh* playerTurretMesh;

    //--------------------------------------------------
    // Enemy Tank Meshes
    //
    // Separate meshes allow visual distinction
    // between player and enemy units.
    //--------------------------------------------------

    Mesh* enemyBodyMesh;

    Mesh* enemyTurretMesh;

    Mesh* bossBodyMesh;

    Mesh* bossTurretMesh;

    //--------------------------------------------------
    // Bullet Mesh
    //
    // Shared by both player and enemy
    // projectiles.
    //--------------------------------------------------

    Mesh* bulletMesh;

    //--------------------------------------------------
    // Map Tile Meshes
    //
    // brickMesh
    //      Breakable obstacle.
    //
    // steelMesh
    //      Indestructible obstacle.
    //--------------------------------------------------

    Mesh* brickMesh;

    Mesh* steelMesh;

    Mesh* brickMeshLevel2;
    Mesh* steelMeshLevel2;
    Mesh* brickMeshLevel3;
    Mesh* steelMeshLevel3;
    int   currentLevel;

    // UI meshes
    Mesh* overlayMesh;
    Mesh* goPanelMesh;
    Mesh* victoryPanelMesh;
    Mesh* enemyIconMesh;
    Mesh* enemyIconDeadMesh;
    Mesh* bossHpBorderMesh;
    Mesh* bossHpBgMesh;
    Mesh* bossHpFillMesh;
    Mesh* uiPixelRedMesh;
    Mesh* uiPixelGoldMesh;

    int alphaLocation;

private:

    //--------------------------------------------------
    // Internal Rendering Helpers
    //
    // These functions are only used internally
    // by the renderer.
    //--------------------------------------------------

    /*
        Draws a mesh using a supplied model matrix.

        Calculates:

            MVP =
                Projection *
                View *
                Model

        Then sends the result to the shader.
    */
    void DrawMesh(
        Mesh* mesh,
        const glm::mat4& model);

    /*
        Draws a complete tank.

        Components:

            • Tank Body
            • Tank Turret

        Applies:

            • Translation
            • Rotation
    */
    void DrawTank(
        const Tank& tank,
        Mesh* bodyMesh,
        Mesh* turretMesh);

    /*
        Draws a projectile.

        Applies translation to the bullet's
        world position.
    */
    void DrawBullet(
        const Bullet& bullet);

    /*
        Draws a map tile.

        Supported Types:

            • Breakable
            • Steel
    */
    void DrawTile(
        const Tile& tile);

    void SetAlpha(float alpha);
    void DrawPixelChar(char c, float left, float top, Mesh* pixelMesh);
    void DrawPixelText(const char* text, float cx, float cy, Mesh* pixelMesh);

public:

    //--------------------------------------------------
    // Constructor
    //
    // Initializes renderer state and resource
    // pointers.
    //--------------------------------------------------

    Renderer();

    //--------------------------------------------------
    // Initialization
    //
    // Creates all meshes required by the game.
    //
    // Meshes Created:
    //
    //      • Player Body
    //      • Player Turret
    //      • Enemy Body
    //      • Enemy Turret
    //      • Bullet
    //      • Brick Block
    //      • Steel Block
    //
    // Returns:
    //
    //      true
    //          Success
    //
    //      false
    //          Failure
    //--------------------------------------------------

    bool Initialize();

    //--------------------------------------------------
    // Shader Assignment
    //
    // Associates a shader program with the
    // renderer.
    //
    // Also retrieves uniform locations.
    //--------------------------------------------------

    void SetShader(
        Shader* shaderProgram);

    //--------------------------------------------------
    // Frame Management
    //
    // BeginFrame()
    //      Clears buffers and prepares rendering.
    //
    // EndFrame()
    //      Finalizes rendering operations.
    //--------------------------------------------------

    void BeginFrame();

    void EndFrame();

    //--------------------------------------------------
    // Rendering Passes
    //
    // RenderPlayer()
    //      Draws the player tank.
    //
    // RenderEnemies()
    //      Draws all enemy tanks.
    //
    // RenderBullets()
    //      Draws all active projectiles.
    //
    // RenderMap()
    //      Draws the battlefield.
    //--------------------------------------------------

    void RenderPlayer(
        const Tank& player);

    void RenderEnemies(
        const std::vector<Enemy>& enemies);

    void RenderBoss(const Boss& boss);

    void RenderBullets(
        const std::vector<Bullet>& bullets);

    void RenderMap(
        const Map& map);

    void RenderEnemyCounter(int remaining, int total);
    void RenderBossHealthBar(int hp, int maxHp);
    void RenderGameOver();
    void RenderVictory();

    //--------------------------------------------------
    // Resource Cleanup
    //
    // Releases all dynamically allocated
    // rendering resources.
    //
    // Resources Released:
    //
    //      • All Mesh Objects
    //      • Internal References
    //--------------------------------------------------

    void Shutdown();

    //--------------------------------------------------
    // Destructor
    //
    // Automatically releases rendering
    // resources before application shutdown.
    //--------------------------------------------------

    ~Renderer();
    void SetLevel(int level);
};

#endif