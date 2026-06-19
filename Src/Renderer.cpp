/*
    ============================================================
    Renderer.cpp

    Author: Leonardo Moura
    Date: 6/12/2026

    Description:

    Implements the rendering subsystem for the
    Tank Battle game.

    Responsibilities:

        • Create renderable meshes
        • Configure orthographic projection
        • Render tanks
        • Render bullets
        • Render map tiles
        • Manage GPU rendering state
        • Release rendering resources

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
        Vertex Shader
              ↓
        Fragment Shader
              ↓
        Screen

    ============================================================
*/

#include "Renderer.h"

#include <vector>
#include <algorithm>

#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>

namespace
{
    /*
        Creates a single colored triangle and
        appends it to the vertex array.

        Vertex Format:

            x y z r g b
    */
    void AddTriangle(
        std::vector<float>& vertices,

        float x1, float y1,
        float x2, float y2,
        float x3, float y3,

        float r,
        float g,
        float b)
    {
        vertices.insert(
            vertices.end(),
            {
                x1,y1,0.0f,r,g,b,
                x2,y2,0.0f,r,g,b,
                x3,y3,0.0f,r,g,b
            });
    }

    /*
        Creates a colored rectangle using
        two triangles.

        Used to build:

            • Tank bodies
            • Tank turrets
            • Bullets
            • Map blocks
    */
    std::vector<float> CreateRectangle(
        float width,
        float height,

        float r,
        float g,
        float b)
    {
        std::vector<float> vertices;

        float hw =
            width * 0.5f;

        float hh =
            height * 0.5f;

        //--------------------------------------------------
        // Triangle 1
        //--------------------------------------------------

        AddTriangle(
            vertices,

            -hw, -hh,
            hw, -hh,
            hw, hh,

            r, g, b);

        //--------------------------------------------------
        // Triangle 2
        //--------------------------------------------------

        AddTriangle(
            vertices,

            -hw, -hh,
            hw, hh,
            -hw, hh,

            r, g, b);

        return vertices;
    }
}

/*
    Constructor

    Initializes all rendering pointers and
    OpenGL related variables.
*/
Renderer::Renderer()
{
    shader = nullptr;

    mvpLocation = -1;

    playerBodyMesh = nullptr;
    playerTurretMesh = nullptr;

    enemyBodyMesh = nullptr;
    enemyTurretMesh = nullptr;

    bulletMesh = nullptr;

    brickMesh = nullptr;
    steelMesh = nullptr;

    //--------------------------------------------------
    // Level 2 obstacle meshes (new color scheme).
    //--------------------------------------------------

    brickMeshLevel2 = nullptr;
    steelMeshLevel2 = nullptr;

    //--------------------------------------------------
    // Level 3 obstacle meshes (new color scheme).
    //
    // These reuse the exact same geometry as the
    // Level 1 blocks; only the colors differ.
    //--------------------------------------------------

    brickMeshLevel3 = nullptr;
    steelMeshLevel3 = nullptr;

    overlayMesh         = nullptr;
    goPanelMesh         = nullptr;
    goBarMesh           = nullptr;
    victoryPanelMesh    = nullptr;
    victoryDiamondMesh  = nullptr;
    enemyIconMesh       = nullptr;
    enemyIconDeadMesh   = nullptr;
    bossHpBorderMesh    = nullptr;
    bossHpBgMesh        = nullptr;
    bossHpFillMesh      = nullptr;

    alphaLocation = -1;

    //--------------------------------------------------
    // Active level.
    //
    // Determines which obstacle color scheme is
    // used when drawing map tiles.
    //
    //      1 -> Level 1 colors
    //      2 -> Level 2 colors
    //      3 -> Level 3 colors
    //--------------------------------------------------

    currentLevel = 1;
}

