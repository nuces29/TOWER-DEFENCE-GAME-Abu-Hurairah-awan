#include "Tower.h"
#include "Enemy.h"
#include <cmath>

void Tower::loadTexture(const char* path, float displaySize) {
    if (texture.loadFromFile(path)) {
        sprite.setTexture(texture);
        sf::Vector2u sz = texture.getSize();
        float sx = displaySize / static_cast<float>(sz.x);
        float sy = displaySize / static_cast<float>(sz.y);
        sprite.setScale(sx, sy);
        sprite.setOrigin(sz.x / 2.f, sz.y / 2.f);
        textureLoaded = true;
    }
}

Enemy* Tower::findTarget(Enemy** enemies, int count) {
    Enemy* best     = nullptr;
    float  bestDist = 0.f;
    for (int i = 0; i < count; ++i) {
        Enemy* e = enemies[i];
        if (!e || !e->isAlive()) continue;
        float dx = e->getPosX() - posX;
        float dy = e->getPosY() - posY;
        float dist = std::sqrt(dx * dx + dy * dy);
        if (dist <= range) {
            if (!best || e->getDistanceTravelled() > bestDist) {
                best     = e;
                bestDist = e->getDistanceTravelled();
            }
        }
    }
    return best;
}

Enemy* Tower::findWeakestTarget(Enemy** enemies, int count) {
    Enemy* best = nullptr;
    for (int i = 0; i < count; ++i) {
        Enemy* e = enemies[i];
        if (!e || !e->isAlive()) continue;
        float dx = e->getPosX() - posX;
        float dy = e->getPosY() - posY;
        float dist = std::sqrt(dx * dx + dy * dy);
        if (dist <= range) {
            if (!best || (*e < *best)) best = e;
        }
    }
    return best;
}

void Tower::render(sf::RenderWindow& window) const {
    if (textureLoaded) {
        sprite.setPosition(posX, posY);
        window.draw(sprite);
        return;
    }
    float sz = 20.f;
    sf::RectangleShape base(sf::Vector2f(sz, sz));
    base.setFillColor(color);
    base.setOutlineColor(sf::Color::Black);
    base.setOutlineThickness(2.f);
    base.setOrigin(sz / 2.f, sz / 2.f);
    base.setPosition(posX, posY);
    window.draw(base);

    sf::RectangleShape barrel(sf::Vector2f(14.f, 5.f));
    barrel.setFillColor(sf::Color(50, 50, 50));
    barrel.setOrigin(0.f, 2.5f);
    barrel.setPosition(posX, posY);
    window.draw(barrel);
}

void Tower::drawRange(sf::RenderWindow& window) const {
    sf::CircleShape circle(range);
    circle.setOrigin(range, range);
    circle.setPosition(posX, posY);
    circle.setFillColor(sf::Color(255, 255, 255, 20));
    circle.setOutlineColor(sf::Color(255, 255, 255, 80));
    circle.setOutlineThickness(1.f);
    window.draw(circle);
}

CannonTower::CannonTower(float x, float y)
    : Tower(x, y, 200.f, 130.f, 0.6f, 60.f, getBuyCost(), sf::Color(80, 80, 80))
{
    loadTexture("assets/tower_cannon.png", 40.f);
}

void CannonTower::attack(Enemy** enemies, int enemyCount,
                         Projectile* projectiles, int& projCount) {
    if (fireCooldown > 0.f) return;
    Enemy* target = findTarget(enemies, enemyCount);
    if (!target) return;
    if (projCount < MAX_PROJECTILES) {
        projectiles[projCount++] = Projectile(posX, posY,
            target->getPosX(), target->getPosY(),
            500.f, damage, sf::Color(80, 80, 80), 7.f);
    }
    fireCooldown = 1.f / fireRate;
}

