#pragma once
#include "entities/Entity.h"
#include <memory>

class Bullet;

enum class EnemyState {
    Entering,
    Attacking,
    Leaving,
    Dead
};

class Enemy : public Entity {
public:
    void init(sf::Texture& tex, const sf::Vector2f& pos, int patternLevel);
    void update(float dt) override;
    void updateAI(float dt);

    std::unique_ptr<Bullet> fire(sf::Texture& bulletTex);

    bool isAlive() const { return state != EnemyState::Dead; }
    void kill() { state = EnemyState::Dead; }

    void applyDamage(int amount);
    int getScoreValue() const { return scoreValue; }

private:
    EnemyState state = EnemyState::Entering;
    sf::Vector2f velocity;
    float attackTimer = 0.f;
    float fireTimer = 0.f;
    float fireCooldown = 1.0f;
    int health = 3;
    int patternLevel = 1;
    int scoreValue = 100;
};
