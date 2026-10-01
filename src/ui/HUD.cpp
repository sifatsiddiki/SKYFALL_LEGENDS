#include "ui/HUD.h"

bool HUD::init(const sf::Font& sharedFont) {
    font = sharedFont;
    scoreText.setFont(font);
    livesText.setFont(font);
    bombsText.setFont(font);
    levelText.setFont(font);
    difficultyText.setFont(font);
    comboText.setFont(font);
    accuracyText.setFont(font);

    scoreText.setCharacterSize(20);
    livesText.setCharacterSize(20);
    bombsText.setCharacterSize(20);
    levelText.setCharacterSize(20);
    difficultyText.setCharacterSize(20);
    comboText.setCharacterSize(18);
    accuracyText.setCharacterSize(18);

    scoreText.setFillColor(sf::Color(255, 255, 100));
    livesText.setFillColor(sf::Color::Cyan);
    bombsText.setFillColor(sf::Color(255, 150, 0));
    levelText.setFillColor(sf::Color(180, 180, 255));
    difficultyText.setFillColor(sf::Color::White);
    comboText.setFillColor(sf::Color::Yellow);
    accuracyText.setFillColor(sf::Color(200, 255, 200));

    scoreText.setPosition(20.f, 10.f);
    livesText.setPosition(20.f, 40.f);
    bombsText.setPosition(20.f, 70.f);
    levelText.setPosition(20.f, 100.f);
    difficultyText.setPosition(20.f, 130.f);
    comboText.setPosition(20.f, 160.f);
    accuracyText.setPosition(20.f, 185.f);

    return true;
}

void HUD::update(int score, int lives, int bombs, int level,
                 const std::string& difficultyLabel,
                 int combo, float scoreMultiplier,
                 int shotsHit, int totalShotsFired) {
    scoreText.setString("Score: " + std::to_string(score));
    livesText.setString("Lives: " + std::to_string(lives));
    if (lives == 1) {
        livesText.setFillColor(sf::Color::Red);
    } else {
        livesText.setFillColor(sf::Color::Cyan);
    }
    bombsText.setString("Bombs: " + std::to_string(bombs));
    levelText.setString("Level: " + std::to_string(level));
    difficultyText.setString("Difficulty: " + difficultyLabel);

    // combo & multiplier
    std::string comboStr = "Combo: " + std::to_string(combo);
    if (scoreMultiplier > 1.0f) {
        char buf[64];
        std::snprintf(buf, sizeof(buf), " (x%.1f)", scoreMultiplier);
        comboStr += buf;
    }
    comboText.setString(comboStr);

    // accuracy
    float acc = 0.f;
    if (totalShotsFired > 0) {
        acc = (static_cast<float>(shotsHit) / static_cast<float>(totalShotsFired)) * 100.f;
    }
    char abuf[64];
    std::snprintf(abuf, sizeof(abuf), "Accuracy: %.1f%%", acc);
    accuracyText.setString(abuf);
}

void HUD::draw(sf::RenderWindow& window) {
    window.draw(scoreText);
    window.draw(livesText);
    window.draw(levelText);
    window.draw(difficultyText);
    window.draw(comboText);
    window.draw(accuracyText);
}
