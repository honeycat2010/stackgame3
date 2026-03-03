#pragma once

enum class AppScene {
    Title,
    Menu,
    InGame,
    GameOver,
    Exit
};

enum class InGameSubState {
    Running,
    Pause,
    ResumeCountdown
};

enum class PauseMenuItem {
    Resume,
    Title,
    Quit
};

enum class InputAction {
    None,
    Confirm,
    Back,
    MoveUp,
    MoveDown,
    MoveLeft,
    MoveRight,
    Pause
};
