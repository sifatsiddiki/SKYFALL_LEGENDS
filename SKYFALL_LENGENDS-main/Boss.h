#pragma once
#include "entities/Entity.h"
#include <memory>

class Bullet;

class Boss : public Entity {
public:
    void init(sf::Texture& tex, const sf::Vector2f& pos);
    void update(float dt) override;

    std::unique_ptr<Bullet> fire(sf::Texture& bulletTex);

    void applyDamage(int amount);
    bool isAlive() const { return alive; }
    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }
    int getScoreValue() const { return scoreValue; }

private:
    sf::Vector2f velocity;
    float fireTimer = 0.f;
    float fireCooldown = 1.5f;
    int health = 40;
    int maxHealth = 40;
    int scoreValue = 1300;
    bool alive = true;
};