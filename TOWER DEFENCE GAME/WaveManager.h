#pragma once
#include "Enemy.h"
#include "Constants.h"
#include <SFML/Graphics.hpp>

class WaveManager {
public:
    static const int TOTAL_WAVES = 5;
    static const int MAX_QUEUE   = 25;

    WaveManager();

    void startWave(int waveNumber);

    void update(float dt,
                Enemy**              enemies,
                int&                 enemyCount,
                const sf::Vector2f*  waypoints,
                int                  waypointCount,
                float entryX, float entryY,
                float exitX,  float exitY);

    bool isWaveActive()   const { return waveActive; }
    bool isWaveComplete() const { return waveComplete; }
    int  getCurrentWave() const { return currentWave; }

    void reset();

private:
    struct SpawnEntry {
        int   type;     
        int   count;
        float interval;
    };

    int  currentWave;
    bool waveActive;
    bool waveComplete;
    float spawnTimer;

    SpawnEntry spawnQueue[MAX_QUEUE];
    int        queueSize;
    int        queueIndex;
    int        queueSpawned;

    void buildWaveQueue(int wave);
};
