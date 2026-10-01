#include "modules/physics/PhysicsSystem.h"
#include "entities/Player.h"
#include "entities/Enemy.h"
#include "entities/Bullet.h"

void PhysicsSystem::handleCollisions(
    Player& player,
    std::vector<std::unique_ptr<Enemy>>& enemies,
    std::vector<std::unique_ptr<Bullet>>& playerBullets,
    std::vector<std::unique_ptr<Bullet>>& enemyBullets,
    int& score,
    int& lives,
    int& enemiesDestroyed,
    int& shotsHit,
    int& combo,
    int& maxCombo,
    float& scoreMultiplier)
{
    auto playerBounds = player.getSprite().getGlobalBounds();

    for (auto& b : enemyBullets) {
        if (!b->isAlive()) continue;
        if (b->getSprite().getGlobalBounds().intersects(playerBounds)) {
            b->kill();
            if (player.handleHit()) {
                lives -= 1;
            }
        }
    }

    for (auto& e : enemies) {
        if (!e->isAlive()) continue;

        auto enemyBounds = e->getSprite().getGlobalBounds();

        if (enemyBounds.intersects(playerBounds)) {
            e->kill();
            if (player.handleHit()) {
                lives -= 1;
            }
        }

        for (auto& b : playerBullets) {
            if (!b->isAlive()) continue;
            if (b->getSprite().getGlobalBounds().intersects(enemyBounds)) {
                b->kill();
                e->applyDamage(1);
                shotsHit += 1;
                if (!e->isAlive()) {
                    // increase combo and multiplier on kill
                    combo += 1;
                    if (combo > maxCombo) maxCombo = combo;
                    // simple multiplier: grows slowly up to 3x
                    if (scoreMultiplier < 3.0f) {
                        scoreMultiplier += 0.1f;
                        if (scoreMultiplier > 3.0f) scoreMultiplier = 3.0f;
                    }
                    int baseScore = e->getScoreValue();
                    score += static_cast<int>(baseScore * scoreMultiplier);
                    enemiesDestroyed += 1;
                }
            }
        }
    }
}