/*
    Initializes the rendering system.

    Creates all meshes required by the game.
*/
bool Renderer::Initialize()
{
    //--------------------------------------------------
    // Create orthographic projection.
    //
    // Origin:
    //      Bottom Left
    //
    // Resolution:
    //      1280 x 720
    //--------------------------------------------------

    projection =
        glm::ortho(
            0.0f,
            1280.0f,
            0.0f,
            720.0f,
            -1.0f,
            1.0f);

    //--------------------------------------------------
    // Identity view matrix.
    //
    // No camera movement currently exists.
    //--------------------------------------------------

    view =
        glm::mat4(1.0f);

    //--------------------------------------------------
    // Player Tank Meshes
    //--------------------------------------------------

    playerBodyMesh =
        new Mesh(
            CreateRectangle(
                32.0f,
                32.0f,

                0.0f,
                0.8f,
                0.0f));

    playerTurretMesh =
        new Mesh(
            CreateRectangle(
                24.0f,
                6.0f,

                0.2f,
                1.0f,
                0.2f));

    //--------------------------------------------------
    // Enemy Tank Meshes
    //--------------------------------------------------

    enemyBodyMesh =
        new Mesh(
            CreateRectangle(
                32.0f,
                32.0f,

                0.8f,
                0.0f,
                0.0f));

    enemyTurretMesh =
        new Mesh(
            CreateRectangle(
                24.0f,
                6.0f,

                1.0f,
                0.2f,
                0.2f));

    //--------------------------------------------------
    // Boss Tank Meshes
    //--------------------------------------------------

    bossBodyMesh =
        new Mesh(
            CreateRectangle(
                32.0f,
                32.0f,

                0.25f,
                0.0f,
                0.45f));

    bossTurretMesh =
        new Mesh(
            CreateRectangle(
                36.0f,
                10.0f,

                1.0f,
                0.85f,
                0.1f));

    //--------------------------------------------------
    // Bullet Mesh
    //--------------------------------------------------

    bulletMesh =
        new Mesh(
            CreateRectangle(
                8.0f,
                8.0f,

                1.0f,
                1.0f,
                0.0f));

    //--------------------------------------------------
    // Breakable Block Mesh (Level 1)
    //--------------------------------------------------

    brickMesh =
        new Mesh(
            CreateRectangle(
                32.0f,
                32.0f,

                0.55f,
                0.27f,
                0.07f));

    //--------------------------------------------------
    // Steel Block Mesh (Level 1)
    //--------------------------------------------------

    steelMesh =
        new Mesh(
            CreateRectangle(
                32.0f,
                32.0f,

                0.65f,
                0.65f,
                0.65f));

    //--------------------------------------------------
    // Breakable Block Mesh (Level 2)
    //
    // Cool teal tone for the "Increased Threat"
    // battlefield.
    //--------------------------------------------------

    brickMeshLevel2 =
        new Mesh(
            CreateRectangle(
                32.0f,
                32.0f,

                0.10f,
                0.45f,
                0.55f));

    //--------------------------------------------------
    // Steel Block Mesh (Level 2)
    //--------------------------------------------------

    steelMeshLevel2 =
        new Mesh(
            CreateRectangle(
                32.0f,
                32.0f,

                0.25f,
                0.30f,
                0.40f));

    //--------------------------------------------------
    // Breakable Block Mesh (Level 3)
    //
    // Crimson tone for the "Heavy Resistance"
    // battlefield.
    //--------------------------------------------------

    brickMeshLevel3 =
        new Mesh(
            CreateRectangle(
                32.0f,
                32.0f,

                0.65f,
                0.12f,
                0.15f));

    //--------------------------------------------------
    // Steel Block Mesh (Level 3)
    //
    // Dark maroon steel to match the Level 3
    // color scheme.
    //--------------------------------------------------

    steelMeshLevel3 =
        new Mesh(
            CreateRectangle(
                32.0f,
                32.0f,

                0.35f,
                0.12f,
                0.12f));

    // Full-screen black overlay (drawn with uAlpha < 1 for transparency)
    overlayMesh =
        new Mesh(CreateRectangle(1280.0f, 720.0f, 0.0f, 0.0f, 0.0f));

    // Game Over: dark red backing panel + red X bars
    goPanelMesh =
        new Mesh(CreateRectangle(300.0f, 160.0f, 0.25f, 0.0f, 0.0f));

    goBarMesh =
        new Mesh(CreateRectangle(180.0f, 22.0f, 0.9f, 0.1f, 0.1f));

    // Victory: dark gold backing panel + gold diamond
    victoryPanelMesh =
        new Mesh(CreateRectangle(300.0f, 160.0f, 0.22f, 0.18f, 0.0f));

    victoryDiamondMesh =
        new Mesh(CreateRectangle(80.0f, 80.0f, 0.95f, 0.82f, 0.05f));

    // Enemy counter icons
    enemyIconMesh =
        new Mesh(CreateRectangle(16.0f, 16.0f, 0.9f, 0.15f, 0.15f));

    enemyIconDeadMesh =
        new Mesh(CreateRectangle(16.0f, 16.0f, 0.25f, 0.25f, 0.25f));

    // Boss HP bar: border, background, fill
    bossHpBorderMesh =
        new Mesh(CreateRectangle(404.0f, 22.0f, 0.1f, 0.1f, 0.1f));

    bossHpBgMesh =
        new Mesh(CreateRectangle(400.0f, 18.0f, 0.35f, 0.35f, 0.35f));

    bossHpFillMesh =
        new Mesh(CreateRectangle(400.0f, 18.0f, 0.85f, 0.1f, 0.1f));

    return true;
}

