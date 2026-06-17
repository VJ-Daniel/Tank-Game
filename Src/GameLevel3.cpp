/*
    ============================================================
    GameLevel3.cpp

    Author: Leonardo Moura
    Date: 6/12/2026

    Description:

    Implements Level 3 ("Heavy Resistance") for the
    Tank Battle game.

    This file ONLY adds Level 3 behavior. It does not
    modify any existing gameplay. It reuses the existing
    systems exactly as they are:

        • Map procedural generation        (Map::Generate)
        • Patrol / Chase enemy AI           (Enemy)
        • Projectiles and collision         (Bullet / Game)
        • MVP rendering pipeline            (Renderer)

    Level 3 Requirements implemented here:

        • 6 enemy tanks
        • New obstacle color scheme         (Renderer::SetLevel)
        • Higher battlefield density
        • Enemy spawns remain obstacle-free
        • Existing patrol and chase AI

    These two member functions belong to the Game class.
    Their declarations must be added to Game.h
    (see LEVEL3_README.md).

    ============================================================
*/

#include "Game.h"

#include <cstdlib>

namespace
{
    //--------------------------------------------------
    // Level 3 enemy count.
    //
    // Level 2 used 5 enemies. Level 3 raises the
    // resistance to 6 enemies.
    //--------------------------------------------------

    const int LEVEL3_ENEMY_COUNT = 6;

    //--------------------------------------------------
    // Level 3 enemy spawn positions (world space).
    //
    // Spread across the battlefield and kept away
    // from the player spawn at (64, 64).
    //
    // The tiles around each position are cleared in
    // LoadLevel3(), guaranteeing obstacle-free spawns
    // even after the density increase.
    //--------------------------------------------------

    const glm::vec2 LEVEL3_ENEMY_SPAWNS[LEVEL3_ENEMY_COUNT] =
    {
        { 1184.0f,   64.0f },   // Bottom Right
        {  640.0f,   64.0f },   // Bottom Center
        {   64.0f,  640.0f },   // Top Left
        { 1184.0f,  640.0f },   // Top Right
        {  640.0f,  640.0f },   // Top Center
        {  640.0f,  352.0f }    // Center
    };
}

//--------------------------------------------------
// Level 3 Loader
//
// Builds the "Heavy Resistance" battlefield.
//
// Steps:
//
//      1. Regenerate the map
//      2. Apply the Level 3 color scheme
//      3. Increase battlefield density
//      4. Clear obstacle-free spawn zones
//      5. Re-place the player
//      6. Spawn 6 enemies
//      7. Clear leftover projectiles
//--------------------------------------------------

void Game::LoadLevel3()
{
    //--------------------------------------------------
    // Regenerate a fresh battlefield.
    //
    // Reuses the existing procedural generation.
    //--------------------------------------------------

    map.Generate();

    //--------------------------------------------------
    // Switch the renderer to the Level 3
    // obstacle color scheme.
    //--------------------------------------------------

    renderer.SetLevel(3);

    int tileSize =
        map.GetTileSize();

    int rows =
        map.GetRows();

    int columns =
        map.GetColumns();

    //--------------------------------------------------
    // Higher Battlefield Density
    //
    // The base generation produces ~25% obstacles.
    // Level 3 adds more obstacles by converting some
    // of the remaining empty tiles into blocks.
    //
    // Only EMPTY tiles are affected, so existing
    // obstacles are preserved.
    //
    //      Breakable:  +15%
    //      Steel:      + 5%
    //--------------------------------------------------

    for (int row = 0;
        row < rows;
        row++)
    {
        for (int col = 0;
            col < columns;
            col++)
        {
            Tile& tile =
                map.GetTile(
                    row,
                    col);

            //--------------------------------------------------
            // Skip tiles that already contain obstacles.
            //--------------------------------------------------

            if (tile.type !=
                TileType::Empty)
            {
                continue;
            }

            int random =
                std::rand() % 100;

            if (random < 15)
            {
                tile.type =
                    TileType::Breakable;

                tile.health = 1;
            }
            else if (random < 20)
            {
                tile.type =
                    TileType::Steel;

                tile.health = -1;
            }
        }
    }

    //--------------------------------------------------
    // Clear obstacle-free zones.
    //
    // Performed AFTER the density increase so the
    // player and every enemy are guaranteed to spawn
    // on clear ground.
    //
    // Each zone is a 5x5 tile area (160 pixels),
    // far larger than the 32x32 tanks.
    //--------------------------------------------------

    for (int s = -1;
        s < LEVEL3_ENEMY_COUNT;
        s++)
    {
        //--------------------------------------------------
        // s == -1 clears the player spawn zone.
        // s >= 0  clears each enemy spawn zone.
        //--------------------------------------------------

        glm::vec2 spawn =
            (s < 0)
            ? map.GetPlayerSpawn()
            : LEVEL3_ENEMY_SPAWNS[s];

        int centerCol =
            static_cast<int>(
                spawn.x)
            / tileSize;

        int centerRow =
            static_cast<int>(
                spawn.y)
            / tileSize;

        for (int row = centerRow - 2;
            row <= centerRow + 2;
            row++)
        {
            for (int col = centerCol - 2;
                col <= centerCol + 2;
                col++)
            {
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
    //--------------------------------------------------

    SpawnPlayer();

    //--------------------------------------------------
    // Spawn the six Level 3 enemies.
    //--------------------------------------------------

    SpawnEnemiesLevel3();

    //--------------------------------------------------
    // Remove any projectiles left over from the
    // previous level.
    //--------------------------------------------------

    bullets.clear();
}

//--------------------------------------------------
// Level 3 Enemy Spawn
//
// Creates 6 enemy tanks at the predefined
// Level 3 spawn positions.
//
// Uses the exact same configuration as the
// original SpawnEnemies(), so the existing
// patrol / chase AI applies unchanged.
//--------------------------------------------------

void Game::SpawnEnemiesLevel3()
{
    enemies.clear();

    for (int i = 0;
        i < LEVEL3_ENEMY_COUNT;
        i++)
    {
        Enemy enemy;

        enemy.SetPosition(
            LEVEL3_ENEMY_SPAWNS[i]);

        enemy.SetMoveSpeed(
            120.0f);

        enemy.SetSize(
            32.0f,
            32.0f);

        enemies.push_back(
            enemy);
    }
}
