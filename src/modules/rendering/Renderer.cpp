#include "modules/rendering/Renderer.h"
#include <iostream>

bool Renderer::init(sf::RenderWindow& window) {
    loadTexture("background", "../assets/textures/ust_bg.jpg");

    // Player jets
    loadTexture("player",      "../assets/textures/arac11.png");
    loadTexture("player_jet1", "../assets/textures/arac11.png");
    loadTexture("player_jet2", "../assets/textures/jet.png");
    loadTexture("player_jet3", "../assets/textures/jet1.png");

    // Player bullets (by power level)
    loadTexture("player_bullet",        "../assets/textures/missile.png"); // fallback / default
    loadTexture("player_missile",       "../assets/textures/missile.png");
    loadTexture("player_missile_heavy", "../assets/textures/missile1.png");
    loadTexture("player_rocket",        "../assets/textures/rocket3.png");

    // Enemies
    loadTexture("enemy",   "../assets/textures/enemy2.png"); // generic fallback
    loadTexture("enemy_A", "../assets/textures/enemy2.png");
    loadTexture("enemy_B", "../assets/textures/enemy21.png");
    loadTexture("enemy_C", "../assets/textures/vv1.png");
    // Boss health battery texture
    loadTexture("boss_health", "../assets/textures/can1.jpg");

    // Enemy & boss bullets
    loadTexture("enemy_bullet", "../assets/textures/rocket4.png");
    loadTexture("boss_bullet1", "../assets/textures/boss1gun.png");
    loadTexture("boss_bullet2", "../assets/textures/boss2gun.png");
    loadTexture("boss_bullet3", "../assets/textures/boss3gun3.png");

    // Boss sprites
    loadTexture("boss1", "../assets/textures/boss1.png");
    loadTexture("boss2", "../assets/textures/boss2.png");
    loadTexture("boss3", "../assets/textures/boss3.png");

    // explosion strip (14 horizontal frames)
    loadTexture("explosion", "../assets/textures/explosion_strip.png");

    // power-up icons
    loadTexture("power_weapon", "../assets/textures/missi.png");
    loadTexture("power_shield", "../assets/textures/kalp.png");
    loadTexture("power_bomb", "../assets/textures/bombready.png");
    loadTexture("power_life", "../assets/textures/score2.png");
    loadTexture("power_slow", "../assets/textures/beamm.png");

    bool fontLoaded = font.loadFromFile("../assets/fonts/arial.ttf");
    if (!fontLoaded) {
        std::cerr << "Warning: font assets/fonts/arial.ttf not found or failed to load. Trying system fallbacks...\n";
#ifdef __APPLE__
        const char* candidates[] = {
            "/System/Library/Fonts/Supplemental/Arial.ttf",
            "/Library/Fonts/Arial.ttf",
            "/System/Library/Fonts/Supplemental/Helvetica.ttc"
        };
#elif defined(_WIN32)
        const char* candidates[] = {
            "C:/Windows/Fonts/arial.ttf",
            "C:/Windows/Fonts/ARIAL.TTF"
        };
#else
        const char* candidates[] = {
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"
        };
#endif
        for (const char* path : candidates) {
            if (font.loadFromFile(path)) {
                std::cerr << "Loaded fallback font from " << path << "\n";
                fontLoaded = true;
                break;
            }
        }
        if (!fontLoaded) {
            std::cerr << "Warning: failed to load any font. Text will not render, but the game will continue.\n";
        }
    }

    windowSize = window.getSize();

    // Parallax background setup (all reusing the same base image)
    loadTexture("bg_far",  "../assets/textures/ust_bg.jpg");
    loadTexture("bg_mid",  "../assets/textures/ust_bg.jpg");
    loadTexture("bg_near", "../assets/textures/ust_bg.jpg");

    auto& farTex = textures["bg_far"];
    auto& midTex = textures["bg_mid"];
    auto& nearTex = textures["bg_near"];

    farTex.setRepeated(true);
    midTex.setRepeated(true);
    nearTex.setRepeated(true);

    bgFar.setTexture(farTex);
    bgMid.setTexture(midTex);
    bgNear.setTexture(nearTex);

    bgFar.setPosition(0.f, 0.f);
    bgMid.setPosition(0.f, 0.f);
    bgNear.setPosition(0.f, 0.f);

    bgFar.setTextureRect(sf::IntRect(0, 0,
                                     static_cast<int>(windowSize.x),
                                     static_cast<int>(windowSize.y)));
    bgMid.setTextureRect(sf::IntRect(0, 0,
                                     static_cast<int>(windowSize.x),
                                     static_cast<int>(windowSize.y)));
    bgNear.setTextureRect(sf::IntRect(0, 0,
                                      static_cast<int>(windowSize.x),
                                      static_cast<int>(windowSize.y)));

    bgFarOffset = 0.f;
    bgMidOffset = 0.f;
    bgNearOffset = 0.f;

    return true;
}