/*
    Selects the active level.

    Used to switch the obstacle color scheme.

        Level 1 -> original brown / gray blocks
        Level 2 -> teal / slate blocks
        Level 3 -> crimson / maroon blocks
*/
void Renderer::SetLevel(
    int level)
{
    currentLevel =
        level;
}

/*
    Associates a shader program with the renderer.

    Also retrieves the MVP matrix uniform
    location from the shader.
*/
void Renderer::SetShader(
    Shader* shaderProgram)
{
    shader =
        shaderProgram;

    if (shader)
    {
        mvpLocation =
            static_cast<int>(
                glGetUniformLocation(
                    shader->GetID(),
                    "mvp"));

        alphaLocation =
            static_cast<int>(
                glGetUniformLocation(
                    shader->GetID(),
                    "uAlpha"));

        glUniform1f(alphaLocation, 1.0f);
    }
}

/*
    Begins a new rendering frame.

    Clears the screen and activates
    the shader program.
*/
void Renderer::BeginFrame()
{
    //--------------------------------------------------
    // Clear screen to black.
    //--------------------------------------------------

    glClearColor(
        0.0f,
        0.0f,
        0.0f,
        1.0f);

    glClear(
        GL_COLOR_BUFFER_BIT);

    //--------------------------------------------------
    // Activate shader program.
    //--------------------------------------------------

    if (shader)
    {
        shader->Use();
        glUniform1f(alphaLocation, 1.0f);
    }
}

/*
    EndFrame

    Reserved for future rendering operations.

    Examples:

        • Post processing
        • UI rendering
        • Particle effects
*/
void Renderer::EndFrame()
{}

/*
    Draws a mesh using the supplied model matrix.

    Computes:

        MVP =
            Projection
            *
            View
            *
            Model
*/
void Renderer::DrawMesh(
    Mesh* mesh,
    const glm::mat4& model)
{
    if (!mesh)
    {
        return;
    }

    glm::mat4 mvp =
        projection *
        view *
        model;

    //--------------------------------------------------
    // Send MVP matrix to shader.
    //--------------------------------------------------

    glUniformMatrix4fv(
        mvpLocation,
        1,
        GL_FALSE,
        glm::value_ptr(
            mvp));

    mesh->Draw();
}

/*
    Draws a tank.

    The tank is composed of:

        • Body
        • Turret

    Both rotate together.
*/
void Renderer::DrawTank(
    const Tank& tank,
    Mesh* bodyMesh,
    Mesh* turretMesh)
{
    glm::vec2 position =
        tank.GetPosition();

    float rotation =
        tank.GetRotation();

    float scaleX = tank.GetWidth() / 32.0f;
    float scaleY = tank.GetHeight() / 32.0f;

    //--------------------------------------------------
    // Tank Body
    //--------------------------------------------------

    glm::mat4 bodyModel =
        glm::translate(
            glm::mat4(1.0f),
            glm::vec3(
                position.x,
                position.y,
                0.0f));

    bodyModel =
        glm::rotate(
            bodyModel,
            rotation,
            glm::vec3(
                0.0f,
                0.0f,
                1.0f));

    bodyModel =
        glm::scale(
            bodyModel,
            glm::vec3(
                tank.GetWidth() / 32.0f,
                tank.GetHeight() / 32.0f,
                1.0f));

    DrawMesh(
        bodyMesh,
        bodyModel);

    //--------------------------------------------------
    // Tank Turret
    //--------------------------------------------------

    glm::mat4 turretModel =
        glm::translate(
            glm::mat4(1.0f),
            glm::vec3(
                position.x,
                position.y,
                0.0f));

    turretModel =
        glm::rotate(
            turretModel,
            rotation,
            glm::vec3(
                0.0f,
                0.0f,
                1.0f));

    //--------------------------------------------------
    // Move turret slightly forward.
    //--------------------------------------------------

    turretModel =
        glm::translate(
            turretModel,
            glm::vec3(
                tank.GetWidth() * 0.35f,
                0.0f,
                0.0f));

    turretModel =
        glm::scale(
            turretModel,
            glm::vec3(
                tank.GetWidth() / 32.0f,
                1.5f,
                1.0f));

    DrawMesh(
        turretMesh,
        turretModel);
}

