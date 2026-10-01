#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include "ui/Menu.h"

class Renderer {
public:
    bool init(sf::RenderWindow& window);

    sf::Texture& getTexture(const std::string& id);
    const sf::Font& getFont() const { return font; }

    void updateBackground(float dt);
    void drawBackground(sf::RenderWindow& window);
    void drawMenu(sf::RenderWindow& window, const Menu& menu, const std::string& title);
    void drawGameOver(sf::RenderWindow& window, int score);
    void drawWin(sf::RenderWindow& window, int score, float accuracy, int enemiesDestroyed, int bombsUsed);
    void drawBossHealth(sf::RenderWindow& window, float healthRatio, const sf::Vector2f& bossPos);

    // Called when the window is recreated or resized so parallax background scales correctly
    void setWindowSize(sf::Vector2u size);

private:
    void loadTexture(const std::string& id, const std::string& path);

    std::map<std::string, sf::Texture> textures;
    // Parallax background layers
    sf::Sprite bgFar;
    sf::Sprite bgMid;
    sf::Sprite bgNear;
    sf::Font font;
    sf::Sprite bossHealthSprite;

    float bgFarOffset = 0.f;
    float bgMidOffset = 0.f;
    float bgNearOffset = 0.f;
    sf::Vector2u windowSize;
    float menuTitleTimer = 0.f;
};