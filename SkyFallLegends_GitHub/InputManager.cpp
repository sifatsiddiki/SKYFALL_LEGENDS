#include "modules/input/InputManager.h"
#include <cmath>

namespace {
    // helper for simple deadzone check
    float applyDeadzone(float value, float deadzone) {
        if (std::fabs(value) < deadzone) return 0.f;
        return value;
    }
}

bool InputManager::init(sf::RenderWindow& win) {
    window = &win;

    keyBindings[PlayerAction::Up]      = sf::Keyboard::Up;
    keyBindings[PlayerAction::Down]    = sf::Keyboard::Down;
    keyBindings[PlayerAction::Left]    = sf::Keyboard::Left;
    keyBindings[PlayerAction::Right]   = sf::Keyboard::Right;
    keyBindings[PlayerAction::Fire]    = sf::Keyboard::Space;
    keyBindings[PlayerAction::Bomb]    = sf::Keyboard::LShift;
    keyBindings[PlayerAction::Pause]   = sf::Keyboard::Escape;
    keyBindings[PlayerAction::Confirm] = sf::Keyboard::Enter;
    keyBindings[PlayerAction::Back]    = sf::Keyboard::BackSpace;

    // initialise pressed / held maps
    pressed.clear();
    held.clear();
    for (const auto& kv : keyBindings) {
        pressed[kv.first] = false;
        held[kv.first] = false;
    }

    axisX = 0.f;
    axisY = 0.f;

    return true;
}

void InputManager::update() {
    // reset "pressed" for this frame
    for (auto& kv : pressed) {
        kv.second = false;
    }

    // keyboard input: update held/pressed
    for (auto& kv : keyBindings) {
        PlayerAction action = kv.first;
        sf::Keyboard::Key key = kv.second;

        bool downNow = sf::Keyboard::isKeyPressed(key);
        bool wasHeld = held[action];

        held[action] = downNow;
        if (downNow && !wasHeld) {
            pressed[action] = true;
        }
    }

    // controller axes (left stick)
    axisX = 0.f;
    axisY = 0.f;

    unsigned int joystickId = 0;
    if (sf::Joystick::isConnected(joystickId)) {
        float rawX = sf::Joystick::getAxisPosition(joystickId, sf::Joystick::X);
        float rawY = sf::Joystick::getAxisPosition(joystickId, sf::Joystick::Y);

        // axis position is -100..100
        float dz = 20.f;
        rawX = applyDeadzone(rawX, dz);
        rawY = applyDeadzone(rawY, dz);

        axisX = rawX / 100.f;
        axisY = rawY / 100.f;

        // controller buttons mapped to actions (Xbox-style layout)
        auto joyDown = [&](unsigned int button) {
            return sf::Joystick::isButtonPressed(joystickId, button);
        };

        auto mergeAction = [&](PlayerAction action, bool downNow) {
            if (!held.count(action)) {
                held[action] = false;
                pressed[action] = false;
            }
            bool wasHeld = held[action];
            if (downNow) {
                held[action] = true;
                if (!wasHeld) {
                    pressed[action] = true;
                }
            }
        };

        // A (0): Fire + Confirm
        bool buttonA = joyDown(0);
        mergeAction(PlayerAction::Fire, buttonA);
        mergeAction(PlayerAction::Confirm, buttonA);

        // B (1): Bomb
        bool buttonB = joyDown(1);
        mergeAction(PlayerAction::Bomb, buttonB);

        // Start (7): Pause
        bool buttonStart = joyDown(7);
        mergeAction(PlayerAction::Pause, buttonStart);

        // Back (6): Back
        bool buttonBack = joyDown(6);
        mergeAction(PlayerAction::Back, buttonBack);
    }
}

bool InputManager::isActionPressed(PlayerAction action) const {
    auto it = pressed.find(action);
    if (it != pressed.end()) return it->second;
    return false;
}

bool InputManager::isActionHeld(PlayerAction action) const {
    auto it = held.find(action);
    if (it != held.end()) return it->second;
    return false;
}

sf::Vector2f InputManager::getMoveAxis() const {
    return sf::Vector2f(axisX, axisY);
}

void InputManager::setKeyBinding(PlayerAction action, sf::Keyboard::Key key) {
    keyBindings[action] = key;
    // also ensure maps are initialised
    if (!pressed.count(action)) pressed[action] = false;
    if (!held.count(action)) held[action] = false;
}
