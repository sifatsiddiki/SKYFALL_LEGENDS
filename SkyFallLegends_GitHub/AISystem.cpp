#include "modules/ai/AISystem.h"
#include <cstdlib>

bool AISystem::init() {
    spawnTimer = 0.f;
    spawnInterval = 1.0f;
    difficulty = Difficulty::Normal;
    return true;
}

void AISystem::update(float dt,
                      std::vector<std::unique_ptr<Enemy>>& enemies,
                      int currentLevel,
                      Renderer& renderer)
{
    spawnTimer += dt;

    float difficultyFactor = 1.0f;
    if (difficulty == Difficulty::Easy) difficultyFactor = 1.3f;
    else if (difficulty == Difficulty::Hard) difficultyFactor = 0.7f;

    float levelFactor = 1.0f - 0.1f * static_cast<float>(currentLevel - 1);
    if (levelFactor < 0.5f) levelFactor = 0.5f;

    float targetInterval = spawnInterval * difficultyFactor * levelFactor;

    if (spawnTimer >= targetInterval) {
        spawnTimer = 0.f;

        // number & behaviour of enemies scale with current level
        int count = 1;
        if (currentLevel == 2) {
            count = 2;
        } else if (currentLevel >= 3) {
            count = 3;
        }

        for (int i = 0; i < count; ++i) {
            auto enemy = std::make_unique<Enemy>();
            float x = static_cast<float>(50 + std::rand() % 900);

            int patternLevel = 1;
            if (currentLevel == 1)      patternLevel = 1; // simple straight
            else if (currentLevel == 2) patternLevel = 2; // wave / zig-zag
            else                        patternLevel = 3; // fast kamikaze style

            // choose sprite variant based on pattern level
            std::string enemyTexId = "enemy_A";
            if (patternLevel == 1)      enemyTexId = "enemy_A";
            else if (patternLevel == 2) enemyTexId = "enemy_B";
            else                        enemyTexId = "enemy_C";

            enemy->init(renderer.getTexture(enemyTexId), sf::Vector2f(x, -50.f), patternLevel);
            enemies.push_back(std::move(enemy));
        }
    }

    for (auto& e : enemies) {
        e->updateAI(dt);
    }
}
