#pragma once
#include <SFML/Graphics.hpp>

class Explosion {
public:
    Explosion() = default;

    void init(sf::Texture& tex, const sf::Vector2f& pos,
              int frameCount, float frameDuration);

    void update(float dt);
    void draw(sf::RenderWindow& window) const;
    bool isFinished() const { return finished; }

private:
    sf::Sprite sprite;
    int currentFrame = 0;
    int totalFrames = 0;
    float frameTime = 0.f;
    float timeAccum = 0.f;
    bool finished = false;
};
