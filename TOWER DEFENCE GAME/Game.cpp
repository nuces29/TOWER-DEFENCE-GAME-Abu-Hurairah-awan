#include "Game.h"
#include <cmath>
#include <cstring>
#include <sstream>

static const int WIN_W       = 1256;
static const int WIN_H       = 720;
static const int PANEL_X     = 1056;
static const int START_GOLD  = 400;
static const int START_LIVES = 20;

Game::Game()
    : window(sf::VideoMode(WIN_W, WIN_H), "Tower Defense Game 25L-2535",
             sf::Style::Titlebar | sf::Style::Default),
      hud(nullptr),
      enemyCount(0), towerCount(0), projectileCount(0),
      projectiles(nullptr),
      gameState(GameState::MENU),
      gold(START_GOLD), lives(START_LIVES),
      selectedTowerType(-1), selectedTower(nullptr)
{
    window.setFramerateLimit(60);

    std::memset(enemies, 0, sizeof(enemies));
    std::memset(towers,  0, sizeof(towers));

    projectiles = new Projectile[MAX_PROJECTILES];

    font.loadFromFile("assets/font.ttf");

    hud = new HUD(font, PANEL_X);
    initMenu();
}

Game::~Game() {
    clearEnemies();
    clearTowers();
    delete[] projectiles;
    delete hud;
}

void Game::clearEnemies() {
    for (int i = 0; i < enemyCount; ++i) {
        delete enemies[i];
        enemies[i] = nullptr;
    }
    enemyCount = 0;
}

void Game::clearTowers() {
    for (int i = 0; i < towerCount; ++i) {
        delete towers[i];
        towers[i] = nullptr;
    }
    towerCount = 0;
}

void Game::run() {
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        if (dt > 0.05f) dt = 0.05f;

        processEvents();
        if (gameState == GameState::PLAYING) update(dt);
        render();
    }
}

void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();

        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape) window.close();
            if (event.key.code == sf::Keyboard::R)      initGame();
        }

        if (event.type == sf::Event::MouseButtonPressed &&
            event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2i mPos(event.mouseButton.x, event.mouseButton.y);

            if (gameState == GameState::MENU) { initGame(); return; }
            if (gameState == GameState::GAME_OVER ||
                gameState == GameState::WIN)        { initMenu(); return; }

            handleMouseClick(mPos);
        }

        if (event.type == sf::Event::MouseButtonPressed &&
            event.mouseButton.button == sf::Mouse::Right) {
            selectedTowerType = -1;
            selectedTower     = nullptr;
        }
    }
}

void Game::handleMouseClick(sf::Vector2i mPos) {
    // ---- HUD area ----
    if (mPos.x >= PANEL_X) {
        if (hud->isStartWaveClicked(mPos)) {
            if (!waveManager.isWaveActive() &&
                waveManager.getCurrentWave() < WaveManager::TOTAL_WAVES) {
                int nextWave = waveManager.getCurrentWave() + 1;
                waveManager.startWave(nextWave);
            }
            return;
        }

        if (hud->isSellClicked(mPos) && selectedTower) {
            int refund = selectedTower->getCost() / 2;
            gold += refund;
            int col = map.pixelToCol(selectedTower->getPosX());
            int row = map.pixelToRow(selectedTower->getPosY());
            map.setTower(col, row, false);

            for (int i = 0; i < towerCount; ++i) {
                if (towers[i] == selectedTower) {
                    delete towers[i];
                    towers[i] = towers[--towerCount];
                    towers[towerCount] = nullptr;
                    break;
                }
            }
            selectedTower = nullptr;
            return;
        }

        int t = hud->handleClick(mPos);
        if (t >= 0) { selectedTowerType = t; selectedTower = nullptr; }
        return;
    }

    for (int i = 0; i < towerCount; ++i) {
        float dx = towers[i]->getPosX() - static_cast<float>(mPos.x);
        float dy = towers[i]->getPosY() - static_cast<float>(mPos.y);
        if (std::sqrt(dx * dx + dy * dy) < 24.f) {
            selectedTower     = towers[i];
            selectedTowerType = -1;
            return;
        }
    }

    int col = map.pixelToCol(static_cast<float>(mPos.x));
    int row = map.pixelToRow(static_cast<float>(mPos.y));

    if (selectedTowerType >= 0 && map.isPlaceable(col, row)) {
        int cost = towerCost(selectedTowerType);
        if (gold >= cost && towerCount < MAX_TOWERS) {
            float px = col * Map::TILE_SIZE + Map::TILE_SIZE / 2.f;
            float py = row * Map::TILE_SIZE + Map::TILE_SIZE / 2.f;
            towers[towerCount++] = makeTower(selectedTowerType, px, py);
            map.setTower(col, row, true);
            gold -= cost;
        }
    }
}

