#pragma once
#include "entities/Entity.h"

class Bullet : public Entity {
public:
    void init(sf::Texture& tex, const sf::Vector2f& pos, const sf::Vector2f& vel);
    void update(float dt) override;

    bool isAlive() const { return alive; }
    void kill() { alive = false; }

private:
    sf::Vector2f velocity;
    bool alive = true;
};
