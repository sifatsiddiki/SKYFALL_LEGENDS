#include "engine/CoreEngine.h"
#include <iostream>
#include <algorithm>
#include <fstream>
#include <cstdlib>

CoreEngine::CoreEngine()
: state(GameState::MainMenu),
  mainMenu({"Start", "Select Jet", "Options", "Quit"}),
  optionsMenu({"Easy", "Normal", "Hard", "Windowed 1920x1080", "Fullscreen 1920x1080", "Vol -", "Vol +", "Music On/Off", "SFX On/Off", "VSync On/Off", "Rebind Fire", "Rebind Bomb", "Back"}),
  jetMenu({"Classic Jet", "Interceptor", "Shadow", "Back"}),
  levelSelectMenu({"Stage 1", "Stage 2", "Stage 3", "Back"}),
  pauseMenu({"Resume", "Restart", "Main Menu", "Quit"}),
  hud()
{
}

CoreEngine::~CoreEngine() {
    saveConfig();
}

bool CoreEngine::init() {
    sf::VideoMode vm(1280, 720);
    window.create(vm, "Skyfall Legends", sf::Style::Default);
    window.setPosition(sf::Vector2i(50, 50));
    window.setFramerateLimit(0);

    if (!renderer.init(window)) {
        std::cerr << "Failed to init renderer\n";
        return false;
    }

    if (!hud.init(renderer.getFont())) {
        std::cerr << "Failed to init HUD\n";
        return false;
    }

    if (!input.init(window)) {
        std::cerr << "Failed to init input\n";
        return false;
    }

    if (!ai.init()) {
        std::cerr << "Failed to init AI\n";
        return false;
    }

    currentDifficulty = Difficulty::Normal;

    loadConfig();
    fullscreen = false;
    if (fullscreen) {
        applyVideoSettings();
    }
    applyVsync();

    // stage music
    if (stageMusic.openFromFile("../assets/music/theme.wav")) {
        stageMusic.setLoop(true);
        stageMusic.play();
    } else {
        std::cerr << "Warning: failed to load stage music assets/music/theme.wav\n";
    }

    // optional boss music (user can add this file later)
    if (!bossMusic.openFromFile("../assets/music/boss_theme.wav")) {
        std::cerr << "Warning: failed to load boss music assets/music/boss_theme.wav\n";
    } else {
        bossMusic.setLoop(true);
    }

    // Load sound effects
    if (!fireBuffer.loadFromFile("../assets/sfx/fuze.wav")) {
        std::cerr << "Warning: failed to load fire sound assets/sfx/fuze.wav\n";
    } else {
        fireSound.setBuffer(fireBuffer);
        fireSound.setVolume(60.f);
    }

    if (!bombBuffer.loadFromFile("../assets/sfx/bomb.wav")) {
        std::cerr << "Warning: failed to load bomb sound assets/sfx/bomb.wav\n";
    } else {
        bombSound.setBuffer(bombBuffer);
        bombSound.setVolume(70.f);
    }

    // Load enemy explosion sound (used when enemies die)
    if (!enemyExplodeBuffer.loadFromFile("../assets/sfx/exp1.wav")) {
        std::cerr << "Warning: failed to load enemy explode sound assets/sfx/exp1.wav\n";
    } else {
        enemyExplodeSound.setBuffer(enemyExplodeBuffer);
        enemyExplodeSound.setVolume(70.f);
    }


    // UI click sound
    if (!uiClickBuffer.loadFromFile("../assets/sfx/baslangic.wav")) {
        std::cerr << "Warning: failed to load UI click sound assets/sfx/baslangic.wav\n";
    } else {
        uiClickSound.setBuffer(uiClickBuffer);
        uiClickSound.setVolume(70.f);
    }

resetGame();
    running = true;
    return true;
}

void CoreEngine::applyVideoSettings() {
    // supported resolutions
    sf::Vector2u resolutions[] = {
        sf::Vector2u(1280, 720),
        sf::Vector2u(1600, 900),
        sf::Vector2u(1920, 1080)
    };
    const int maxIndex = 2;
    if (resolutionIndex < 0) resolutionIndex = 0;
    if (resolutionIndex > maxIndex) resolutionIndex = maxIndex;

    sf::Vector2u res = resolutions[resolutionIndex];
    sf::VideoMode vm(res.x, res.y);
    auto style = fullscreen ? sf::Style::Fullscreen : sf::Style::Default;

    window.create(vm, "Skyfall Legends", style);
    applyVsync();

    // rebind input to the new window
    input.init(window);

    // inform renderer of new window size
    renderer.setWindowSize(window.getSize());
}

bool CoreEngine::loadConfig() {
    std::ifstream in("config.txt");
    if (!in) {
        return false;
    }

    int diffInt = 1;
    int fullInt = 0;
    int storedHigh = 0;
    float storedMasterVol = 1.0f;
    int musicInt = 1;
    int sfxInt = 1;
    int vsyncInt = 1;
    int unlocked = 1;
    int storedResIndex = 2;

    in >> diffInt;
    in >> storedHigh;
    in >> fullInt;
    in >> storedMasterVol;
    in >> musicInt;
    in >> sfxInt;
    in >> vsyncInt;
    in >> unlocked;
    in >> storedResIndex;

    if (!in) {
        return false;
    }

    highScore = storedHigh;
    fullscreen = (fullInt != 0);
    masterVolume = storedMasterVol;
    musicEnabled = (musicInt != 0);
    sfxEnabled = (sfxInt != 0);
    vsyncEnabled = (vsyncInt != 0);
    maxUnlockedLevel = unlocked;
    resolutionIndex = storedResIndex;

    if (diffInt == 0) currentDifficulty = Difficulty::Easy;
    else if (diffInt == 2) currentDifficulty = Difficulty::Hard;
    else currentDifficulty = Difficulty::Normal;

    ai.setDifficulty(currentDifficulty);

    for (auto& kv : input.getKeyBindings()) {
        int actionIndex = static_cast<int>(kv.first);
        int keyCode = 0;
        if (!(in >> keyCode)) {
            break;
        }
        input.setKeyBinding(static_cast<PlayerAction>(actionIndex),
                            static_cast<sf::Keyboard::Key>(keyCode));
    }

    return true;
}

