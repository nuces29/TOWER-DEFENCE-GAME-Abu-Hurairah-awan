#pragma once
#include <SFML/Graphics.hpp>

class Map {
public:
    static const int TILE_SIZE = 48;
    static const int COLS      = 22;
    static const int ROWS      = 15;
    static const int WP_COUNT  =  8;  

    enum TileType { GRASS = 0, PATH = 1, ENTRY = 2, EXIT = 3 };

    Map();
    void render(sf::RenderWindow& window) const;

    TileType getTile(int col, int row) const;
    bool     isPlaceable(int col, int row) const;
    void     setTower(int col, int row, bool val);

   
    const sf::Vector2f* getWaypoints()     const { return waypoints; }
    int                 getWaypointCount() const { return WP_COUNT; }

    sf::Vector2f getEntryPoint() const { return waypoints[0]; }
    sf::Vector2f getExitPoint()  const { return waypoints[WP_COUNT - 1]; }

    int pixelToCol(float x) const { return static_cast<int>(x / TILE_SIZE); }
    int pixelToRow(float y) const { return static_cast<int>(y / TILE_SIZE); }

private:
    int          grid[ROWS][COLS];
    bool         hasTowerGrid[ROWS][COLS];
    sf::Vector2f waypoints[WP_COUNT];

    void buildGrid();
    void buildWaypoints();
};
