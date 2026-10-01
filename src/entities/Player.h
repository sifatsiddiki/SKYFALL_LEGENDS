#pragma once
#include "entities/Entity.h"
#include <memory>

class Bullet;

class Player : public Entity {
public:
    void init(sf::Texture& tex, const sf::Vector2f& pos);
    void update(float dt, const sf::Vector2f& inputDir, const sf::RenderWindow& window);
    void update(float dt) override {}

    std::unique_ptr<Bullet> fire(sf::Texture& bulletTex);

    // Power-up interface
    void addPowerLevel(int delta);
    void resetPowerLevel();
    int getPowerLevel() const { return powerLevel; }

    // Shield handling: returns true if the hit should reduce lives
    bool handleHit();
    bool hasShield() const { return shieldActive; }

private:
    float speed = 400.f;
    float fireCooldown = 0.15f;
    float fireTimer = 0.f;

    int powerLevel = 0;
    bool shieldActive = false;
    float shieldTimer = 0.f;
};