sf::Texture& Renderer::getTexture(const std::string& id) {
    auto it = textures.find(id);
    if (it == textures.end()) {
        static sf::Texture dummy;
        return dummy;
    }
    return it->second;
}

void Renderer::loadTexture(const std::string& id, const std::string& path) {
    sf::Texture tex;
    if (!tex.loadFromFile(path)) {
        std::cerr << "Failed to load texture: " << path << "\n";
    }
    textures[id] = tex;
}

void Renderer::updateBackground(float dt) {
    // Simple vertical parallax scrolling for three layers
    bgFarOffset += 10.f * dt;
    menuTitleTimer += dt;
    bgMidOffset += 30.f * dt;
    bgNearOffset += 60.f * dt;

    int h = static_cast<int>(windowSize.y);

    bgFar.setTextureRect(sf::IntRect(0,
                                     static_cast<int>(bgFarOffset) % h,
                                     static_cast<int>(windowSize.x),
                                     h));
    bgMid.setTextureRect(sf::IntRect(0,
                                     static_cast<int>(bgMidOffset) % h,
                                     static_cast<int>(windowSize.x),
                                     h));
    bgNear.setTextureRect(sf::IntRect(0,
                                      static_cast<int>(bgNearOffset) % h,
                                      static_cast<int>(windowSize.x),
                                      h));
}
void Renderer::drawBackground(sf::RenderWindow& window) {
    window.draw(bgFar);
    window.draw(bgMid);
    window.draw(bgNear);
}

void Renderer::drawMenu(sf::RenderWindow& window, const Menu& menu, const std::string& title) {
    sf::Vector2u size = window.getSize();

    // animated title bobbing a little up and down
    float offsetY = std::sin(menuTitleTimer * 2.f) * 10.f;

    sf::Text titleText;
    titleText.setFont(font);
    titleText.setString(title);
    titleText.setFillColor(sf::Color(135, 206, 250));
    titleText.setOutlineColor(sf::Color::White);
    titleText.setOutlineThickness(2.f);
    titleText.setCharacterSize(64);
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin(titleBounds.width / 2.f, titleBounds.height / 2.f);
    titleText.setPosition(size.x * 0.5f, 80.f + offsetY);
    window.draw(titleText);

    const auto& items = menu.getItems();
    int selected = menu.getSelectedIndex();

    for (std::size_t i = 0; i < items.size(); ++i) {
        sf::Text itemText;
        itemText.setFont(font);
        itemText.setString(items[i]);
        itemText.setCharacterSize(32);
        sf::FloatRect itemBounds = itemText.getLocalBounds();
        itemText.setOrigin(itemBounds.width / 2.f, itemBounds.height / 2.f);
        itemText.setPosition(size.x * 0.5f, 200.f + static_cast<float>(i) * 50.f);

        if (static_cast<int>(i) == selected) {
            itemText.setStyle(sf::Text::Bold);
            itemText.setFillColor(sf::Color::Yellow);

            // draw arrows around currently selected item
            sf::Text leftArrow("<", font, 32);
            sf::Text rightArrow(">", font, 32);
            leftArrow.setFillColor(sf::Color::Yellow);
            rightArrow.setFillColor(sf::Color::Yellow);

            float halfWidth = itemBounds.width * 0.5f;
            leftArrow.setPosition(itemText.getPosition().x - halfWidth - 40.f, itemText.getPosition().y);
            rightArrow.setPosition(itemText.getPosition().x + halfWidth + 10.f, itemText.getPosition().y);

            window.draw(leftArrow);
            window.draw(rightArrow);
        }

        window.draw(itemText);
    }
}

