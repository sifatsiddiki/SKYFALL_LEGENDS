#pragma once
#include "entities/Entity.h"

enum class PowerUpType {
    Weapon,
    Shield,
    Bomb,
    Life,
    SlowMo
};

class PowerUp : public Entity {
public:
    void init(sf::Texture& tex, const sf::Vector2f& pos, PowerUpType type);
    void update(float dt) override;

    PowerUpType getType() const { return type; }
    bool isAlive() const { return alive; }
    void kill() { alive = false; }

private:
    PowerUpType type = PowerUpType::Weapon;
    sf::Vector2f velocity;
    bool alive = true;
};