/*
    Draws a projectile.
*/
void Renderer::DrawBullet(
    const Bullet& bullet)
{
    glm::vec2 position =
        bullet.GetPosition();

    glm::mat4 model =
        glm::translate(
            glm::mat4(1.0f),
            glm::vec3(
                position.x,
                position.y,
                0.0f));

    DrawMesh(
        bulletMesh,
        model);
}

/*
    Draws a map tile.

    Tile Types:

        • Breakable
        • Steel

    The mesh chosen depends on the active level,
    which controls the obstacle color scheme.
*/
void Renderer::DrawTile(
    const Tile& tile)
{
    Mesh* mesh =
        nullptr;

    //--------------------------------------------------
    // Select the obstacle mesh based on the
    // current level color scheme.
    //--------------------------------------------------

    if (tile.type ==
        TileType::Breakable)
    {
        if (currentLevel == 2)
        {
            mesh = brickMeshLevel2;
        }
        else if (currentLevel >= 3)
        {
            mesh = brickMeshLevel3;
        }
        else
        {
            mesh = brickMesh;
        }
    }
    else if (
        tile.type ==
        TileType::Steel)
    {
        if (currentLevel == 2)
        {
            mesh = steelMeshLevel2;
        }
        else if (currentLevel >= 3)
        {
            mesh = steelMeshLevel3;
        }
        else
        {
            mesh = steelMesh;
        }
    }

    if (!mesh)
    {
        return;
    }

    //--------------------------------------------------
    // Convert tile corner position into
    // tile center position.
    //--------------------------------------------------

    glm::mat4 model =
        glm::translate(
            glm::mat4(1.0f),
            glm::vec3(
                tile.position.x + 16.0f,
                tile.position.y + 16.0f,
                0.0f));

    DrawMesh(
        mesh,
        model);
}

/*
    Draws the player tank.
*/
void Renderer::RenderPlayer(
    const Tank& player)
{
    if (!player.IsAlive())
    {
        return;
    }

    DrawTank(
        player,
        playerBodyMesh,
        playerTurretMesh);
}

/*
    Draws all active enemy tanks.
*/
void Renderer::RenderEnemies(
    const std::vector<Enemy>& enemies)
{
    for (const Enemy& enemy : enemies)
    {
        if (!enemy.IsAlive())
        {
            continue;
        }

        DrawTank(
            enemy,
            enemyBodyMesh,
            enemyTurretMesh);
    }
}

void Renderer::RenderBoss(
    const Boss& boss)
{
    if (!boss.IsAlive())
    {
        return;
    }

    DrawTank(
        boss,
        bossBodyMesh,
        bossTurretMesh);
}

/*
    Draws all active bullets.
*/
void Renderer::RenderBullets(
    const std::vector<Bullet>& bullets)
{
    for (const Bullet& bullet : bullets)
    {
        if (!bullet.IsActive())
        {
            continue;
        }

        DrawBullet(
            bullet);
    }
}

/*
    Draws every tile in the map.
*/
void Renderer::RenderMap(
    const Map& map)
{
    const std::vector<Tile>& tiles =
        map.GetTiles();

    for (const Tile& tile : tiles)
    {
        DrawTile(
            tile);
    }
}