void Renderer::drawGameOver(sf::RenderWindow& window, int score) {
    sf::Vector2u size = window.getSize();
    sf::Text txt;
    txt.setFont(font);
    txt.setCharacterSize(40);
    txt.setString("Game Over\nScore: " + std::to_string(score) + "\nPress Enter");
    txt.setPosition(size.x * 0.3f, size.y * 0.4f);
    window.draw(txt);
}


void Renderer::drawWin(sf::RenderWindow& window, int score, float accuracy, int enemiesDestroyed, int bombsUsed) {
    sf::Vector2u size = window.getSize();

    sf::Text title;
    title.setFont(font);
    title.setCharacterSize(48);
    title.setString("CONGRATULATIONS");
    sf::FloatRect tb = title.getLocalBounds();
    title.setOrigin(tb.width / 2.f, tb.height / 2.f);
    title.setPosition(size.x * 0.5f, size.y * 0.25f);
    window.draw(title);

    sf::Text stats;
    stats.setFont(font);
    stats.setCharacterSize(26);

    sf::Text subtitle;
    subtitle.setFont(font);
    subtitle.setCharacterSize(32);
    subtitle.setString("YOU WON THE GAME\ncongrationals you complete all the stage.");
    sf::FloatRect sb = subtitle.getLocalBounds();
    subtitle.setOrigin(sb.width / 2.f, sb.height / 2.f);
    subtitle.setPosition(size.x * 0.5f, size.y * 0.35f);
    window.draw(subtitle);
    std::string accStr = std::to_string(static_cast<int>(accuracy)) + "%";

    stats.setString(
        "Final Score: " + std::to_string(score) + "\n" +
        "Accuracy: " + accStr + "\n" +
        "Enemies Destroyed: " + std::to_string(enemiesDestroyed) + "\n\n" +
        "Press Enter to return to Main Menu"
    );
    stats.setPosition(size.x * 0.3f, size.y * 0.4f);
    window.draw(stats);
}



void Renderer::drawBossHealth(sf::RenderWindow& window, float healthRatio, const sf::Vector2f& bossPos) {
    auto it = textures.find("boss_health");
    if (it == textures.end()) {
        return;
    }

    // Clamp health ratio into [0,1]
    if (healthRatio < 0.f) healthRatio = 0.f;
    if (healthRatio > 1.f) healthRatio = 1.f;

    sf::Texture& tex = it->second;
    bossHealthSprite.setTexture(tex);

    // Use a portion of the texture width to represent remaining health
    sf::IntRect fullRect = bossHealthSprite.getTextureRect();
    if (fullRect.width == 0 || fullRect.height == 0) {
        fullRect = sf::IntRect(
            0,
            0,
            static_cast<int>(tex.getSize().x),
            static_cast<int>(tex.getSize().y)
        );
    }

    fullRect.width = static_cast<int>(static_cast<float>(fullRect.width) * healthRatio);
    bossHealthSprite.setTextureRect(fullRect);

    // Position the health battery just above the boss so it "follows" the boss.
    // We center horizontally on the boss and offset vertically upwards.
    float barWidth = static_cast<float>(fullRect.width);
    float offsetY = 80.f; // distance above the boss sprite center
    bossHealthSprite.setPosition(bossPos.x - barWidth * 0.5f, bossPos.y - offsetY);

    window.draw(bossHealthSprite);
}

void Renderer::setWindowSize(sf::Vector2u size) {
    windowSize = size;

    auto itFar = textures.find("bg_far");
    auto itMid = textures.find("bg_mid");
    auto itNear = textures.find("bg_near");

    if (itFar != textures.end() && itMid != textures.end() && itNear != textures.end()) {
        auto& farTex = itFar->second;
        auto& midTex = itMid->second;
        auto& nearTex = itNear->second;

        farTex.setRepeated(true);
        midTex.setRepeated(true);
        nearTex.setRepeated(true);

        bgFar.setTexture(farTex);
        bgMid.setTexture(midTex);
        bgNear.setTexture(nearTex);

        bgFar.setTextureRect(sf::IntRect(0, 0,
                                         static_cast<int>(windowSize.x),
                                         static_cast<int>(windowSize.y)));
        bgMid.setTextureRect(sf::IntRect(0, 0,
                                         static_cast<int>(windowSize.x),
                                         static_cast<int>(windowSize.y)));
        bgNear.setTextureRect(sf::IntRect(0, 0,
                                          static_cast<int>(windowSize.x),
                                          static_cast<int>(windowSize.y)));
    }
}