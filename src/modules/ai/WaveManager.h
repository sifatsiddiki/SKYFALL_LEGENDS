#pragma once
#include <vector>
#include <functional>
#include <SFML/System/Vector2.hpp>

enum class EnemyType {
    Basic,
    ZigZag,
    FastKamikaze,
    Tanky,
    BossMinion
};

struct Wave {
    float spawnTime;          // seconds since level start
    EnemyType enemyType;
    int pathPattern;          // 1 = straight, 2 = zigzag, 3 = fast, etc.
    int count;                // how many enemies in this wave
    float spawnInterval;      // time between enemies in this wave
    float startX;             // base spawn X
    float startY;             // base spawn Y (usually off-screen top)
};

class WaveManager {
public:
    void setWaves(const std::vector<Wave>& waves);
    void reset();
    void update(float levelTime,
                std::function<void(const Wave&, int indexInWave)> spawnFn);

    bool isFinished() const;

private:
    std::vector<Wave> m_waves;
    std::vector<int>  m_spawnedCount; // how many from each wave already spawned
};
