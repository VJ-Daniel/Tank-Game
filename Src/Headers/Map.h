/*
    ============================================================
    Map.h

    Author: Leonardo Moura
    Date: 6/12/2026

    Description:

    Defines the battlefield system used by the
    Tank Battle game.

    Responsibilities:

        • Procedural map generation
        • Tile storage
        • Collision queries
        • Spawn point management
        • Tile destruction
        • Tank collision testing

    Map Structure:

        Grid Based Layout

            Row
             ↓

        [ ][ ][ ][ ]
        [ ][ ][ ][ ]
        [ ][ ][ ][ ]
        [ ][ ][ ][ ]

              →
            Column

    Tile Types:

        Empty
            Walkable area

        Breakable
            Destroyed by bullets

        Steel
            Indestructible obstacle

    ============================================================
*/

#ifndef MAP_H
#define MAP_H

#include <vector>

#include <glm.hpp>

/*
    Tile Types

    Empty
        No collision.
        Tanks and bullets can pass.

    Breakable
        Blocks movement.
        Can be destroyed.

    Steel
        Blocks movement.
        Cannot be destroyed.
*/
enum class TileType
{
    Empty,
    Breakable,
    Steel
};

/*
    Tile

    Represents a single cell in the battlefield
    grid.

    Stores:

        • Type
        • Health
        • World Position
*/
struct Tile
{
    //--------------------------------------------------
    // Tile Category
    //--------------------------------------------------

    TileType type;

    //--------------------------------------------------
    // Health
    //
    // Empty:
    //      0
    //
    // Breakable:
    //      Positive value
    //
    // Steel:
    //      Usually -1
    //--------------------------------------------------

    int health;

    //--------------------------------------------------
    // World Position
    //
    // Represents the bottom-left corner
    // of the tile in world coordinates.
    //--------------------------------------------------

    glm::vec2 position;

    //--------------------------------------------------
    // Helper Functions
    //--------------------------------------------------

    /*
        Returns true if the tile blocks
        movement.
    */
    bool IsSolid() const;

    /*
        Returns true if the tile can be
        destroyed by projectiles.
    */
    bool IsBreakable() const;
};

/*
    Map

    Represents the entire battlefield.

    Responsibilities:

        • Store all tiles
        • Generate procedural levels
        • Handle collision queries
        • Manage spawn locations
        • Process tile damage
*/
class Map
{
private:

    //--------------------------------------------------
    // Grid Configuration
    //
    // rows
    //      Number of grid rows.
    //
    // columns
    //      Number of grid columns.
    //
    // tileSize
    //      Width and height of each tile.
    //--------------------------------------------------

    int rows;

    int columns;

    int tileSize;

    //--------------------------------------------------
    // Tile Storage
    //
    // Stores every tile in the battlefield.
    //
    // Internally represented as a one-dimensional
    // array for efficiency.
    //--------------------------------------------------

    std::vector<Tile> tiles;

private:

    //--------------------------------------------------
    // Index Conversion
    //
    // Converts:
    //
    //      Row + Column
    //
    // Into:
    //
    //      Linear Array Index
    //
    // Formula:
    //
    //      row * columns + column
    //--------------------------------------------------

    int Index(
        int row,
        int column) const;

public:

    //--------------------------------------------------
    // Constructor
    //
    // Creates a new map and initializes
    // grid configuration values.
    //--------------------------------------------------

    Map();

    //--------------------------------------------------
    // Map Generation
    //
    // Generate()
    //      Creates a new procedural map.
    //
    // Clear()
    //      Removes all existing tiles.
    //--------------------------------------------------

    void Generate();

    void Clear();

    //--------------------------------------------------
    // Grid Information
    //
    // Returns basic information about the
    // battlefield dimensions.
    //--------------------------------------------------

    int GetRows() const;

    int GetColumns() const;

    int GetTileSize() const;

    //--------------------------------------------------
    // Tile Access
    //
    // Provides direct access to a tile
    // using row and column coordinates.
    //--------------------------------------------------

    Tile& GetTile(
        int row,
        int column);

    const Tile& GetTile(
        int row,
        int column) const;

    //--------------------------------------------------
    // World Queries
    //
    // IsInsideMap()
    //      Checks whether a position is
    //      inside map boundaries.
    //
    // IsBlocked()
    //      Checks whether a position contains
    //      a solid tile.
    //
    // DamageTile()
    //      Applies damage to a tile.
    //--------------------------------------------------

    bool IsInsideMap(
        const glm::vec2& position) const;

    bool IsBlocked(
        const glm::vec2& position) const;

    bool DamageTile(
        const glm::vec2& position);

    //--------------------------------------------------
    // Spawn Points
    //
    // Returns predefined spawn locations
    // used during level initialization.
    //--------------------------------------------------

    glm::vec2 GetPlayerSpawn() const;

    glm::vec2 GetEnemySpawn(
        int index) const;

    //--------------------------------------------------
    // Collision Detection
    //
    // Tests whether a tank overlaps any
    // solid tiles.
    //
    // Used by:
    //
    //      • Player movement
    //      • Enemy movement
    //--------------------------------------------------

    bool CheckTankCollision(
        const glm::vec2& position,
        float width,
        float height) const;

    //--------------------------------------------------
    // Tile Collection Access
    //
    // Provides read-only access to all tiles.
    //
    // Used primarily by the Renderer.
    //--------------------------------------------------

    const std::vector<Tile>&
        GetTiles() const;

    //--------------------------------------------------
    // Destructor
    //
    // Releases map resources.
    //
    // No dynamic memory is currently
    // allocated directly by this class.
    //--------------------------------------------------

    ~Map();
};

#endif