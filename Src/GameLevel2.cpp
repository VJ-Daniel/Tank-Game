/*
    ============================================================
    GameLevel2.cpp

    Author: Leonardo Moura
    Date: 6/12/2026

    Description:

    Implements Level 2 ("Increased Threat") for the
    Tank Battle game.

    This file ONLY adds Level 2 behavior. It does not
    modify any existing gameplay. It reuses the existing
    systems exactly as they are:

        • Map procedural generation        (Map::Generate)
        • Patrol / Chase enemy AI           (Enemy)
        • Projectiles and collision         (Bullet / Game)
        • MVP rendering pipeline            (Renderer)

    Level 2 Requirements implemented here:

        • 5 enemy tanks
        • New obstacle color scheme         (Renderer::SetLevel)
        • Regenerated battlefield           (Map::Generate)
        • Enemy spawns never overlap obstacles
        • Existing patrol and chase AI

    These two member functions belong to the Game class.
    Their declarations must be added to Game.h
    (see LEVEL2_README.md).

    ============================================================
*/

#include "Game.h"

namespace
{
    //--------------------------------------------------
    // Level 2 enemy count.
    //
    // Level 1 used 4 enemies. Level 2 increases the
    // threat with 5 enemies.
    //--------------------------------------------------

    const int LEVEL2_ENEMY_COUNT = 5;

    //--------------------------------------------------
    // Level 2 enemy spawn positions (world space).
    //
    // Spread across the battlefield and kept away
    // from the player spawn at (64, 64).
    //
    // The tiles around each position are cleared in
    // LoadLevel2(), guaranteeing obstacle-free spawns.
    //--------------------------------------------------

    const glm::vec2 LEVEL2_ENEMY_SPAWNS[LEVEL2_ENEMY_COUNT] =
    {
        { 1184.0f,   64.0f },   // Bottom Right
        {   64.0f,  640.0f },   // Top Left
        { 1184.0f,  640.0f },   // Top Right
        {  640.0f,  352.0f },   // Center
        {  640.0f,  640.0f }    // Top Center
    };
}

//--------------------------------------------------
// Level 2 Loader
//
// Builds the "Increased Threat" battlefield.
//
// Steps:
//
//      1. Regenerate the map
//      2. Apply the Level 2 color scheme
//      3. Clear obstacle-free enemy spawn zones
//      4. Re-place the player
//      5. Spawn 5 enemies
//      6. Clear leftover projectiles
//--------------------------------------------------

void Game::LoadLevel2()
{
    //--------------------------------------------------
    // Regenerate a fresh battlefield.
    //
    // Reuses the existing procedural generation.
    //--------------------------------------------------

    map.Generate();

    //--------------------------------------------------
    // Switch the renderer to the Level 2
    // obstacle color scheme.
    //--------------------------------------------------

    renderer.SetLevel(2);

    //--------------------------------------------------
    // Clear an obstacle-free zone around every
    // Level 2 enemy spawn point.
    //
    // This guarantees enemies never spawn inside
    // a breakable or steel block.
    //--------------------------------------------------

    int tileSize =
        map.GetTileSize();

    int rows =
        map.GetRows();

    int columns =
        map.GetColumns();

    for (int i = 0;
        i < LEVEL2_ENEMY_COUNT;
        i++)
    {
        //--------------------------------------------------
        // Convert the spawn world position into the
        // grid cell that contains it.
        //--------------------------------------------------

        int centerCol =
            static_cast<int>(
                LEVEL2_ENEMY_SPAWNS[i].x)
            / tileSize;

        int centerRow =
            static_cast<int>(
                LEVEL2_ENEMY_SPAWNS[i].y)
            / tileSize;

        //--------------------------------------------------
        // Clear a 5x5 tile area around the spawn.
        //
        // 5 tiles = 160 pixels, far larger than the
        // 32x32 tank, so the spawn is always clear.
        //--------------------------------------------------

        for (int row = centerRow - 2;
            row <= centerRow + 2;
            row++)
        {
            for (int col = centerCol - 2;
                col <= centerCol + 2;
                col++)
            {
                //--------------------------------------------------
                // Skip tiles outside the map.
                //--------------------------------------------------

                if (row < 0 ||
                    row >= rows ||
                    col < 0 ||
                    col >= columns)
                {
                    continue;
                }

                Tile& tile =
                    map.GetTile(
                        row,
                        col);

                tile.type =
                    TileType::Empty;

                tile.health = 0;
            }
        }
    }

    //--------------------------------------------------
    // Re-place the player at its spawn point.
    //
    // The map generator always clears the player
    // spawn zone, so this position is obstacle-free.
    //--------------------------------------------------

    SpawnPlayer();

    //--------------------------------------------------
    // Spawn the five Level 2 enemies.
    //--------------------------------------------------

    SpawnEnemiesLevel2();

    //--------------------------------------------------
    // Remove any projectiles left over from the
    // previous level.
    //--------------------------------------------------

    bullets.clear();
}

//--------------------------------------------------
// Level 2 Enemy Spawn
//
// Creates 5 enemy tanks at the predefined
// Level 2 spawn positions.
//
// Uses the exact same configuration as the
// original SpawnEnemies(), so the existing
// patrol / chase AI applies unchanged.
//--------------------------------------------------

void Game::SpawnEnemiesLevel2()
{
    enemies.clear();

    for (int i = 0;
        i < LEVEL2_ENEMY_COUNT;
        i++)
    {
        Enemy enemy;

        enemy.SetPosition(
            LEVEL2_ENEMY_SPAWNS[i]);

        enemy.SetMoveSpeed(
            120.0f);

        enemy.SetSize(
            32.0f,
            32.0f);

        enemies.push_back(
            enemy);
    }
}
