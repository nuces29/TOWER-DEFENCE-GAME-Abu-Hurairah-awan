#include "Enemy.h"
#include <cmath>
#include <algorithm>
#include <cstring>

// ---------------------------------------------------------------
// Enemy (base)
// ---------------------------------------------------------------
void Enemy::loadTexture(const char* path) {
    if (texture.loadFromFile(path)) {
        // Scale 512x512 sprite down to 28x28 in-game pixels
        sprite.setTexture(texture);
        sf::Vector2u sz = texture.getSize();
        float scaleX = 28.f / static_cast<float>(sz.x);
        float scaleY = 28.f / static_cast<float>(sz.y);
        sprite.setScale(scaleX, scaleY);
        sprite.setOrigin(sz.x / 2.f, sz.y / 2.f);
        textureLoaded = true;
    }
}

void Enemy::setWaypoints(const sf::Vector2f* wp, int count) {
    waypointCount = (count > MAX_WAYPOINTS) ? MAX_WAYPOINTS : count;
    for (int i = 0; i < waypointCount; ++i) waypoints[i] = wp[i];
    waypointIndex = 0;
}

void Enemy::update(float dt) {
    if (!alive) return;
    if (slowTimer > 0.f) {
        slowTimer -= dt;
        if (slowTimer <= 0.f) slowFactor = 1.f;
    }
    move(dt);
}

void Enemy::takeDamage(float dmg) {
    currentHp -= dmg;
    if (currentHp <= 0.f) { currentHp = 0.f; alive = false; }
}

void Enemy::applySlow(float factor, float duration) {
    slowFactor = factor;
    slowTimer  = duration;
}

void Enemy::advanceWaypoints(float dt) {
    if (waypointCount == 0 || waypointIndex >= waypointCount) {
        reachedEnd = true;
        alive = false;
        return;
    }
    sf::Vector2f target = waypoints[waypointIndex];
    float dx   = target.x - posX;
    float dy   = target.y - posY;
    float dist = std::sqrt(dx * dx + dy * dy);
    float step = speed * slowFactor * dt;

    if (dist <= step) {
        posX = target.x;
        posY = target.y;
        distTravelled += dist;
        waypointIndex++;
        if (waypointIndex >= waypointCount) {
            reachedEnd = true;
            alive = false;
        }
    } else {
        posX += (dx / dist) * step;
        posY += (dy / dist) * step;
        distTravelled += step;
    }
}

void Enemy::render(sf::RenderWindow& window) const {
    if (!alive) return;
    if (textureLoaded) {
        sprite.setPosition(posX, posY);
        window.draw(sprite);
    } else {
        // Fallback circle if texture missing
        float r = 14.f;
        sf::CircleShape body(r);
        body.setFillColor(color);
        body.setOutlineColor(sf::Color::Black);
        body.setOutlineThickness(1.f);
        body.setOrigin(r, r);
        body.setPosition(posX, posY);
        window.draw(body);
    }
    drawHpBar(window);
}

void Enemy::drawHpBar(sf::RenderWindow& window) const {
    float w = 28.f, h = 5.f;
    float x = posX - w / 2.f;
    float y = posY - 22.f;

    sf::RectangleShape bg(sf::Vector2f(w, h));
    bg.setFillColor(sf::Color(80, 0, 0));
    bg.setPosition(x, y);
    window.draw(bg);

    float ratio = std::max(0.f, currentHp / maxHp);
    sf::RectangleShape fill(sf::Vector2f(w * ratio, h));
    fill.setFillColor(sf::Color(50, 220, 50));
    fill.setPosition(x, y);
    window.draw(fill);
}

BasicEnemy::BasicEnemy(float x, float y, int ws)
    : Enemy(x, y, 80.f * ws, 70.f, 30 * ws, sf::Color(220, 60, 60))
{
    loadTexture("assets/enemy_basic.png");
}

void BasicEnemy::move(float dt) {
    advanceWaypoints(dt);
}

FastEnemy::FastEnemy(float x, float y, int ws)
    : Enemy(x, y, 35.f * ws, 150.f, 18 * ws, sf::Color(255, 200, 0))
{
    loadTexture("assets/enemy_fast.png");
}

void FastEnemy::move(float dt) {
    advanceWaypoints(dt);
}

