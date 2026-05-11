#include "Projectile.h"
#include <cmath>

Projectile::Projectile()
    : posX(0), posY(0), velX(0), velY(0),
      damage(0), hitRadius(5.f), alive(false) {}

Projectile::Projectile(float x, float y, float tx, float ty,
                       float spd, float dmg, sf::Color col, float radius)
    : posX(x), posY(y), damage(dmg), hitRadius(radius), alive(true)
{
    float dx = tx - x, dy = ty - y;
    float len = std::sqrt(dx * dx + dy * dy);
    if (len > 0.f) { velX = (dx / len) * spd; velY = (dy / len) * spd; }
    else           { velX = 0.f; velY = spd; }

    shape.setRadius(radius);
    shape.setFillColor(col);
    shape.setOrigin(radius, radius);
}

void Projectile::update(float dt) {
    if (!alive) return;
    posX += velX * dt;
    posY += velY * dt;
    shape.setPosition(posX, posY);
    if (posX < -100.f || posX > 2000.f || posY < -100.f || posY > 2000.f)
        alive = false;
}

void Projectile::render(sf::RenderWindow& window) const {
    if (!alive) return;
    window.draw(shape);
}

SlowProjectile::SlowProjectile(float x, float y, float tx, float ty)
    : Projectile(x, y, tx, ty, 200.f, 5.f, sf::Color(100, 150, 255), 6.f)
{
    isSlow       = true;
    slowFactor   = 0.4f;
    slowDuration = 3.0f;
}
