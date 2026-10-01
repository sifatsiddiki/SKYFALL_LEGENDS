#pragma once
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <map>

enum class PlayerAction {
    Up,
    Down,
    Left,
    Right,
    Fire,
    Bomb,
    Pause,
    Confirm,
    Back
};

class InputManager {
public:
    bool init(sf::RenderWindow& win);

    // Update per-frame input state (keyboard + controller)
    void update();

    // Returns true on the frame the action was pressed
    bool isActionPressed(PlayerAction action) const;

    // Returns true while the action is held down
    bool isActionHeld(PlayerAction action) const;

    // Analog movement axis (primarily from controller)
    sf::Vector2f getMoveAxis() const;

    // Keyboard binding management
    void setKeyBinding(PlayerAction action, sf::Keyboard::Key key);
    const std::map<PlayerAction, sf::Keyboard::Key>& getKeyBindings() const { return keyBindings; }

private:
    sf::RenderWindow* window = nullptr;

    std::map<PlayerAction, sf::Keyboard::Key> keyBindings;
    std::map<PlayerAction, bool> pressed;
    std::map<PlayerAction, bool> held;

    float axisX = 0.f;
    float axisY = 0.f;
};