void Game::update(float dt) {
    waveManager.update(dt,
                       enemies, enemyCount,
                       map.getWaypoints(), map.getWaypointCount(),
                       map.getEntryPoint().x, map.getEntryPoint().y,
                       map.getExitPoint().x,  map.getExitPoint().y);

    for (int i = 0; i < towerCount; ++i) {
        towers[i]->update(dt);
        towers[i]->attack(enemies, enemyCount, projectiles, projectileCount);
    }

    for (int i = 0; i < towerCount; ++i) {
        for (int j = 0; j < enemyCount; ++j) {
            if (!enemies[j] || !enemies[j]->isAlive()) continue;
            StealthEnemy* se = dynamic_cast<StealthEnemy*>(enemies[j]);
            if (se) {
                float dx = se->getPosX() - towers[i]->getPosX();
                float dy = se->getPosY() - towers[i]->getPosY();
                if (std::sqrt(dx * dx + dy * dy) < 150.f) se->reveal();
            }
        }
    }

    for (int i = 0; i < enemyCount; ++i) {
        if (!enemies[i] || !enemies[i]->isAlive()) continue;
        enemies[i]->update(dt);
        if (enemies[i]->hasReachedEnd()) {
            lives--;
            if (lives <= 0) { initGameOver(false); return; }
        }
    }

    for (int i = 0; i < projectileCount; ++i)
        projectiles[i].update(dt);

    updateCollisions();
    cleanupDead();

    if (waveManager.isWaveComplete() &&
        waveManager.getCurrentWave() >= WaveManager::TOTAL_WAVES) {
        initGameOver(true);
        return;
    }

    hud->update(gold, lives,
                waveManager.getCurrentWave(), WaveManager::TOTAL_WAVES,
                selectedTowerType, waveManager.isWaveActive());
}

void Game::updateCollisions() {
    for (int pi = 0; pi < projectileCount; ++pi) {
        if (!projectiles[pi].isAlive()) continue;
        for (int ei = 0; ei < enemyCount; ++ei) {
            if (!enemies[ei] || !enemies[ei]->isAlive()) continue;
            float dx   = projectiles[pi].getPosX() - enemies[ei]->getPosX();
            float dy   = projectiles[pi].getPosY() - enemies[ei]->getPosY();
            float dist = std::sqrt(dx * dx + dy * dy);
            if (dist < projectiles[pi].getRadius() + 12.f) {
                if (projectiles[pi].isSlow) {
                    enemies[ei]->applySlow(projectiles[pi].slowFactor,
                                           projectiles[pi].slowDuration);
                }
                enemies[ei]->takeDamage(projectiles[pi].getDamage());
                if (!enemies[ei]->isAlive())
                    gold += enemies[ei]->getGoldReward();
                projectiles[pi].kill();
                break;
            }
        }
    }
}

void Game::cleanupDead() {
    for (int i = enemyCount - 1; i >= 0; --i) {
        if (!enemies[i] || !enemies[i]->isAlive()) {
            delete enemies[i];
            enemies[i] = enemies[--enemyCount];
            enemies[enemyCount] = nullptr;
        }
    }

    for (int i = projectileCount - 1; i >= 0; --i) {
        if (!projectiles[i].isAlive()) {
            projectiles[i] = projectiles[--projectileCount];
        }
    }

    if (selectedTower) {
        bool found = false;
        for (int i = 0; i < towerCount; ++i)
            if (towers[i] == selectedTower) { found = true; break; }
        if (!found) selectedTower = nullptr;
    }
}

void Game::render() {
    window.clear(sf::Color(20, 20, 25));

    if (gameState == GameState::MENU ||
        gameState == GameState::GAME_OVER ||
        gameState == GameState::WIN) {
        window.draw(titleText);
        window.draw(subtitleText);
        window.draw(promptText);
        window.draw(myname);
        window.display();
        return;
    }

    map.render(window);

    if (selectedTower) selectedTower->drawRange(window);

    for (int i = 0; i < towerCount; ++i)      towers[i]->render(window);
    for (int i = 0; i < projectileCount; ++i)  projectiles[i].render(window);
    for (int i = 0; i < enemyCount; ++i)
        if (enemies[i]) enemies[i]->render(window);

    hud->render(window);
    window.display();
}

