#include "Map.h"
#include <cstring>

Map::Map() {
    buildGrid();
    buildWaypoints();
    std::memset(hasTowerGrid, false, sizeof(hasTowerGrid));
}

void Map::buildGrid() {
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            grid[r][c] = GRASS;

    // Horizontal 1: row 3, col 0-10
    for (int c = 0; c <= 10; c++) grid[3][c] = PATH;
    grid[3][0] = ENTRY;

    // Vertical 1: col 10, row 3-8
    for (int r = 3; r <= 8;  r++) grid[r][10] = PATH;

    // Horizontal 2: row 8, col 4-10
    for (int c = 4; c <= 10; c++) grid[8][c]  = PATH;

    // Vertical 2: col 4, row 8-11
    for (int r = 8; r <= 11; r++) grid[r][4]  = PATH;

    // Horizontal 3: row 11, col 4-17
    for (int c = 4; c <= 17; c++) grid[11][c] = PATH;

    // Vertical 3: col 17, row 6-11
    for (int r = 6; r <= 11; r++) grid[r][17] = PATH;

    // Horizontal 4: row 6, col 17-21
    for (int c = 17; c <= 21; c++) grid[6][c] = PATH;
    grid[6][21] = EXIT;
}

void Map::buildWaypoints() {
    const float TS = static_cast<float>(TILE_SIZE);
    auto centre = [&](int col, int row) -> sf::Vector2f {
        return sf::Vector2f(col * TS + TS / 2.f, row * TS + TS / 2.f);
    };
    waypoints[0] = centre(0,  3);
    waypoints[1] = centre(10, 3);
    waypoints[2] = centre(10, 8);
    waypoints[3] = centre(4,  8);
    waypoints[4] = centre(4,  11);
    waypoints[5] = centre(17, 11);
    waypoints[6] = centre(17, 6);
    waypoints[7] = centre(21, 6);
}

Map::TileType Map::getTile(int col, int row) const {
    if (col < 0 || col >= COLS || row < 0 || row >= ROWS) return GRASS;
    return static_cast<TileType>(grid[row][col]);
}

bool Map::isPlaceable(int col, int row) const {
    if (col < 0 || col >= COLS || row < 0 || row >= ROWS) return false;
    if (col >= 20) return false;
    TileType t = getTile(col, row);
    return (t == GRASS) && !hasTowerGrid[row][col];
}

void Map::setTower(int col, int row, bool val) {
    if (col >= 0 && col < COLS && row >= 0 && row < ROWS)
        hasTowerGrid[row][col] = val;
}

void Map::render(sf::RenderWindow& window) const {
    const float TS = static_cast<float>(TILE_SIZE);
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            sf::RectangleShape tile(sf::Vector2f(TS - 1.f, TS - 1.f));
            tile.setPosition(c * TS, r * TS);
            switch (grid[r][c]) {
            case PATH:  tile.setFillColor(sf::Color(180, 150, 100)); break;
            case ENTRY: tile.setFillColor(sf::Color(50,  200,  50)); break;
            case EXIT:  tile.setFillColor(sf::Color(200,  50,  50)); break;
            default:
                tile.setFillColor(((r + c) % 2 == 0)
                    ? sf::Color(80, 150, 60)
                    : sf::Color(70, 135, 55));
                break;
            }
            window.draw(tile);
        }
    }
}
