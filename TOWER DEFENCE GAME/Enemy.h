#pragma once
#include "Entity.h"
#include "Constants.h"
#include <SFML/Graphics.hpp>

class Enemy : public Entity {
public:
    Enemy(float x, float y, float hp, float spd, int reward, sf::Color col)
        : Entity(x, y, hp), speed(spd), goldReward(reward), color(col),
          waypointCount(0), waypointIndex(0),
          reachedEnd(false), slowTimer(0.f), slowFactor(1.f),
          distTravelled(0.f), textureLoaded(false) {}

    virtual ~Enemy() = default;

    virtual void move(float dt) = 0;
    virtual void attack() = 0;

    void update(float dt) override;
    void render(sf::RenderWindow& window) const override;
    void takeDamage(float dmg) override;

    void setWaypoints(const sf::Vector2f* wp, int count);

    bool  hasReachedEnd()        const { return reachedEnd; }
    int   getGoldReward()        const { return goldReward; }
    float getSpeed()             const { return speed * slowFactor; }
    float getDistanceTravelled() const { return distTravelled; }
    int   getWaypointIndex()     const { return waypointIndex; }

    virtual void applySlow(float factor, float duration);

    void loadTexture(const char* path);

protected:
    float     speed;
    int       goldReward;
    sf::Color color;

    sf::Vector2f waypoints[MAX_WAYPOINTS];
    int          waypointCount;
    int          waypointIndex;

    bool  reachedEnd;
    float slowTimer;
    float slowFactor;
    float distTravelled;
    sf::Texture        texture;
    mutable sf::Sprite sprite;
    bool               textureLoaded;

    void drawHpBar(sf::RenderWindow& window) const;
    void advanceWaypoints(float dt);
};

class BasicEnemy : public Enemy {
public:
    BasicEnemy(float x, float y, int waveScale = 1);
    void move(float dt) override;
    void attack() override {}
};

class FastEnemy : public Enemy {
public:
    FastEnemy(float x, float y, int waveScale = 1);
    void move(float dt) override;
    void attack() override {}
};

class TankEnemy : public Enemy {
public:
    TankEnemy(float x, float y, int waveScale = 1);
    void move(float dt) override;
    void render(sf::RenderWindow& window) const override;
    void attack() override {}
    void applySlow(float, float) override {}  
};

class FlyingEnemy : public Enemy {
public:
    FlyingEnemy(float sx, float sy, float tx, float ty, int waveScale = 1);
    void move(float dt) override;
    void render(sf::RenderWindow& window) const override;
    void attack() override {}
private:
    float        targetX, targetY;
    sf::Vector2f direction;
};

class StealthEnemy : public Enemy {
public:
    StealthEnemy(float x, float y, int waveScale = 1);
    void move(float dt) override;
    void render(sf::RenderWindow& window) const override;
    void attack() override {}
    void reveal() { if (!revealed) { revealed = true; blinkClock.restart(); } }
    bool isRevealed() const { return revealed; }
private:
    bool           revealed = false;
    mutable sf::Clock blinkClock;
};
