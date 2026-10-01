#include "modules/ai/WaveManager.h"
#include <algorithm>

void WaveManager::setWaves(const std::vector<Wave>& waves) {
    m_waves = waves;
    m_spawnedCount.assign(m_waves.size(), 0);
}

void WaveManager::reset() {
    std::fill(m_spawnedCount.begin(), m_spawnedCount.end(), 0);
}

void WaveManager::update(float levelTime,
                         std::function<void(const Wave&, int)> spawnFn) {
    if (m_waves.empty()) return;
    if (m_spawnedCount.size() != m_waves.size()) {
        m_spawnedCount.assign(m_waves.size(), 0);
    }

    for (std::size_t i = 0; i < m_waves.size(); ++i) {
        const Wave& w = m_waves[i];

        float dt = levelTime - w.spawnTime;
        if (dt < 0.f) continue; // not started yet

        int shouldHaveSpawned = 0;
        if (w.spawnInterval > 0.f) {
            shouldHaveSpawned = static_cast<int>(dt / w.spawnInterval) + 1;
        } else {
            // spawn all at once
            shouldHaveSpawned = w.count;
        }

        if (shouldHaveSpawned > w.count) {
            shouldHaveSpawned = w.count;
        }

        while (m_spawnedCount[i] < shouldHaveSpawned) {
            spawnFn(w, m_spawnedCount[i]);
            m_spawnedCount[i]++;
        }
    }
}

bool WaveManager::isFinished() const {
    if (m_waves.empty()) return true;
    if (m_spawnedCount.size() != m_waves.size()) return false;
    for (std::size_t i = 0; i < m_waves.size(); ++i) {
        if (m_spawnedCount[i] < m_waves[i].count) {
            return false;
        }
    }
    return true;
}