/*
    Releases all dynamically allocated meshes.
*/
void Renderer::Shutdown()
{
    delete playerBodyMesh;
    playerBodyMesh = nullptr;

    delete playerTurretMesh;
    playerTurretMesh = nullptr;

    delete enemyBodyMesh;
    enemyBodyMesh = nullptr;

    delete enemyTurretMesh;
    enemyTurretMesh = nullptr;

    delete bulletMesh;
    bulletMesh = nullptr;

    delete brickMesh;
    brickMesh = nullptr;

    delete steelMesh;
    steelMesh = nullptr;

    //--------------------------------------------------
    // Release Level 2 obstacle meshes.
    //--------------------------------------------------

    delete brickMeshLevel2;
    brickMeshLevel2 = nullptr;

    delete steelMeshLevel2;
    steelMeshLevel2 = nullptr;

    //--------------------------------------------------
    // Release Level 3 obstacle meshes.
    //--------------------------------------------------

    delete brickMeshLevel3;
    brickMeshLevel3 = nullptr;

    delete steelMeshLevel3;
    steelMeshLevel3 = nullptr;

    delete overlayMesh;         overlayMesh         = nullptr;
    delete goPanelMesh;         goPanelMesh         = nullptr;
    delete goBarMesh;           goBarMesh           = nullptr;
    delete victoryPanelMesh;    victoryPanelMesh    = nullptr;
    delete victoryDiamondMesh;  victoryDiamondMesh  = nullptr;
    delete enemyIconMesh;       enemyIconMesh       = nullptr;
    delete enemyIconDeadMesh;   enemyIconDeadMesh   = nullptr;
    delete bossHpBorderMesh;    bossHpBorderMesh    = nullptr;
    delete bossHpBgMesh;        bossHpBgMesh        = nullptr;
    delete bossHpFillMesh;      bossHpFillMesh      = nullptr;
}

/*
    Destructor

    Ensures all rendering resources
    are properly released.
*/
Renderer::~Renderer()
{
    Shutdown();
}

void Renderer::SetAlpha(float alpha)
{
    if (alphaLocation != -1)
        glUniform1f(alphaLocation, alpha);
}

void Renderer::RenderEnemyCounter(int remaining, int total)
{
    if (total <= 0)
        return;

    const float iconSize = 16.0f;
    const float gap      = 4.0f;
    const float startX   = 20.0f + iconSize * 0.5f;
    const float y        = 702.0f;

    for (int i = 0; i < total; i++)
    {
        float cx = startX + i * (iconSize + gap);
        glm::mat4 model =
            glm::translate(glm::mat4(1.0f), glm::vec3(cx, y, 0.0f));

        DrawMesh(i < remaining ? enemyIconMesh : enemyIconDeadMesh, model);
    }
}

void Renderer::RenderBossHealthBar(int hp, int maxHp)
{
    if (maxHp <= 0)
        return;

    float ratio = std::clamp((float)hp / (float)maxHp, 0.0f, 1.0f);
    const float barY = 702.0f;
    const float barHalfW = 200.0f; // half of 400px bar

    glm::mat4 borderModel =
        glm::translate(glm::mat4(1.0f), glm::vec3(640.0f, barY, 0.0f));
    DrawMesh(bossHpBorderMesh, borderModel);

    glm::mat4 bgModel =
        glm::translate(glm::mat4(1.0f), glm::vec3(640.0f, barY, 0.0f));
    DrawMesh(bossHpBgMesh, bgModel);

    if (ratio > 0.0f)
    {
        float fillCenterX = (640.0f - barHalfW) + ratio * barHalfW;
        glm::mat4 fillModel =
            glm::translate(glm::mat4(1.0f), glm::vec3(fillCenterX, barY, 0.0f));
        fillModel = glm::scale(fillModel, glm::vec3(ratio, 1.0f, 1.0f));
        DrawMesh(bossHpFillMesh, fillModel);
    }
}

void Renderer::RenderGameOver()
{
    // Semi-transparent overlay
    SetAlpha(0.75f);
    DrawMesh(overlayMesh, glm::mat4(1.0f));
    SetAlpha(1.0f);

    glm::mat4 center =
        glm::translate(glm::mat4(1.0f), glm::vec3(640.0f, 360.0f, 0.0f));

    // Dark red backing panel
    DrawMesh(goPanelMesh, center);

    // Red X: two bars rotated ±45°
    glm::mat4 bar1 = glm::rotate(center, glm::radians(45.0f),  glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 bar2 = glm::rotate(center, glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    DrawMesh(goBarMesh, bar1);
    DrawMesh(goBarMesh, bar2);
}

void Renderer::RenderVictory()
{
    // Semi-transparent overlay
    SetAlpha(0.75f);
    DrawMesh(overlayMesh, glm::mat4(1.0f));
    SetAlpha(1.0f);

    glm::mat4 center =
        glm::translate(glm::mat4(1.0f), glm::vec3(640.0f, 360.0f, 0.0f));

    // Dark gold backing panel
    DrawMesh(victoryPanelMesh, center);

    // Gold diamond (square rotated 45°)
    glm::mat4 diamond = glm::rotate(center, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    DrawMesh(victoryDiamondMesh, diamond);
}
