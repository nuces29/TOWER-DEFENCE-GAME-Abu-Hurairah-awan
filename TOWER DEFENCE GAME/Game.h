#pragma once
#include <SFML/Graphics.hpp>
#include<SFML/Audio.hpp>
#include "Constants.h"
#include "Map.h"
#include "Enemy.h"
#include "Tower.h"
#include "Projectile.h"
#include "WaveManager.h"
#include "HUD.h"

class Game {
public:
    enum class GameState { MENU, PLAYING, GAME_OVER, WIN };

    Game();
    ~Game();
    void run();

private:
    // ---- SFML ----
    sf::RenderWindow window;
    sf::Font         font;
    sf::Clock        clock;
    sf::Music          music;

    Map        map;
    WaveManager waveManager;
    HUD*       hud;

    Enemy*      enemies[MAX_ENEMIES];
    int         enemyCount;

    Tower*      towers[MAX_TOWERS];
    int         towerCount;

    Projectile* projectiles;
    int         projectileCount;

    // Game state 
    GameState gameState;
    int       gold;
    int       lives;
    int       selectedTowerType;
    Tower* selectedTower;
    int       score;
    int       highScore;

    //High score
    void saveHighScore();
    void loadHighScore();

    sf::Text titleText, subtitleText, promptText, myname;

    void processEvents();
    void update(float dt);
    void render();

    void handleMouseClick(sf::Vector2i mousePos);
    void updateCollisions();
    void cleanupDead();

    void initMenu();
    void initGame();
    void initGameOver(bool won);
    void clearEnemies();
    void clearTowers();

    Tower* makeTower(int type, float x, float y);
    int    towerCost(int type) const;
};
