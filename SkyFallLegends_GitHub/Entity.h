#pragma once
#include <SFML/Graphics.hpp>

class Entity {
public:
    virtual ~Entity() = default;

    virtual void update(float dt) = 0;

    sf::Sprite& getSprite() { return sprite; }
    const sf::Sprite& getSprite() const { return sprite; }

protected:
    sf::Sprite sprite;
};
