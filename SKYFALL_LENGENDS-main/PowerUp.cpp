#include "entities/PowerUp.h"

void PowerUp::init(sf::Texture& tex, const sf::Vector2f& pos, PowerUpType t) {
    sprite.setTexture(tex);
    auto bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    sprite.setPosition(pos);
    type = t;
    velocity = sf::Vector2f(0.f, 140.f);
    alive = true;
}

void PowerUp::update(float dt) {
    sprite.move(velocity * dt);
    auto pos = sprite.getPosition();
    if (pos.y > 1200.f) {
        alive = false;
    }
}
