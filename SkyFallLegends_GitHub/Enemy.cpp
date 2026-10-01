#include "entities/Enemy.h"
#include "entities/Bullet.h"
#include <cmath>

void Enemy::init(sf::Texture& tex, const sf::Vector2f& pos, int pattern) {
    sprite.setTexture(tex);
    sprite.setPosition(pos);
    auto bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    patternLevel = pattern;
    // base stats
    velocity = sf::Vector2f(0.f, 120.f);
    state = EnemyState::Entering;
    health = 3;
    // simple scoring tuned so that fixed enemy counts match level target scores
    scoreValue = 300;
    fireCooldown = 1.0f;
    attackTimer = 0.f;
    fireTimer = 0.f;

    if (patternLevel == 2) {
        // slightly tougher wavy enemies
        health = 4;
        fireCooldown = 0.9f;
    } else if (patternLevel >= 3) {
        // fast kamikaze style
        health = 2;
        fireCooldown = 0.7f;
    }
}

void Enemy::update(float dt) {
    sprite.move(velocity * dt);
}

void Enemy::updateAI(float dt) {
    attackTimer += dt;
    fireTimer += dt;

    sf::Vector2f pos = sprite.getPosition();

    if (patternLevel == 1) {
        // simple straight-down movement
        velocity = sf::Vector2f(0.f, 140.f);
    } else if (patternLevel == 2) {
        // classic zig-zag / wave
        if (state == EnemyState::Entering) {
            if (pos.y > 200.f) {
                state = EnemyState::Attacking;
            }
        } else if (state == EnemyState::Attacking) {
            velocity.y = 60.f;
            velocity.x = 90.f * std::sin(attackTimer * 2.f);
        } else if (state == EnemyState::Leaving) {
            velocity = sf::Vector2f(0.f, -200.f);
            if (pos.y < -50.f) {
                state = EnemyState::Dead;
            }
        }
    } else {
        // fast kamikaze: dive straight towards bottom
        velocity = sf::Vector2f(0.f, 260.f);
    }

    if (pos.y > 800.f) {
        state = EnemyState::Dead;
    }
}

std::unique_ptr<Bullet> Enemy::fire(sf::Texture& bulletTex) {
    if (patternLevel == 3) {
        // kamikaze enemies do not shoot, they just rush
        return nullptr;
    }

    if (fireTimer < fireCooldown) return nullptr;

    fireTimer = 0.f;
    auto bullet = std::make_unique<Bullet>();
    bullet->init(bulletTex, sprite.getPosition(), sf::Vector2f(0.f, 300.f));
    return bullet;
}

void Enemy::applyDamage(int amount) {
    health -= amount;
    if (health <= 0) {
        state = EnemyState::Dead;
    }
}