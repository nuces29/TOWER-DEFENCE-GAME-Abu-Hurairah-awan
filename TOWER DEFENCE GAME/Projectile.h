#pragma once
#include <SFML/Graphics.hpp>

class Projectile {
public:
    Projectile();
    Projectile(float x, float y, float tx, float ty,
               float spd, float dmg, sf::Color col, float radius = 5.f);

    void update(float dt);
    void render(sf::RenderWindow& window) const;

    bool  isAlive()   const { return alive; }
    float getDamage() const { return damage; }
    float getPosX()   const { return posX; }
    float getPosY()   const { return posY; }
    float getRadius() const { return hitRadius; }
    void  kill()            { alive = false; }

    bool  isSlow       = false;
    float slowFactor   = 1.f;
    float slowDuration = 0.f;

private:
    float posX, posY;
    float velX, velY;
    float damage;
    float hitRadius;
    bool  alive;
    sf::CircleShape shape;
};

class SlowProjectile : public Projectile {
public:
    SlowProjectile(float x, float y, float tx, float ty);
};
