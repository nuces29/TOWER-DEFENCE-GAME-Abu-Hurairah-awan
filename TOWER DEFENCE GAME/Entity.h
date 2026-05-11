#pragma once
#include <SFML/Graphics.hpp>

class Entity {
public:
    Entity(float x, float y, float hp)
        : posX(x), posY(y), maxHp(hp), currentHp(hp), alive(true) {}

    virtual ~Entity() = default;

    virtual void update(float dt) = 0;
    virtual void render(sf::RenderWindow& window) const = 0;
    virtual void takeDamage(float dmg) = 0;
    
    float getPosX()  const { return posX; }
    float getPosY()  const { return posY; }
    float getHp()    const { return currentHp; }
    float getMaxHp() const { return maxHp; }
    bool  isAlive()  const { return alive; }

    void setPosX(float x) { posX = x; }
    void setPosY(float y) { posY = y; }
    void setAlive(bool a) { alive = a; }

    bool operator<(const Entity& o) const { return currentHp < o.currentHp; }
    bool operator>(const Entity& o) const { return currentHp > o.currentHp; }

protected:
    float posX, posY;
    float maxHp, currentHp;
    bool  alive;
};
