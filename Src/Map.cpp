/*
    ============================================================
    Map.cpp

    Author: Leonardo Moura
    Date: 6/12/2026

    Description:

    Implements the battlefield used by the Tank Battle game.

    Responsibilities:

        • Procedural map generation
        • Tile management
        • Collision queries
        • Spawn point management
        • Destructible environment
        • Tank collision testing

    Tile Types:

        • Empty
            Walkable area

        • Breakable
            Destroyed with one shot

        • Steel
            Indestructible obstacle

    The map is generated every time the game starts,
    creating a unique battlefield for each session.

    ============================================================
*/

#include "Map.h"

#include <cstdlib>
#include <ctime>

/*
    Constructor

    Initializes map dimensions and seeds
    the random number generator used for
    procedural generation.

    Current Map Size:

        Rows:
            22

        Columns:
            40

        Tile Size:
            32x32 pixels

    Total Playable Area:

        1280 x 704 pixels
*/
Map::Map()
{
    rows = 22;
    columns = 40;
    tileSize = 32;

    //--------------------------------------------------
    // Initialize random number generator.
    //
    // This ensures a different battlefield is
    // generated each time the game runs.
    //--------------------------------------------------

    std::srand(
        static_cast<unsigned int>(
            std::time(nullptr)));
}

/*
    Converts a row and column pair into
    a one-dimensional array index.

    Formula:

        index =
            row * columns + column
*/
int Map::Index(
    int row,
    int column) const
{
    return row * columns + column;
}

//--------------------------------------------------
// Map Generation
//--------------------------------------------------

/*
    Removes all tiles from the map.

    Used before generating a new battlefield.
*/
void Map::Clear()
{
    tiles.clear();
}

/*
    Generates a new procedural battlefield.

    Generation Steps:

        1. Create empty grid
        2. Randomly place obstacles
        3. Create player safe zone
        4. Create enemy safe zones
*/
void Map::Generate()
{
    //--------------------------------------------------
    // Remove previous map data.
    //--------------------------------------------------

    Clear();

    //--------------------------------------------------
    // Allocate all grid cells.
    //--------------------------------------------------

    tiles.resize(
        rows * columns);

    //--------------------------------------------------
    // Random Tile Generation
    //
    // Breakable:
    //      18%
    //
    // Steel:
    //      7%
    //
    // Empty:
    //      Remaining area
    //--------------------------------------------------

    for (int row = 0;
        row < rows;
        row++)
    {
        for (int column = 0;
            column < columns;
            column++)
        {
            Tile tile;

            //--------------------------------------------------
            // Convert grid coordinates into
            // world coordinates.
            //--------------------------------------------------

            tile.position =
            {
                static_cast<float>(
                    column * tileSize),

                static_cast<float>(
                    row * tileSize)
            };

            //--------------------------------------------------
            // Default tile.
            //--------------------------------------------------

            tile.type =
                TileType::Empty;

            tile.health = 0;

            //--------------------------------------------------
            // Random obstacle generation.
            //--------------------------------------------------

            int random =
                std::rand() % 100;

            if (random < 18)
            {
                tile.type =
                    TileType::Breakable;

                tile.health = 1;
            }
            else if (random < 25)
            {
                tile.type =
                    TileType::Steel;

                tile.health = -1;
            }

            tiles[
                Index(
                    row,
                    column)] =
                tile;
        }
    }

    //--------------------------------------------------
    // Player Spawn Zone
    //
    // Clears a safe area around the
    // player starting position.
    //--------------------------------------------------

    for (int row = 0;
        row < 5;
        row++)
    {
        for (int col = 0;
            col < 5;
            col++)
        {
            Tile& tile =
                tiles[
                    Index(
                        row,
                        col)];

            tile.type =
                TileType::Empty;

            tile.health = 0;
        }
    }

    //--------------------------------------------------
    // Enemy Spawn Zone
    //
    // Top Left Enemy
    //--------------------------------------------------

    for (int row = 17;
        row < 22;
        row++)
    {
        for (int col = 0;
            col < 5;
            col++)
        {
            Tile& tile =
                tiles[
                    Index(
                        row,
                        col)];

            tile.type =
                TileType::Empty;

            tile.health = 0;
        }
    }

    //--------------------------------------------------
    // Enemy Spawn Zone
    //
    // Top Right Enemy
    //--------------------------------------------------

    for (int row = 17;
        row < 22;
        row++)
    {
        for (int col = 35;
            col < 40;
            col++)
        {
            Tile& tile =
                tiles[
                    Index(
                        row,
                        col)];

            tile.type =
                TileType::Empty;

            tile.health = 0;
        }
    }

    //--------------------------------------------------
    // Enemy Spawn Zone
    //
    // Center Enemy
    //--------------------------------------------------

    for (int row = 9;
        row < 13;
        row++)
    {
        for (int col = 18;
            col < 22;
            col++)
        {
            Tile& tile =
                tiles[
                    Index(
                        row,
                        col)];

            tile.type =
                TileType::Empty;

            tile.health = 0;
        }
    }

    //--------------------------------------------------
    // Enemy Spawn Zone
    //
    // Bottom Right Enemy
    //--------------------------------------------------

    for (int row = 0;
        row < 5;
        row++)
    {
        for (int col = 35;
            col < 40;
            col++)
        {
            Tile& tile =
                tiles[
                    Index(
                        row,
                        col)];

            tile.type =
                TileType::Empty;

            tile.health = 0;
        }
    }
}

