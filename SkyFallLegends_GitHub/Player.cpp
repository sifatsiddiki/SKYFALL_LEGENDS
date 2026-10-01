#include "entities/Player.h"
#include "entities/Bullet.h"
#include <cmath>

void Player::init(sf::Texture& tex, const sf::Vector2f& pos) {
    sprite.setTexture(tex);
    sprite.setPosition(pos);
    auto bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
}

void Player::update(float dt, const sf::Vector2f& inputDir, const sf::RenderWindow& window) {
    fireTimer += dt;

    // update temporary shield timer if active
    if (shieldActive) {
        shieldTimer -= dt;
        if (shieldTimer <= 0.f) {
            shieldActive = false;
            shieldTimer = 0.f;
        }
    }

    sf::Vector2f dir = inputDir;
    if (dir.x != 0.f || dir.y != 0.f) {
        float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        if (len > 0.f) {
            dir /= len;
        }
    }

    sprite.move(dir * speed * dt);

    auto pos = sprite.getPosition();
    sf::Vector2u size = window.getSize();
    if (pos.x < 30.f) pos.x = 30.f;
    if (pos.x > size.x - 30.f) pos.x = static_cast<float>(size.x - 30.f);
    if (pos.y < 100.f) pos.y = 100.f;
    if (pos.y > size.y - 30.f) pos.y = static_cast<float>(size.y - 30.f);
    sprite.setPosition(pos);
}

std::unique_ptr<Bullet> Player::fire(sf::Texture& bulletTex) {
    if (fireTimer < fireCooldown) {
        return nullptr;
    }
    fireTimer = 0.f;

    auto bullet = std::make_unique<Bullet>();
    bullet->init(bulletTex, sprite.getPosition(), sf::Vector2f(0.f, -600.f));
    return bullet;
}

void Player::addPowerLevel(int delta) {
    powerLevel += delta;
    if (powerLevel < 0) powerLevel = 0;
    if (powerLevel > 3) powerLevel = 3;
}

void Player::resetPowerLevel() {
    powerLevel = 0;
}

bool Player::handleHit() {
    if (shieldActive) {
        // absorb this hit
        shieldActive = false;
        shieldTimer = 0.f;
        return false;
    }
    return true;
}

