#include "entities/Boss.h"
#include "entities/Bullet.h"
#include <cmath>

void Boss::init(sf::Texture& tex, const sf::Vector2f& pos) {
    sprite.setTexture(tex);
    sprite.setPosition(pos);
    auto bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);

    velocity = sf::Vector2f(80.f, 0.f);
    maxHealth = 40;
    health = maxHealth;
    alive = true;
    fireTimer = 0.f;
    fireCooldown = 1.5f;
}

void Boss::update(float dt) {
    if (!alive) return;

    fireTimer += dt;

    sprite.move(velocity * dt);

    sf::Vector2f pos = sprite.getPosition();
    if (pos.x < 150.f) {
        pos.x = 150.f;
        velocity.x = std::fabs(velocity.x);
    } else if (pos.x > 1770.f) {
        pos.x = 1770.f;
        velocity.x = -std::fabs(velocity.x);
    }
    sprite.setPosition(pos);
}

std::unique_ptr<Bullet> Boss::fire(sf::Texture& bulletTex) {
    if (!alive) return nullptr;
    if (fireTimer < fireCooldown) return nullptr;

    fireTimer = 0.f;

    auto bullet = std::make_unique<Bullet>();
    bullet->init(bulletTex, sprite.getPosition() + sf::Vector2f(0.f, 40.f), sf::Vector2f(0.f, 250.f));
    return bullet;
}

void Boss::applyDamage(int amount) {
    if (!alive) return;
    health -= amount;
    if (health <= 0) {
        alive = false;
    }
}