//--------------------------------------------------
// Grid Information
//
// Provides information about the map layout.
//--------------------------------------------------

int Map::GetRows() const
{
    return rows;
}

int Map::GetColumns() const
{
    return columns;
}

int Map::GetTileSize() const
{
    return tileSize;
}

//--------------------------------------------------
// Tile Access
//
// Allows retrieval of tiles using
// row/column coordinates.
//--------------------------------------------------

Tile& Map::GetTile(
    int row,
    int column)
{
    return
        tiles[
            Index(
                row,
                column)];
}

const Tile& Map::GetTile(
    int row,
    int column) const
{
    return
        tiles[
            Index(
                row,
                column)];
}

//--------------------------------------------------
// Map Queries
//--------------------------------------------------

/*
    Determines whether a world position
    exists inside the map boundaries.
*/
bool Map::IsInsideMap(
    const glm::vec2& position) const
{
    return
        position.x >= 0.0f &&
        position.y >= 0.0f &&
        position.x <
        columns * tileSize &&
        position.y <
        rows * tileSize;
}

/*
    Determines whether a position contains
    a solid obstacle.

    Returns:

        true
            Breakable or Steel

        false
            Empty tile
*/
bool Map::IsBlocked(
    const glm::vec2& position) const
{
    if (!IsInsideMap(position))
    {
        //--------------------------------------------------
        // Outside the map is considered blocked.
        //--------------------------------------------------

        return true;
    }

    int column =
        static_cast<int>(
            position.x /
            tileSize);

    int row =
        static_cast<int>(
            position.y /
            tileSize);

    const Tile& tile =
        GetTile(
            row,
            column);

    return
        tile.type !=
        TileType::Empty;
}

/*
    Applies damage to a tile.

    Breakable:
        Loses health

    Steel:
        Ignores damage
*/
bool Map::DamageTile(
    const glm::vec2& position)
{
    if (!IsInsideMap(position))
    {
        return false;
    }

    int column =
        static_cast<int>(
            position.x /
            tileSize);

    int row =
        static_cast<int>(
            position.y /
            tileSize);

    Tile& tile =
        GetTile(
            row,
            column);

    //--------------------------------------------------
    // Steel blocks cannot be damaged.
    //--------------------------------------------------

    if (tile.type ==
        TileType::Steel)
    {
        return false;
    }

    //--------------------------------------------------
    // Breakable blocks lose health.
    //--------------------------------------------------

    if (tile.type ==
        TileType::Breakable)
    {
        tile.health--;

        //--------------------------------------------------
        // Remove block when health reaches zero.
        //--------------------------------------------------

        if (tile.health <= 0)
        {
            tile.type =
                TileType::Empty;

            tile.health = 0;
        }

        return true;
    }

    return false;
}

//--------------------------------------------------
// Spawn Points
//
// Defines where gameplay entities
// initially appear.
//--------------------------------------------------

/*
    Player spawn position.
*/
glm::vec2 Map::GetPlayerSpawn() const
{
    return
    {
        64.0f,
        64.0f
    };
}

/*
    Enemy spawn positions.

    Distribution:

        Enemy 0:
            Bottom Left

        Enemy 1:
            Bottom Right

        Enemy 2:
            Center

        Enemy 3:
            Top Right
*/
glm::vec2 Map::GetEnemySpawn(
    int index) const
{
    switch (index)
    {
    case 0:
        return
        {
            64.0f,
            640.0f
        };

    case 1:
        return
        {
            1184.0f,
            640.0f
        };

    case 2:
        return
        {
            640.0f,
            352.0f
        };

    default:
        return
        {
            1184.0f,
            64.0f
        };
    }
}

//--------------------------------------------------
// Tank Collision Testing
//
// Checks whether a tank overlaps
// any solid tile.
//
// Uses the four corners of the tank's
// bounding box.
//--------------------------------------------------

bool Map::CheckTankCollision(
    const glm::vec2& position,
    float width,
    float height) const
{
    float left =
        position.x -
        width * 0.5f;

    float right =
        position.x +
        width * 0.5f;

    float bottom =
        position.y -
        height * 0.5f;

    float top =
        position.y +
        height * 0.5f;

    //--------------------------------------------------
    // Test all four corners.
    //--------------------------------------------------

    glm::vec2 corners[4] =
    {
        {left,bottom},
        {right,bottom},
        {left,top},
        {right,top}
    };

    for (int i = 0;
        i < 4;
        i++)
    {
        if (
            IsBlocked(
                corners[i]))
        {
            return true;
        }
    }

    return false;
}

//--------------------------------------------------
// Tile Collection Access
//
// Provides read-only access to the entire
// tile array for rendering.
//--------------------------------------------------

const std::vector<Tile>&
Map::GetTiles() const
{
    return tiles;
}

//--------------------------------------------------
// Tile Helper Functions
//--------------------------------------------------

/*
    Returns true if the tile blocks movement.
*/
bool Tile::IsSolid() const
{
    return
        type ==
        TileType::Breakable ||
        type ==
        TileType::Steel;
}

/*
    Returns true if the tile can be destroyed.
*/
bool Tile::IsBreakable() const
{
    return
        type ==
        TileType::Breakable;
}

//--------------------------------------------------
// Destructor
//
// No dynamic memory is owned directly
// by the Map class.
//--------------------------------------------------

Map::~Map()
{
}
