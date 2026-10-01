#pragma once
#include <vector>
#include <memory>
#include "entities/Enemy.h"
#include "modules/rendering/Renderer.h"

enum class Difficulty {
    Easy,
    Normal,
    Hard
};

class AISystem {
public:
    bool init();
    void update(float dt,
                std::vector<std::unique_ptr<Enemy>>& enemies,
                int currentLevel,
                Renderer& renderer);

    void setDifficulty(Difficulty d) { difficulty = d; }

private:
    float spawnTimer = 0.f;
    float spawnInterval = 1.0f;
    Difficulty difficulty = Difficulty::Normal;
};