SniperTower::SniperTower(float x, float y)
    : Tower(x, y, 150.f, 280.f, 0.8f, 45.f, getBuyCost(), sf::Color(220, 200, 50))
{
    loadTexture("assets/tower_sniper.png", 40.f);
}

void SniperTower::attack(Enemy** enemies, int enemyCount,
                         Projectile* projectiles, int& projCount) {
    if (fireCooldown > 0.f) return;
    Enemy* target = findWeakestTarget(enemies, enemyCount);
    if (!target) return;
    if (projCount < MAX_PROJECTILES) {
        projectiles[projCount++] = Projectile(posX, posY,
            target->getPosX(), target->getPosY(),
            900.f, damage, sf::Color(255, 240, 0), 5.f);
    }
    fireCooldown = 1.f / fireRate;
}

MachineGunTower::MachineGunTower(float x, float y)
    : Tower(x, y, 150.f, 110.f, 5.0f, 12.f, getBuyCost(), sf::Color(200, 100, 30))
{
    loadTexture("assets/tower_machinegun.png", 50.f);
}

void MachineGunTower::attack(Enemy** enemies, int enemyCount,
                              Projectile* projectiles, int& projCount) {
    if (fireCooldown > 0.f) return;
    Enemy* target = findTarget(enemies, enemyCount);
    if (!target) return;
    if (projCount < MAX_PROJECTILES) {
        projectiles[projCount++] = Projectile(posX, posY,
            target->getPosX(), target->getPosY(),
            700.f, damage, sf::Color(255, 150, 50), 3.f);
    }
    fireCooldown = 1.f / fireRate;
}

SlowTower::SlowTower(float x, float y)
    : Tower(x, y, 150.f, 120.f, 1.2f, 5.f, getBuyCost(), sf::Color(60, 100, 220))
{
    loadTexture("assets/tower_slow.png", 40.f);
}

void SlowTower::attack(Enemy** enemies, int enemyCount,
                       Projectile* projectiles, int& projCount) {
    if (fireCooldown > 0.f) return;
    Enemy* target = findTarget(enemies, enemyCount);
    if (!target) return;
    if (projCount < MAX_PROJECTILES) {
        SlowProjectile sp(posX, posY, target->getPosX(), target->getPosY());
        projectiles[projCount++] = sp;
    }
    fireCooldown = 1.f / fireRate;
}

LaserTower::LaserTower(float x, float y)
    : Tower(x, y, 180.f, 200.f, 0.5f, 20.f, getBuyCost(), sf::Color(180, 50, 220)),
      laserBeam(sf::Lines, 2)
{
    loadTexture("assets/tower_laser.png", 40.f);
}

void LaserTower::attack(Enemy** enemies, int enemyCount,
                        Projectile* projectiles, int& projCount) {
    if (fireCooldown > 0.f) return;
    Enemy* primary = findTarget(enemies, enemyCount);
    if (!primary) return;

    for (int i = 0; i < enemyCount; ++i) {
        Enemy* e = enemies[i];
        if (!e || !e->isAlive()) continue;
        float dx = e->getPosX() - posX;
        float dy = e->getPosY() - posY;
        if (std::sqrt(dx * dx + dy * dy) <= range)
            e->takeDamage(damage);
    }

    laserBeam[0] = sf::Vertex(sf::Vector2f(posX, posY),
                               sf::Color(220, 80, 255, 200));
    laserBeam[1] = sf::Vertex(sf::Vector2f(primary->getPosX(), primary->getPosY()),
                               sf::Color(220, 80, 255, 0));
    beamAlpha    = 1.f;
    fireCooldown = 1.f / fireRate;
    (void)projectiles;
    (void)projCount;
}

void LaserTower::render(sf::RenderWindow& window) const {
    Tower::render(window);
    if (beamAlpha > 0.f) {
        window.draw(laserBeam);
        beamAlpha -= 0.05f;
        if (beamAlpha < 0.f) beamAlpha = 0.f;
    }
}