void CoreEngine::saveConfig() const {
    std::ofstream out("config.txt");
    if (!out) {
        return;
    }

    int diffInt = 1;
    if (currentDifficulty == Difficulty::Easy) diffInt = 0;
    else if (currentDifficulty == Difficulty::Hard) diffInt = 2;

    out << diffInt << "\n";
    out << highScore << "\n";
    out << (fullscreen ? 1 : 0) << "\n";
    out << masterVolume << "\n";
    out << (musicEnabled ? 1 : 0) << "\n";
    out << (sfxEnabled ? 1 : 0) << "\n";
    out << (vsyncEnabled ? 1 : 0) << "\n";
    out << maxUnlockedLevel << "\n";
    out << resolutionIndex << "\n";

    for (const auto& kv : input.getKeyBindings()) {
        int actionIndex = static_cast<int>(kv.first);
        (void)actionIndex;
        out << static_cast<int>(kv.second) << "\n";
    }
}


void CoreEngine::applyVsync() {
    window.setVerticalSyncEnabled(vsyncEnabled);
    if (vsyncEnabled) {
        window.setFramerateLimit(0);
    } else {
        window.setFramerateLimit(60);
    }
}

void CoreEngine::applyAudioSettings() {
    float musicVol = masterVolume * 100.f * (musicEnabled ? 1.f : 0.f);
    float sfxVol = masterVolume * 100.f * (sfxEnabled ? 1.f : 0.f);

    stageMusic.setVolume(musicVol);
    bossMusic.setVolume(musicVol);

    fireSound.setVolume(sfxVol);
    bombSound.setVolume(sfxVol);
    enemyExplodeSound.setVolume(sfxVol);
    powerupPickSound.setVolume(sfxVol);
    uiClickSound.setVolume(sfxVol);
    lowLifeBeepSound.setVolume(sfxVol);
}

void CoreEngine::startLevel(int levelIndex) {
    currentLevel = levelIndex;
    levelTimer = 0.f;

    enemies.clear();
    playerBullets.clear();
    enemyBullets.clear();
    powerUps.clear();
    explosions.clear();
    boss.reset();

    // reset runtime combat state for a fresh level
    bombEffectTimer = 0.f;
    enemySpawnTimer = 0.f;
    bossExplosionInProgress = false;

    // Set lives per-level as requested:
    // Level 1 & 2 -> 2 lives, Level 3 -> 3 lives.
    if (currentLevel == 3) {
        lives = 3;
    } else {
        lives = 2;
    }
    totalShotsFired = 0;
    shotsHit = 0;
    enemiesDestroyed = 0;
    bombsUsed = 0;
    combo = 0;
    maxCombo = 0;
    scoreMultiplier = 1.0f;

    // Choose player texture based on selected jet, but
    // always use the Level 2 jet sprite for Level 1 as requested.
    std::string jetId = "player_jet1";
    if (selectedJetIndex == 1) {
        jetId = "player_jet2";
    } else if (selectedJetIndex == 2) {
        jetId = "player_jet3";
    }

    if (currentLevel == 1) {
        // Use the jet sprite that is normally used for Level 2
        jetId = "player_jet2";
    }

    player = Player();
    player.init(renderer.getTexture(jetId), sf::Vector2f(960.f, 950.f));

    // Spawn a fixed set of enemies/boss for this level
    spawnFixedEnemiesForCurrentLevel();
}

void CoreEngine::spawnFixedEnemiesForCurrentLevel() {
    // Clear any leftover enemies from previous runs
    enemies.clear();

    if (currentLevel == 1) {
        // Level 1: 3 basic enemies
        float y = -80.f;
        float xs[3] = { 640.f, 960.f, 1280.f };
        for (int i = 0; i < 3; ++i) {
            auto enemy = std::make_unique<Enemy>();
            enemy->init(renderer.getTexture("enemy_A"),
                        sf::Vector2f(xs[i], y),
                        1);
            enemies.push_back(std::move(enemy));
        }
    } else if (currentLevel == 2) {
        // Level 2: 5 enemies with a slightly wider spread
        float y = -80.f;
        float xs[5] = { 520.f, 740.f, 960.f, 1180.f, 1400.f };
        for (int i = 0; i < 5; ++i) {
            auto enemy = std::make_unique<Enemy>();
            enemy->init(renderer.getTexture("enemy_B"),
                        sf::Vector2f(xs[i], y),
                        2);
            enemies.push_back(std::move(enemy));
        }
    } else {
        // Level 3: Boss + 3 supporting enemies
        spawnBoss();

        float y = 120.f;
        float xs[3] = { 720.f, 960.f, 1200.f };
        for (int i = 0; i < 3; ++i) {
            auto enemy = std::make_unique<Enemy>();
            enemy->init(renderer.getTexture("enemy_C"),
                        sf::Vector2f(xs[i], y),
                        2);
            enemies.push_back(std::move(enemy));
        }
    }
}

