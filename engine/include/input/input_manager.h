#pragma once
#include <unordered_map>

namespace d2::input {

class Gamepad;

enum class GameButton {
    ACTION,
    CANCEL,
    SKILL1,
    SKILL2
};

class InputManager {
public:
    explicit InputManager(Gamepad* gamepad);
    
    void update();
    
    bool isButtonPressed(GameButton button) const;

private:
    Gamepad* gamepad_;
    std::unordered_map<GameButton, bool> buttonStates_;
};

} // namespace d2::input