TankEnemy::TankEnemy(float x, float y, int ws)
    : Enemy(x, y, 400.f * ws, 30.f, 75 * ws, sf::Color(80, 80, 200))
{
    loadTexture("assets/enemy_tank.png");
    if (textureLoaded) {
        sf::Vector2u sz = texture.getSize();
        float scaleX = 50.f / static_cast<float>(sz.x);
        float scaleY = 50.f / static_cast<float>(sz.y);
        sprite.setScale(scaleX, scaleY);
    }
}

void TankEnemy::move(float dt) {
    advanceWaypoints(dt);
}

void TankEnemy::render(sf::RenderWindow& window) const {
    if (!alive) return;
    if (textureLoaded) {
        sprite.setPosition(posX, posY);
        window.draw(sprite);
    } else {
        float sz = 22.f;
        sf::RectangleShape body(sf::Vector2f(sz, sz));
        body.setFillColor(color);
        body.setOutlineColor(sf::Color::Black);
        body.setOutlineThickness(2.f);
        body.setOrigin(sz / 2.f, sz / 2.f);
        body.setPosition(posX, posY);
        window.draw(body);
    }
    drawHpBar(window);
}

FlyingEnemy::FlyingEnemy(float sx, float sy, float tx, float ty, int ws)
    : Enemy(sx, sy, 50.f * ws, 90.f, 24 * ws, sf::Color(50, 230, 200)),
      targetX(tx), targetY(ty)
{
    float dx = tx - sx, dy = ty - sy;
    float len = std::sqrt(dx * dx + dy * dy);
    if (len > 0.f) { direction.x = dx / len; direction.y = dy / len; }
    loadTexture("assets/enemy_flying.png");
}

void FlyingEnemy::move(float dt) {
    float step = speed * slowFactor * dt;
    posX += direction.x * step;
    posY += direction.y * step;
    distTravelled += step;
    float dx = targetX - posX, dy = targetY - posY;
    if (std::sqrt(dx * dx + dy * dy) < 10.f) {
        reachedEnd = true;
        alive = false;
    }
}

void FlyingEnemy::render(sf::RenderWindow& window) const {
    if (!alive) return;
    if (textureLoaded) {
        sprite.setPosition(posX, posY);
        window.draw(sprite);
    } else {
        sf::ConvexShape diamond;
        diamond.setPointCount(4);
        float sz = 12.f;
        diamond.setPoint(0, sf::Vector2f(0,   -sz));
        diamond.setPoint(1, sf::Vector2f(sz,    0));
        diamond.setPoint(2, sf::Vector2f(0,    sz));
        diamond.setPoint(3, sf::Vector2f(-sz,   0));
        diamond.setFillColor(color);
        diamond.setOutlineColor(sf::Color::Black);
        diamond.setOutlineThickness(1.f);
        diamond.setPosition(posX, posY);
        window.draw(diamond);
    }
    drawHpBar(window);
}

StealthEnemy::StealthEnemy(float x, float y, int ws)
    : Enemy(x, y, 60.f * ws, 90.f, 36 * ws, sf::Color(180, 50, 220))
{
    loadTexture("assets/enemy_stealth.png");
}

void StealthEnemy::move(float dt) {
    advanceWaypoints(dt);
}

void StealthEnemy::render(sf::RenderWindow& window) const {
    if (!alive) return;

    sf::Uint8 alpha = 255;
    if (!revealed) {
        alpha = 55;   
    } else {
        float t = blinkClock.getElapsedTime().asSeconds();
        alpha = static_cast<sf::Uint8>((std::sin(t * 10.f) > 0.f) ? 220 : 80);
    }

    if (textureLoaded) {
        sf::Color c(255, 255, 255, alpha);
        sprite.setColor(c);
        sprite.setPosition(posX, posY);
        window.draw(sprite);
        sprite.setColor(sf::Color::White);   
    } else {
        float r = 14.f;
        sf::CircleShape body(r);
        sf::Color c = color;
        c.a = alpha;
        body.setFillColor(c);
        body.setOutlineColor(sf::Color(0, 0, 0, alpha));
        body.setOutlineThickness(1.f);
        body.setOrigin(r, r);
        body.setPosition(posX, posY);
        window.draw(body);
    }

    if (revealed) drawHpBar(window);
}
