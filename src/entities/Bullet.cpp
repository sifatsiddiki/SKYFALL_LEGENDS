#include "entities/Bullet.h"

void Bullet::init(sf::Texture& tex, const sf::Vector2f& pos, const sf::Vector2f& vel) {
    sprite.setTexture(tex);
    sprite.setPosition(pos);
    auto bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    velocity = vel;
    alive = true;
}

void Bullet::update(float dt) {
    if (!alive) return;
    sprite.move(velocity * dt);
    auto pos = sprite.getPosition();
    if (pos.y < -50.f || pos.y > 900.f) {
        alive = false;
    }
}
