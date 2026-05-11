#pragma once
#include "Entity.h"
#include "Projectile.h"
#include "Constants.h"
#include <SFML/Graphics.hpp>

class Enemy;

class Tower : public Entity {
public:
    Tower(float x, float y, float hp, float range, float fireRate,
          float dmg, int cost, sf::Color col)
        : Entity(x, y, hp), range(range), fireRate(fireRate),
          damage(dmg), cost(cost), color(col),
          fireCooldown(0.f), textureLoaded(false) {}

    virtual ~Tower() = default;

    virtual void attack(Enemy** enemies, int enemyCount,
                        Projectile* projectiles, int& projCount) = 0;

    void takeDamage(float dmg) override {
        currentHp -= dmg;
        if (currentHp <= 0.f) alive = false;
    }
    void update(float dt) override {
        if (fireCooldown > 0.f) fireCooldown -= dt;
    }
    void render(sf::RenderWindow& window) const override;

    float getRange()  const { return range; }
    float getDamage() const { return damage; }
    int   getCost()   const { return cost; }
    void  drawRange(sf::RenderWindow& window) const;

    void loadTexture(const char* path, float displaySize = 32.f);

protected:
    float     range;
    float     fireRate;
    float     damage;
    int       cost;
    sf::Color color;
    float     fireCooldown;

    sf::Texture        texture;
    mutable sf::Sprite sprite;  
    bool               textureLoaded;

    Enemy* findTarget(Enemy** enemies, int count);
    Enemy* findWeakestTarget(Enemy** enemies, int count); 
};

class CannonTower : public Tower {
public:
    CannonTower(float x, float y);
    void attack(Enemy** enemies, int enemyCount,
                Projectile* projectiles, int& projCount) override;
    static int getBuyCost() { return 100; }
};

class SniperTower : public Tower {
public:
    SniperTower(float x, float y);
    void attack(Enemy** enemies, int enemyCount,
                Projectile* projectiles, int& projCount) override;
    static int getBuyCost() { return 125; }
};

class MachineGunTower : public Tower {
public:
    MachineGunTower(float x, float y);
    void attack(Enemy** enemies, int enemyCount,
                Projectile* projectiles, int& projCount) override;
    static int getBuyCost() { return 150; }
};

class SlowTower : public Tower {
public:
    SlowTower(float x, float y);
    void attack(Enemy** enemies, int enemyCount,
                Projectile* projectiles, int& projCount) override;
    static int getBuyCost() { return 100; }
};

class LaserTower : public Tower {
public:
    LaserTower(float x, float y);
    void attack(Enemy** enemies, int enemyCount,
                Projectile* projectiles, int& projCount) override;
    void render(sf::RenderWindow& window) const override;
    static int getBuyCost() { return 175; }
private:
    mutable sf::VertexArray laserBeam;
    mutable float beamAlpha = 0.f;
};