int CoreEngine::getLevelTargetScore() const {
    // Score thresholds per level (updated per latest spec)
    // Level 1: 1200, Level 2: 2200, Level 3: 3200
    if (currentLevel == 1) {
        return 1200;
    } else if (currentLevel == 2) {
        return 2200;
    } else {
        return 3200;
    }
}


void CoreEngine::resetGame() {
    enemies.clear();
    playerBullets.clear();
    enemyBullets.clear();
    powerUps.clear();
    boss.reset();
    explosions.clear();

    score = 0;
    enemySpawnTimer = 0.f;
    bossExplosionInProgress = false;
    lives = 3;
    bombs = 3;
    currentLevel = 1;
    levelTimer = 0.f;
    bombEffectTimer = 0.f;
    totalShotsFired = 0;
    shotsHit = 0;
    enemiesDestroyed = 0;
    bombsUsed = 0;
    combo = 0;
    maxCombo = 0;
    scoreMultiplier = 1.0f;
    totalShotsFired = 0;
    shotsHit = 0;
    enemiesDestroyed = 0;
    bombsUsed = 0;
    combo = 0;
    maxCombo = 0;
    scoreMultiplier = 1.0f;
    boss = nullptr;

    // ensure stage music is active for new run
    if (bossMusic.getStatus() == sf::SoundSource::Playing) {
        bossMusic.stop();
    }
    if (stageMusic.getStatus() != sf::SoundSource::Playing) {
        stageMusic.play();
    }

    player = Player();

    // choose player texture based on selected jet
    std::string jetId = "player_jet1";
    if (selectedJetIndex == 1) {
        jetId = "player_jet2";
    } else if (selectedJetIndex == 2) {
        jetId = "player_jet3";
    }

    player.init(renderer.getTexture(jetId), sf::Vector2f(960.f, 950.f));
}

void CoreEngine::run() {
    sf::Clock clock;
    while (running && window.isOpen()) {
        float dt = clock.restart().asSeconds();
        if (dt > 0.f) {
            float fps = 1.f / dt;
            fpsSmoothed = fpsSmoothed * 0.9f + fps * 0.1f;
        }
        processEvents();
        update(dt);
        render();
    }
}


void CoreEngine::spawnPowerUp(const sf::Vector2f& pos, PowerUpType type) {
    std::string texId = "power_weapon";
    switch (type) {
        case PowerUpType::Weapon: texId = "power_weapon"; break;
        case PowerUpType::Shield: texId = "power_shield"; break;
        case PowerUpType::Bomb:   texId = "power_bomb"; break;
        case PowerUpType::Life:   texId = "power_life"; break;
        case PowerUpType::SlowMo: texId = "power_slow"; break;
    }

    auto& tex = renderer.getTexture(texId);
    auto p = std::make_unique<PowerUp>();
    p->init(tex, pos, type);
    powerUps.push_back(std::move(p));
}

void CoreEngine::spawnRandomPowerUp(const sf::Vector2f& pos) {
    int roll = std::rand() % 100;
    PowerUpType type = PowerUpType::Weapon;
    if (roll < 40)      type = PowerUpType::Weapon;
    else if (roll < 65) type = PowerUpType::Bomb;
    else if (roll < 85) type = PowerUpType::Life;
    else                type = PowerUpType::Shield;

    spawnPowerUp(pos, type);
}

void CoreEngine::applyPowerUp(PowerUp& powerUp) {
    switch (powerUp.getType()) {
        case PowerUpType::Weapon:
            player.addPowerLevel(1);
            break;
        case PowerUpType::Shield:
            // Treat shield as a short extra slow-motion + protection feel
            bombEffectTimer += 0.8f;
            break;
        case PowerUpType::Bomb:
            bombs += 1;
            break;
        case PowerUpType::Life:
            lives += 1;
            break;
        case PowerUpType::SlowMo:
            bombEffectTimer += 1.2f;
            break;
    }
}
std::string CoreEngine::getDifficultyLabel() const {
    switch (currentDifficulty) {
    case Difficulty::Easy: return "Easy";
    case Difficulty::Hard: return "Hard";
    default: return "Normal";
    }
}

void CoreEngine::spawnBoss() {
    boss = std::make_unique<Boss>();

    // choose boss texture based on current level
    std::string bossId = "boss1";
    if (currentLevel == 2) {
        bossId = "boss2";
    } else if (currentLevel >= 3) {
        bossId = "boss3";
    }

    boss->init(renderer.getTexture(bossId), sf::Vector2f(960.f, 200.f));

    // switch to boss music if available
    if (bossMusic.getStatus() != sf::SoundSource::Playing) {
        if (stageMusic.getStatus() == sf::SoundSource::Playing) {
            stageMusic.stop();
        }
        if (bossMusic.getStatus() != sf::SoundSource::Playing) {
            bossMusic.play();
        }
    }
}

