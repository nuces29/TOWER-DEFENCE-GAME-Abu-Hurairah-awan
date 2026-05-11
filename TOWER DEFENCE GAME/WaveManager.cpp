#include "WaveManager.h"
#include <cstring>

WaveManager::WaveManager()
    : currentWave(0), waveActive(false), waveComplete(false),
      spawnTimer(0.f), queueSize(0), queueIndex(0), queueSpawned(0) {}

void WaveManager::reset() {
    currentWave  = 0;
    waveActive   = false;
    waveComplete = false;
    spawnTimer   = 0.f;
    queueSize    = 0;
    queueIndex   = 0;
    queueSpawned = 0;
}

void WaveManager::buildWaveQueue(int wave) {
    queueSize    = 0;
    queueIndex   = 0;
    queueSpawned = 0;

    auto push = [&](int type, int count, float interval) {
        if (queueSize < MAX_QUEUE) {
            spawnQueue[queueSize++] = { type, count, interval };
        }
    };

    switch (wave) {
    case 1:
        push(0,  8, 0.9f);                         // 8 Basic
        break;
    case 2:
        push(0,  6, 0.8f);
        push(1,  5, 0.6f);                         // + Fast
        break;
    case 3:
        push(0,  8, 0.7f);
        push(1,  6, 0.5f);
        push(2,  2, 1.5f);                         // + Tanks
        break;
    case 4:
        push(0,  6, 0.7f);
        push(1,  8, 0.45f);
        push(2,  3, 1.4f);
        push(3,  4, 1.0f);                         // + Flying
        break;
    case 5:
        push(0,  8, 0.6f);
        push(1,  8, 0.4f);
        push(2,  4, 1.2f);
        push(3,  5, 0.9f);
        push(4,  5, 0.8f);                         // + Stealth
        break;
    default:
        push(0,  5, 1.0f);
        break;
    }
}

void WaveManager::startWave(int waveNumber) {
    currentWave  = waveNumber;
    waveActive   = true;
    waveComplete = false;
    spawnTimer   = 0.f;
    buildWaveQueue(waveNumber);
}

void WaveManager::update(float dt,
                          Enemy**             enemies,
                          int&                enemyCount,
                          const sf::Vector2f* waypoints,
                          int                 waypointCount,
                          float entryX, float entryY,
                          float exitX,  float exitY) {
    if (!waveActive) return;

    bool allSpawned = (queueIndex >= queueSize);
    if (allSpawned) {
        bool anyAlive = false;
        for (int i = 0; i < enemyCount; ++i)
            if (enemies[i] && enemies[i]->isAlive()) { anyAlive = true; break; }
        if (!anyAlive) {
            waveActive   = false;
            waveComplete = true;
        }
        return;
    }

    spawnTimer += dt;
    SpawnEntry& entry = spawnQueue[queueIndex];
    if (spawnTimer < entry.interval) return;
    spawnTimer = 0.f;

    if (enemyCount >= MAX_ENEMIES) return;  

    int ws = currentWave;   
    Enemy* e = nullptr;

    switch (entry.type) {
    case 0: {
        BasicEnemy* be = new BasicEnemy(entryX, entryY, ws);
        be->setWaypoints(waypoints, waypointCount);
        e = be;
        break;
    }
    case 1: {
        FastEnemy* fe = new FastEnemy(entryX, entryY, ws);
        fe->setWaypoints(waypoints, waypointCount);
        e = fe;
        break;
    }
    case 2: {
        TankEnemy* te = new TankEnemy(entryX, entryY, ws);
        te->setWaypoints(waypoints, waypointCount);
        e = te;
        break;
    }
    case 3: {
        e = new FlyingEnemy(entryX, entryY, exitX, exitY, ws);
        break;
    }
    case 4: {
        StealthEnemy* se = new StealthEnemy(entryX, entryY, ws);
        se->setWaypoints(waypoints, waypointCount);
        e = se;
        break;
    }
    }

    if (e) enemies[enemyCount++] = e;

    queueSpawned++;
    if (queueSpawned >= entry.count) {
        queueIndex++;
        queueSpawned = 0;
    }
}
