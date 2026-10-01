#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class HUD {
public:
    bool init(const sf::Font& sharedFont);

    void update(int score, int lives, int bombs, int level,
                const std::string& difficultyLabel,
                int combo, float scoreMultiplier,
                int shotsHit, int totalShotsFired);
    void draw(sf::RenderWindow& window);

private:
    sf::Font font;
    sf::Text scoreText;
    sf::Text livesText;
    sf::Text bombsText;
    sf::Text levelText;
    sf::Text difficultyText;
    sf::Text comboText;
    sf::Text accuracyText;
};