void CoreEngine::processEvents() {
    // Handle OS/window events and keep the SFML window responsive
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            running = false;
            window.close();
            return;
        } else if (event.type == sf::Event::Resized) {
            sf::FloatRect visibleArea(0.f, 0.f,
                                      static_cast<float>(event.size.width),
                                      static_cast<float>(event.size.height));
            window.setView(sf::View(visibleArea));
        }
    }

    // Now update high-level input (keyboard/controller)
    input.update();

    // debug keys
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::F1)) {
        debugOverlay = !debugOverlay;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::F2)) {
        if (state == GameState::Playing) {
            state = GameState::StageClear;
        }
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::F3)) {
        if (state == GameState::Playing && !boss) {
            spawnBoss();
        }
    }

    if (state == GameState::MainMenu) {
        if (input.isActionPressed(PlayerAction::Up)) {
            mainMenu.moveSelection(-1);
        } else if (input.isActionPressed(PlayerAction::Down)) {
            mainMenu.moveSelection(1);
        } else if (input.isActionPressed(PlayerAction::Confirm)) {
            uiClickSound.play();
            int sel = mainMenu.getSelectedIndex();
            if (sel == 0) {
                // Start Game -> go to Level Select menu (3 level options visible)
                resetGame();
                state = GameState::LevelSelect;
            } else if (sel == 1) {
                state = GameState::JetSelect;
            } else if (sel == 2) {
                state = GameState::Options;
            } else if (sel == 3) {
                running = false;
                window.close();
            }
        }
    } else if (state == GameState::Options) {
        if (input.isActionPressed(PlayerAction::Up)) {
            optionsMenu.moveSelection(-1);
        } else if (input.isActionPressed(PlayerAction::Down)) {
            optionsMenu.moveSelection(1);
        } else if (input.isActionPressed(PlayerAction::Confirm)) {
            uiClickSound.play();
            int sel = optionsMenu.getSelectedIndex();
            switch (sel) {
            case 0:
                currentDifficulty = Difficulty::Easy;
                ai.setDifficulty(currentDifficulty);
                break;
            case 1:
                currentDifficulty = Difficulty::Normal;
                ai.setDifficulty(currentDifficulty);
                break;
            case 2:
                currentDifficulty = Difficulty::Hard;
                ai.setDifficulty(currentDifficulty);
                break;
            case 3:
                // cycle resolution 1280x720 -> 1600x900 -> 1920x1080
                resolutionIndex = (resolutionIndex + 1) % 3;
                applyVideoSettings();
                break;
            case 4:
                // toggle windowed / fullscreen
                fullscreen = !fullscreen;
                applyVideoSettings();
                break;
            case 5:
                masterVolume -= 0.1f;
                if (masterVolume < 0.f) masterVolume = 0.f;
                applyAudioSettings();
                break;
            case 6:
                masterVolume += 0.1f;
                if (masterVolume > 1.f) masterVolume = 1.f;
                applyAudioSettings();
                break;
            case 7:
                musicEnabled = !musicEnabled;
                applyAudioSettings();
                break;
            case 8:
                sfxEnabled = !sfxEnabled;
                applyAudioSettings();
                break;
            case 9:
                vsyncEnabled = !vsyncEnabled;
                applyVsync();
                break;
            case 10: {
                static sf::Keyboard::Key fireOptions[] = {
                    sf::Keyboard::Space,
                    sf::Keyboard::LControl,
                    sf::Keyboard::RControl
                };
                static std::size_t currentIndex = 0;
                currentIndex = (currentIndex + 1) % (sizeof(fireOptions) / sizeof(fireOptions[0]));
                input.setKeyBinding(PlayerAction::Fire, fireOptions[currentIndex]);
                break;
            }
            case 11: {
                static sf::Keyboard::Key bombOptions[] = {
                    sf::Keyboard::LShift,
                    sf::Keyboard::RShift,
                    sf::Keyboard::B
                };
                static std::size_t currentIndex = 0;
                currentIndex = (currentIndex + 1) % (sizeof(bombOptions) / sizeof(bombOptions[0]));
                input.setKeyBinding(PlayerAction::Bomb, bombOptions[currentIndex]);
                break;
            }
            case 12:
            default:
                state = GameState::MainMenu;
                break;
            }
        } else if (input.isActionPressed(PlayerAction::Back)) {
            state = GameState::MainMenu;
        }

    } else if (state == GameState::JetSelect) {
        if (input.isActionPressed(PlayerAction::Up)) {
            jetMenu.moveSelection(-1);
        } else if (input.isActionPressed(PlayerAction::Down)) {
            jetMenu.moveSelection(1);
        } else if (input.isActionPressed(PlayerAction::Confirm)) {
            int sel = jetMenu.getSelectedIndex();
            if (sel == 0 || sel == 1 || sel == 2) {
                selectedJetIndex = sel;
                state = GameState::MainMenu;
            } else if (sel == 3) {
                state = GameState::MainMenu;
            }
        } else if (input.isActionPressed(PlayerAction::Back)) {
            state = GameState::MainMenu;
        }
    } else if (state == GameState::LevelSelect) {
        if (input.isActionPressed(PlayerAction::Up)) {
            levelSelectMenu.moveSelection(-1);
        } else if (input.isActionPressed(PlayerAction::Down)) {
            levelSelectMenu.moveSelection(1);
        } else if (input.isActionPressed(PlayerAction::Confirm)) {
            uiClickSound.play();
            int sel = levelSelectMenu.getSelectedIndex();
            if (sel == 0 || sel == 1 || sel == 2) {
                // Levels are 1-based for the player
                int levelIndex = sel + 1;
                startLevel(levelIndex);
                state = GameState::Playing;
            } else if (sel == 3) { // Back
                state = GameState::MainMenu;
            }
        } else if (input.isActionPressed(PlayerAction::Back)) {
            state = GameState::MainMenu;
        }
    } else if (state == GameState::Playing) {
        // Allow both Pause and Back actions to open the pause menu during gameplay
        if (input.isActionPressed(PlayerAction::Pause) || input.isActionPressed(PlayerAction::Back)) {
            state = GameState::Paused;
        }
    } else if (state == GameState::Paused) {
        if (input.isActionPressed(PlayerAction::Up)) {
            pauseMenu.moveSelection(-1);
        } else if (input.isActionPressed(PlayerAction::Down)) {
            pauseMenu.moveSelection(1);
        } else if (input.isActionPressed(PlayerAction::Confirm)) {
            int sel = pauseMenu.getSelectedIndex();
            if (sel == 0) { // Resume
                state = GameState::Playing;
            } else if (sel == 1) { // Restart
                resetGame();
                state = GameState::Playing;
            } else if (sel == 2) { // Main Menu
                state = GameState::MainMenu;
            } else if (sel == 3) { // Quit
                running = false;
                window.close();
            }
        } else if (input.isActionPressed(PlayerAction::Pause) || input.isActionPressed(PlayerAction::Back)) {
            state = GameState::Playing;
        }
        } else if (state == GameState::StageClear) {
        // On stage clear, allow both Confirm (Enter/Fire) and Back (Backspace/B) so the player is never stuck.
        if (input.isActionPressed(PlayerAction::Confirm)) {
            uiClickSound.play();
            if (currentLevel >= MAX_LEVELS) {
                // Last level completed -> go to final congratulations screen
                state = GameState::Win;
            } else {
                // Allow the player to pick the next level from the level-select screen
                state = GameState::LevelSelect;
            }
        } else if (input.isActionPressed(PlayerAction::Back)) {
            uiClickSound.play();
            // Go straight back to the main menu from the results screen
            state = GameState::MainMenu;
        }
    } else if (state == GameState::GameOver) {
        // From Game Over screen, Confirm or Back both return to Main Menu.
        if (input.isActionPressed(PlayerAction::Confirm) || input.isActionPressed(PlayerAction::Back)) {
            state = GameState::MainMenu;
        }
    } else if (state == GameState::Win) {
        // From final Win screen, Confirm or Back both return to Main Menu.
        if (input.isActionPressed(PlayerAction::Confirm) || input.isActionPressed(PlayerAction::Back)) {
            state = GameState::MainMenu;
        }
    }
}

