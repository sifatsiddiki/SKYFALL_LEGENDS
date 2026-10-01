#include "entities/Explosion.h"

void Explosion::init(sf::Texture& tex, const sf::Vector2f& pos,
                     int frameCount, float frameDuration) {
    sprite.setTexture(tex);
    totalFrames = frameCount;
    frameTime = frameDuration;
    currentFrame = 0;
    timeAccum = 0.f;
    finished = false;

    int texWidth = static_cast<int>(tex.getSize().x);
    int texHeight = static_cast<int>(tex.getSize().y);
    int frameWidth = frameCount > 0 ? texWidth / frameCount : texWidth;

    sprite.setTextureRect(sf::IntRect(0, 0, frameWidth, texHeight));
    sprite.setOrigin(frameWidth / 2.f, texHeight / 2.f);
    sprite.setPosition(pos);
}

void Explosion::update(float dt) {
    if (finished) return;

    timeAccum += dt;
    if (timeAccum >= frameTime) {
        timeAccum -= frameTime;
        currentFrame++;
        if (currentFrame >= totalFrames) {
            finished = true;
            return;
        }

        int frameWidth = sprite.getTextureRect().width;
        int frameHeight = sprite.getTextureRect().height;
        sprite.setTextureRect(sf::IntRect(
            currentFrame * frameWidth,
            0,
            frameWidth,
            frameHeight
        ));
    }
}

void Explosion::draw(sf::RenderWindow& window) const {
    if (!finished) {
        window.draw(sprite);
    }
}
