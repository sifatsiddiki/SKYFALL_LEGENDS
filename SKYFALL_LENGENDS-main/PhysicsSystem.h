#pragma once
#include <memory>
#include <vector>

class Player;
class Enemy;
class Bullet;

class PhysicsSystem {
public:
    void handleCollisions(
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
        float& scoreMultiplier);
};