void CoreEngine::update(float dt) {
    renderer.updateBackground(dt);

    // Slow-motion window after using a bomb
    if (bombEffectTimer > 0.f) {
        bombEffectTimer -= dt;
        if (bombEffectTimer < 0.f) {
            bombEffectTimer = 0.f;
        }
    }

    float gameplayDt = dt;
    if (bombEffectTimer > 0.f) {
        // During bomb effect, slow down overall gameplay a bit
        gameplayDt *= 0.35f;
    }

    if (state != GameState::Playing) {
        return;
    }

    // track time spent in current level for stage-style progression
    levelTimer += gameplayDt;

    // wave-based enemy spawning
    // Wave-based spawning disabled for simplified level design;
    // enemies are spawned explicitly per level.

    sf::Vector2f moveDir;
    if (input.isActionHeld(PlayerAction::Up)) moveDir.y -= 1.f;
    if (input.isActionHeld(PlayerAction::Down)) moveDir.y += 1.f;
    if (input.isActionHeld(PlayerAction::Left)) moveDir.x -= 1.f;
    if (input.isActionHeld(PlayerAction::Right)) moveDir.x += 1.f;

    moveDir += input.getMoveAxis();

    player.update(gameplayDt, moveDir, window);

    if (input.isActionPressed(PlayerAction::Fire)) {
        // Keep bullet texture choice based on power level so weapon upgrades still matter
        int pwr = player.getPowerLevel();
        std::string bulletTexId = "player_missile";       // base
        if (pwr >= 1) bulletTexId = "player_missile_heavy";
        if (pwr >= 2) bulletTexId = "player_rocket";

        sf::Texture& btex = renderer.getTexture(bulletTexId);

        // All firing patterns are level-based:
        // Level 1: single center stream
        // Level 2: two wing streams (left/right)
        // Level 3: three streams (left, center, right)
        sf::Vector2f basePos = player.getSprite().getPosition();

        auto spawnBullet = [&](const sf::Vector2f& offset) {
            auto b = std::make_unique<Bullet>();
            b->init(btex, basePos + offset, sf::Vector2f(0.f, -600.f));
            playerBullets.push_back(std::move(b));
        };

        // We only count a "shot" once per press, regardless of number of streams
        totalShotsFired += 1;

        if (currentLevel == 1) {
            // single center stream
            spawnBullet(sf::Vector2f(0.f, 0.f));
        } else if (currentLevel == 2) {
            // two wing streams (no center)
            spawnBullet(sf::Vector2f(-30.f, 0.f));
            spawnBullet(sf::Vector2f(30.f, 0.f));
        } else {
            // level 3 and above: three streams (left, center, right)
            spawnBullet(sf::Vector2f(-24.f, 0.f));
            spawnBullet(sf::Vector2f(0.f, 0.f));
            spawnBullet(sf::Vector2f(24.f, 0.f));
        }

        // play fire sound if loaded
        fireSound.play();
    }


    if (input.isActionPressed(PlayerAction::Bomb) && bombs > 0) {
        bombs -= 1;
        bombsUsed += 1;
        for (auto &e : enemies) {
            e->applyDamage(5);
        }
        for (auto &b : enemyBullets) {
            b->kill();
        }
        if (boss) {
            boss->applyDamage(5);
        }
        // trigger short slow-motion & screen flash
        bombEffectTimer = 0.6f;
        // play bomb sound if loaded
        bombSound.play();
    }


    for (auto &enemy : enemies) {
        // Update enemy behavior (movement patterns & fire timers)
        enemy->updateAI(gameplayDt);
        // Apply velocity-based movement
        enemy->update(gameplayDt);
        // Allow enemies to fire bullets towards the player
        if (auto shot = enemy->fire(renderer.getTexture("enemy_bullet"))) {
            enemyBullets.push_back(std::move(shot));
        }
    }

    if (boss && boss->isAlive()) {
        boss->update(gameplayDt);

        // boss stage-wise unique bullet textures
        std::string bossBulletId = "enemy_bullet"; // fallback
        if (currentLevel == 1)      bossBulletId = "boss_bullet1";
        else if (currentLevel == 2) bossBulletId = "boss_bullet2";
        else if (currentLevel >= 3) bossBulletId = "boss_bullet3";

        if (auto shot = boss->fire(renderer.getTexture(bossBulletId))) {
            enemyBullets.push_back(std::move(shot));
        }
    }

    for (auto &b : playerBullets) {
        b->update(gameplayDt);
    }
    for (auto &b : enemyBullets) {
        b->update(gameplayDt);
    }

    // Continuous enemy spawning while the level is active.
    // Enemies keep spawning until the level's target score is reached
    // or the player dies / the game leaves the Playing state.
    if (state == GameState::Playing && lives > 0) {
        int targetScore = getLevelTargetScore();
        if (score < targetScore && currentLevel >= 1 && currentLevel <= MAX_LEVELS) {
            enemySpawnTimer += gameplayDt;

            // Base spawn interval tuned per level & difficulty
            float interval = 1.6f;
            if (currentDifficulty == Difficulty::Easy)      interval = 1.8f;
            else if (currentDifficulty == Difficulty::Hard) interval = 1.2f;

            if (currentLevel == 2)      interval *= 0.9f;
            else if (currentLevel >= 3) interval *= 0.8f;

            bool bossAlive = boss && boss->isAlive();
            std::size_t enemyCap = bossAlive ? 8u : 12u;

            if (enemySpawnTimer >= interval && enemies.size() < enemyCap) {
                enemySpawnTimer = 0.f;

                sf::Vector2u winSize = window.getSize();
                float minX = 80.f;
                float maxX = static_cast<float>(winSize.x) - 80.f;
                if (maxX <= minX) {
                    maxX = minX + 1.f;
                }
                float spawnX = minX + static_cast<float>(std::rand()) /
                                          static_cast<float>(RAND_MAX) *
                                          (maxX - minX);
                float spawnY = -60.f;

                // Choose enemy visuals & behavior per-level
                int patternLevel = 1;
                std::string enemyTexId = "enemy_A";
                if (currentLevel == 1) {
                    patternLevel = 1;
                    enemyTexId = "enemy_A";
                } else if (currentLevel == 2) {
                    patternLevel = 2;
                    enemyTexId = "enemy_B";
                } else {
                    patternLevel = 2;
                    enemyTexId = "enemy_C";
                }

                auto enemy = std::make_unique<Enemy>();
                enemy->init(renderer.getTexture(enemyTexId),
                            sf::Vector2f(spawnX, spawnY),
                            patternLevel);
                enemies.push_back(std::move(enemy));
            }
        } else {
            // stop the spawn timer once we've reached the target score
            enemySpawnTimer = 0.f;
        }
    }

    // update floating power-ups
    for (auto &p : powerUps) {
        p->update(gameplayDt);
    }

    physics.handleCollisions(player, enemies, playerBullets, enemyBullets,
                             score, lives, enemiesDestroyed, shotsHit,
                             combo, maxCombo, scoreMultiplier);

    if (boss && boss->isAlive()) {
        auto bossBounds = boss->getSprite().getGlobalBounds();
        auto playerBounds = player.getSprite().getGlobalBounds();

        if (bossBounds.intersects(playerBounds)) {
            lives -= 1;
            combo = 0;
            scoreMultiplier = 1.0f;
        }

        for (auto &b : playerBullets) {
            if (!b->isAlive()) continue;
            if (b->getSprite().getGlobalBounds().intersects(bossBounds)) {
                b->kill();
                // Apply a bit more damage on Stage 3 so the boss health visibly decreases as expected.
                int damage = 1;
                if (currentLevel == 3) {
                    damage = 2;
                }
                boss->applyDamage(damage);
                shotsHit += 1;
            }
        }

        if (!boss->isAlive()) {
            // Begin scripted boss destruction sequence the first time we detect death.
            if (!bossExplosionInProgress) {
                score += boss->getScoreValue();

                // guaranteed helpful drops when boss dies
                sf::Vector2f bossPos = boss->getSprite().getPosition();
                spawnPowerUp(bossPos, PowerUpType::Bomb);
                spawnPowerUp(bossPos + sf::Vector2f(40.f, 0.f), PowerUpType::Weapon);

                // start a large multi-frame explosion animation at the boss position
                bossExplosionInProgress = true;
                bossExplosion.init(renderer.getTexture("explosion"), bossPos, 14, 0.06f);

                // clear remaining regular enemies and their bullets so the stage is visually focused on the boss
                enemies.clear();
                enemyBullets.clear();
                powerUps.clear();

                // transition music back to the stage theme
                if (bossMusic.getStatus() == sf::SoundSource::Playing) {
                    bossMusic.stop();
                }
                if (stageMusic.getStatus() != sf::SoundSource::Playing) {
                    stageMusic.play();
                }
            }
        }

    // player collects power-ups
    auto playerBoundsForPower = player.getSprite().getGlobalBounds();
    for (auto &p : powerUps) {
        if (!p->isAlive()) continue;
        if (p->getSprite().getGlobalBounds().intersects(playerBoundsForPower)) {
            applyPowerUp(*p);
            p->kill();
        }
    }

    // remove consumed power-ups
    powerUps.erase(std::remove_if(powerUps.begin(), powerUps.end(),
        [](const std::unique_ptr<PowerUp>& p){ return !p->isAlive(); }), powerUps.end());

    }

    if (score > highScore) {
        // update and persist new high score when record is broken
        highScore = score;
        saveConfig();
    }

    if (state == GameState::Playing) {
        bool bossAlive = boss && boss->isAlive();
        bool allEnemiesCleared = enemies.empty() && !bossAlive;
        int targetScore = getLevelTargetScore();

        // Level is considered complete only when the target score
        // is reached AND all active threats are cleared.
        if (allEnemiesCleared && score >= targetScore) {
            state = GameState::StageClear;
            if (currentLevel == maxUnlockedLevel && currentLevel < MAX_LEVELS) {
                maxUnlockedLevel++;
            }
            saveConfig();
        }
    }

    if (lives <= 0) {
        state = GameState::GameOver;
        combo = 0;
        scoreMultiplier = 1.0f;
    }

    // low-life warning beep
    if (lives == 1) {
        lowLifeBeepTimer -= gameplayDt;
        if (lowLifeBeepTimer <= 0.f) {
            lowLifeBeepSound.play();
            lowLifeBeepTimer = 2.0f;
        }
    } else {
        lowLifeBeepTimer = 0.f;
    }

    // advance boss destruction explosion (if any); transition to StageClear when finished
    if (bossExplosionInProgress) {
        bossExplosion.update(gameplayDt);
        if (bossExplosion.isFinished()) {
            bossExplosionInProgress = false;
            // once the cinematic explosion is done, remove the boss entirely
            boss.reset();
            // use the regular level clear flow (shows win screen, then proceed/back)
            state = GameState::StageClear;
        }
    }

    // update & prune regular enemy explosions
    for (auto &e : explosions) {
        e.update(gameplayDt);
    }
    explosions.erase(
        std::remove_if(explosions.begin(), explosions.end(),
                       [](const Explosion& e){ return e.isFinished(); }),
        explosions.end()
    );

    // remove dead enemies and occasionally spawn power-ups at their position
    for (auto it = enemies.begin(); it != enemies.end(); ) {
        if (!(*it)->isAlive()) {
            sf::Vector2f pos = (*it)->getSprite().getPosition();
            // spawn explosion & play SFX
            Explosion e;
            e.init(renderer.getTexture("explosion"), pos, 14, 0.05f);
            explosions.push_back(e);
            enemyExplodeSound.play();

            if (std::rand() % 100 < 25) { // ~25% chance for a drop
                spawnRandomPowerUp(pos);
            }
            it = enemies.erase(it);
        } else {
            ++it;
        }
    }

    playerBullets.erase(std::remove_if(playerBullets.begin(), playerBullets.end(),
        [](const std::unique_ptr<Bullet>& b){ return !b->isAlive(); }), playerBullets.end());

    enemyBullets.erase(std::remove_if(enemyBullets.begin(), enemyBullets.end(),
        [](const std::unique_ptr<Bullet>& b){ return !b->isAlive(); }), enemyBullets.end());

    hud.update(score, lives, bombs, currentLevel,
               getDifficultyLabel(),
               combo, scoreMultiplier,
               shotsHit, totalShotsFired);
}