void Game::initMenu() {
    gameState = GameState::MENU;

    auto centre = [&](sf::Text& t, float y) {
        sf::FloatRect r = t.getLocalBounds();
        t.setOrigin(r.width / 2.f, r.height / 2.f);
        t.setPosition(WIN_W / 2.f, y);
    };

    titleText.setFont(font);
    titleText.setCharacterSize(64);
    titleText.setFillColor(sf::Color(255, 200, 50));
    titleText.setString("Tower Defense Game");
    centre(titleText, WIN_H / 2.f - 120);

    myname.setFont(font);
    myname.setCharacterSize(22);
    myname.setFillColor(sf::Color(255, 200, 200));
    myname.setString("OOP Project by Abu Hurairah 25L-2535");
    centre(myname, WIN_H / 2.f + 100);

    subtitleText.setFont(font);
    subtitleText.setCharacterSize(22);
    subtitleText.setFillColor(sf::Color(200, 200, 200));
    subtitleText.setString("5 Waves | 5 Towers | 5 Enemy Types");
    centre(subtitleText, WIN_H / 2.f - 30);

    promptText.setFont(font);
    promptText.setCharacterSize(20);
    promptText.setFillColor(sf::Color(150, 255, 150));
    promptText.setString("Click anywhere to start  |  R to Restart  |  ESC to Quit");
    centre(promptText, WIN_H / 2.f + 40);
}

void Game::initGame() {
    gameState         = GameState::PLAYING;
    gold              = START_GOLD;
    lives             = START_LIVES;
    selectedTowerType = -1;
    selectedTower     = nullptr;

    clearEnemies();
    clearTowers();
    projectileCount = 0;
    waveManager.reset();

    for (int r = 0; r < Map::ROWS; r++)
        for (int c = 0; c < Map::COLS; c++)
            map.setTower(c, r, false);

    hud->update(gold, lives, 0, WaveManager::TOTAL_WAVES, -1, false);
}

void Game::initGameOver(bool won) {
    gameState = won ? GameState::WIN : GameState::GAME_OVER;

    auto centre = [&](sf::Text& t, float y) {
        sf::FloatRect r = t.getLocalBounds();
        t.setOrigin(r.width / 2.f, r.height / 2.f);
        t.setPosition(WIN_W / 2.f, y);
    };

    titleText.setFont(font);
    titleText.setCharacterSize(72);
    if (won) {
        titleText.setFillColor(sf::Color(50, 255, 100));
        titleText.setString("YOU WIN!");
    } else {
        titleText.setFillColor(sf::Color(255, 60, 60));
        titleText.setString("GAME OVER");
    }
    centre(titleText, WIN_H / 2.f - 100);

    subtitleText.setFont(font);
    subtitleText.setCharacterSize(24);
    subtitleText.setFillColor(sf::Color::White);
    subtitleText.setString(won
        ? std::string("All waves survived!  Gold left: ") + std::to_string(gold)
        : std::string("Base got captured,  Lives: 0"));
    centre(subtitleText, WIN_H / 2.f);

    promptText.setFont(font);
    promptText.setCharacterSize(20);
    promptText.setFillColor(sf::Color(180, 180, 255));
    promptText.setString("Click to return to menu  |  R = Restart");
    centre(promptText, WIN_H / 2.f + 60);
}

Tower* Game::makeTower(int type, float x, float y) {
    switch (type) {
    case 0: return new CannonTower(x, y);
    case 1: return new SniperTower(x, y);
    case 2: return new MachineGunTower(x, y);
    case 3: return new SlowTower(x, y);
    case 4: return new LaserTower(x, y);
    default: return new CannonTower(x, y);
    }
}

int Game::towerCost(int type) const {
    switch (type) {
    case 0: return CannonTower::getBuyCost();
    case 1: return SniperTower::getBuyCost();
    case 2: return MachineGunTower::getBuyCost();
    case 3: return SlowTower::getBuyCost();
    case 4: return LaserTower::getBuyCost();
    default: return 100;
    }
}
