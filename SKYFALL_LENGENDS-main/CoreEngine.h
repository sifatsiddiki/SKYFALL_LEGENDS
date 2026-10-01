#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <memory>
#include <vector>
#include <string>

#include "modules/input/InputManager.h"
#include "modules/rendering/Renderer.h"
#include "modules/physics/PhysicsSystem.h"
#include "modules/ai/AISystem.h"
#include "modules/ai/WaveManager.h"
#include "entities/Player.h"
#include "entities/Enemy.h"
#include "entities/Bullet.h"
#include "entities/Boss.h"
#include "entities/PowerUp.h"
#include "entities/Explosion.h"
#include "ui/Menu.h"
#include "ui/HUD.h"

enum class GameState {
    MainMenu,
    Options,
    JetSelect,
    LevelSelect,
    Playing,
    StageClear,
    Paused,
    GameOver,
    Win
};

class CoreEngine {
public:
    CoreEngine();
    ~CoreEngine();

    bool init();
    void run();

private:
    void processEvents();
    void update(float dt);
    void render();
    void resetGame();

    void applyVideoSettings();
    void applyVsync();
    void applyAudioSettings();
    bool loadConfig();
    void saveConfig() const;

    void spawnBoss();
    void spawnPowerUp(const sf::Vector2f& pos, PowerUpType type);
    void spawnRandomPowerUp(const sf::Vector2f& pos);
    void applyPowerUp(PowerUp& powerUp);
    std::string getDifficultyLabel() const;
    void startLevel(int levelIndex);
    void setupWavesForCurrentLevel();
    void spawnFixedEnemiesForCurrentLevel();
    int getLevelTargetScore() const;

    static constexpr int MAX_LEVELS = 3;

    sf::RenderWindow window;
    InputManager input;
    Renderer renderer;
    PhysicsSystem physics;
    AISystem ai;
    WaveManager waveManager;

    sf::Music stageMusic;
    sf::Music bossMusic;
    sf::SoundBuffer fireBuffer;
    sf::SoundBuffer bombBuffer;
    sf::Sound fireSound;
    sf::Sound bombSound;

    sf::SoundBuffer enemyExplodeBuffer;
    sf::SoundBuffer powerupPickBuffer;
    sf::SoundBuffer uiClickBuffer;
    sf::SoundBuffer lowLifeBeepBuffer;
    sf::Sound enemyExplodeSound;
    sf::Sound powerupPickSound;
    sf::Sound uiClickSound;
    sf::Sound lowLifeBeepSound;

    GameState state;
    Menu mainMenu;
    Menu optionsMenu;
    Menu jetMenu;
    Menu levelSelectMenu;
    Menu pauseMenu;
    HUD hud;

    Player player;
    std::vector<std::unique_ptr<Enemy>> enemies;
    std::vector<std::unique_ptr<Bullet>> playerBullets;
    std::vector<std::unique_ptr<Bullet>> enemyBullets;
    std::vector<std::unique_ptr<PowerUp>> powerUps;
    std::vector<Explosion> explosions;
    std::unique_ptr<Boss> boss;

    // Timer used for continuous enemy spawning while a level is active
    float enemySpawnTimer = 0.f;

    // Dedicated explosion used for the boss destruction cutscene
    bool bossExplosionInProgress = false;
    Explosion bossExplosion;


    int score = 0;
    int highScore = 0;
    int lives = 3;
    int bombs = 3;
    int currentLevel = 1;
    int maxUnlockedLevel = 1;
    float levelTimer = 0.f;
    float bombEffectTimer = 0.f;

    int totalShotsFired = 0;
    int shotsHit = 0;
    int enemiesDestroyed = 0;
    int bombsUsed = 0;
    int combo = 0;
    int maxCombo = 0;
    float scoreMultiplier = 1.0f;
    bool running = false;
    bool fullscreen = false;
    bool vsyncEnabled = true;
    bool musicEnabled = true;
    bool sfxEnabled = true;
    float masterVolume = 1.0f;
    float lowLifeBeepTimer = 0.f;
    int resolutionIndex = 2; // 0:1280x720, 1:1600x900, 2:1920x1080

    Difficulty currentDifficulty = Difficulty::Normal;
    int selectedJetIndex = 0;

    bool debugOverlay = false;
    float fpsSmoothed = 60.f;
};