void CoreEngine::render() {
    window.clear();

    if (state == GameState::MainMenu) {
        renderer.drawBackground(window);
        renderer.drawMenu(window, mainMenu, "SkYFall Legends");
    } else if (state == GameState::Options) {
        renderer.drawBackground(window);
        renderer.drawMenu(window, optionsMenu, "OPTIONS");
        
        // show current resolution, mode, and key bindings
        sf::Text info;
        info.setFont(renderer.getFont());
        info.setCharacterSize(20);
        info.setFillColor(sf::Color::White);

        const char* resStrings[] = { "1280x720", "1600x900", "1920x1080" };
        int resIdx = resolutionIndex;
        if (resIdx < 0) resIdx = 0;
        if (resIdx > 2) resIdx = 2;

        std::string modeStr = fullscreen ? "Fullscreen" : "Windowed";

        auto &bindings = input.getKeyBindings();
        auto keyName = [](sf::Keyboard::Key key) -> std::string {
            switch (key) {
                case sf::Keyboard::Space:    return "Space";
                case sf::Keyboard::LControl: return "Left Ctrl";
                case sf::Keyboard::RControl: return "Right Ctrl";
                case sf::Keyboard::LShift:   return "Left Shift";
                case sf::Keyboard::RShift:   return "Right Shift";
                case sf::Keyboard::Enter:    return "Enter";
                case sf::Keyboard::BackSpace:return "Backspace";
                case sf::Keyboard::Up:       return "Up Arrow";
                case sf::Keyboard::Down:     return "Down Arrow";
                case sf::Keyboard::Left:     return "Left Arrow";
                case sf::Keyboard::Right:    return "Right Arrow";
                case sf::Keyboard::B:        return "B";
                default:
                    return std::to_string(static_cast<int>(key));
            }
        };

        std::string fireKey = keyName(bindings.at(PlayerAction::Fire));
        std::string bombKey = keyName(bindings.at(PlayerAction::Bomb));

        std::string text =
            "Resolution: " + std::string(resStrings[resIdx]) + "\n" +
            "Mode: " + modeStr + "\n\n" +
            "Fire Key: " + fireKey + "\n" +
            "Bomb Key: " + bombKey + "\n\n" +
            "Controller: Left Stick move, A = Fire/Select, B = Bomb, Start = Pause";

        info.setString(text);
        info.setPosition(40.f, 600.f);
        window.draw(info);
    } else if (state == GameState::LevelSelect) {
        renderer.drawBackground(window);
        renderer.drawMenu(window, levelSelectMenu, "SELECT STAGE");
    } else if (state == GameState::JetSelect) {
        renderer.drawBackground(window);
        renderer.drawMenu(window, jetMenu, "SELECT JET");

        // Show the actual jet image beside the selection menu, but only for real jets.
        int sel = jetMenu.getSelectedIndex();
        if (sel >= 0 && sel <= 2) {
            std::string jetId = "player_jet1";
            if (sel == 1) jetId = "player_jet2";
            else if (sel == 2) jetId = "player_jet3";

            sf::Sprite preview;
            preview.setTexture(renderer.getTexture(jetId));
            auto bounds = preview.getLocalBounds();
            preview.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
            sf::Vector2u size = window.getSize();
            preview.setPosition(size.x * 0.75f, size.y * 0.5f);
            preview.setScale(1.2f, 1.2f);
            window.draw(preview);
        }
    } else if (state == GameState::Playing) {
        renderer.drawBackground(window);

        // subtle white flash overlay during bomb slow-motion
        if (bombEffectTimer > 0.f) {
            sf::RectangleShape flash(sf::Vector2f(static_cast<float>(window.getSize().x),
                                                  static_cast<float>(window.getSize().y)));
            float alphaFactor = bombEffectTimer / 0.6f;
            if (alphaFactor < 0.f) alphaFactor = 0.f;
            if (alphaFactor > 1.f) alphaFactor = 1.f;
            flash.setFillColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(alphaFactor * 180.f)));
            window.draw(flash, sf::BlendAdd);
        }

        window.draw(player.getSprite());
        for (auto &enemy : enemies) {
            window.draw(enemy->getSprite());
        }
        if (boss && boss->isAlive()) {
            window.draw(boss->getSprite());
            // Boss health UI battery using can1 asset
            float ratio = 1.f;
            if (boss->getMaxHealth() > 0) {
                ratio = static_cast<float>(boss->getHealth()) / static_cast<float>(boss->getMaxHealth());
            }
            // Attach health battery directly above the boss so it moves with the boss
            renderer.drawBossHealth(window, ratio, boss->getSprite().getPosition());
        }
        for (auto &b : playerBullets) {
            window.draw(b->getSprite());
        }
        for (auto &b : enemyBullets) {
            window.draw(b->getSprite());
        }

        // draw active explosions
        for (const auto &e : explosions) {
            e.draw(window);
        }

        // draw boss destruction explosion (if active)
        if (bossExplosionInProgress) {
            bossExplosion.draw(window);
        }

        hud.draw(window);

        if (debugOverlay) {
            sf::Text debugText;
            debugText.setFont(renderer.getFont());
            debugText.setCharacterSize(16);
            debugText.setFillColor(sf::Color::Green);
            debugText.setPosition(10.f, 80.f);
            char buf[256];
            std::snprintf(buf, sizeof(buf), "FPS: %.1f\nEnemies: %zu\nLevel Time: %.1f",
                          fpsSmoothed,
                          enemies.size() + playerBullets.size() + enemyBullets.size(),
                          levelTimer);
            debugText.setString(buf);
            window.draw(debugText);
        }
    } else if (state == GameState::Paused) {
        renderer.drawBackground(window);
        renderer.drawMenu(window, pauseMenu, "PAUSED");

        // Show quick info & control reminder while paused
        sf::Text infoText;
        infoText.setFont(renderer.getFont());
        infoText.setCharacterSize(22);
        infoText.setFillColor(sf::Color::White);
        infoText.setString("Level: " + std::to_string(currentLevel) + "   Score: " + std::to_string(score));
        infoText.setPosition(40.f, 140.f);
        window.draw(infoText);

        sf::Text controlsText;
        controlsText.setFont(renderer.getFont());
        controlsText.setCharacterSize(18);
        controlsText.setFillColor(sf::Color(200, 200, 200));
        controlsText.setString(
            "Controls:\n"
            "Move: Arrow Keys / Left Stick\n"
            "Fire: Space / A button\n"
            "Bomb: Left Shift / B button\n"
            "Pause: Esc / Start button"
        );
        controlsText.setPosition(40.f, 180.f);
        window.draw(controlsText);
    } else if (state == GameState::StageClear) {
        renderer.drawBackground(window);

        sf::Text title;
        title.setFont(renderer.getFont());
        title.setCharacterSize(48);
        title.setFillColor(sf::Color::Yellow);
        title.setString("STAGE CLEARED");
        auto tb = title.getLocalBounds();
        title.setOrigin(tb.width / 2.f, tb.height / 2.f);
        title.setPosition(window.getSize().x * 0.5f, window.getSize().y * 0.25f);
        window.draw(title);

        sf::Text info;
        info.setFont(renderer.getFont());
        info.setCharacterSize(26);
        info.setFillColor(sf::Color::White);
        char buf[256];
        float accuracy = 0.f;
        if (totalShotsFired > 0) {
            accuracy = (static_cast<float>(shotsHit) / static_cast<float>(totalShotsFired)) * 100.f;
        }
        std::snprintf(buf, sizeof(buf),
                      "Score: %d\nEnemies Destroyed: %d\nAccuracy: %.1f%%\nMax Combo: %d\n\nPress Enter to continue",
                      score, enemiesDestroyed, accuracy, maxCombo);
        info.setString(buf);
        info.setPosition(window.getSize().x * 0.3f, window.getSize().y * 0.4f);
        window.draw(info);
    } else if (state == GameState::GameOver) {
        renderer.drawBackground(window);
        renderer.drawGameOver(window, score);
    } else if (state == GameState::Win) {
        renderer.drawBackground(window);

        float accuracy = 0.f;
        if (totalShotsFired > 0) {
            accuracy = 100.f * static_cast<float>(shotsHit) / static_cast<float>(totalShotsFired);
        }

        renderer.drawWin(window, score, accuracy, enemiesDestroyed, bombsUsed);
    }

    window.display();
}