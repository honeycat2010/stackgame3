#include "core/InputRouter.h"

namespace {
InputAction ParseRawInput(const std::string& raw) {
    if (raw == "enter" || raw == "space" || raw == "confirm") return InputAction::Confirm;
    if (raw == "esc" || raw == "back") return InputAction::Back;
    if (raw == "up") return InputAction::MoveUp;
    if (raw == "down") return InputAction::MoveDown;
    if (raw == "left") return InputAction::MoveLeft;
    if (raw == "right") return InputAction::MoveRight;
    return InputAction::None;
}

bool IsAllowedInTitle(const InputAction action) {
    return action == InputAction::Confirm || action == InputAction::Back;
}

bool IsAllowedInMenu(const InputAction action) {
    return action == InputAction::Confirm || action == InputAction::Back
        || action == InputAction::MoveUp || action == InputAction::MoveDown;
}

bool IsAllowedInInGame(const InGameSubState subState, const InputAction action) {
    if (subState != InGameSubState::Running) {
        return action == InputAction::Back;
    }

    return action == InputAction::Confirm || action == InputAction::Back
        || action == InputAction::MoveLeft || action == InputAction::MoveRight;
}

bool IsAllowedInGameOver(const InputAction action) {
    return action == InputAction::Confirm || action == InputAction::Back;
}
}

InputAction InputRouter::Route(const AppScene scene, const InGameSubState subState, const std::string& raw) const {
    const InputAction parsed = ParseRawInput(raw);
    if (parsed == InputAction::None) {
        return InputAction::None;
    }

    switch (scene) {
        case AppScene::Title:
            return IsAllowedInTitle(parsed) ? parsed : InputAction::None;
        case AppScene::Menu:
            return IsAllowedInMenu(parsed) ? parsed : InputAction::None;
        case AppScene::InGame:
            return IsAllowedInInGame(subState, parsed) ? parsed : InputAction::None;
        case AppScene::GameOver:
            return IsAllowedInGameOver(parsed) ? parsed : InputAction::None;
        case AppScene::Exit:
            return InputAction::None;
    }

    return InputAction::None;
}
