#pragma once

#include "core/GameTypes.h"

class SceneManager {
public:
    AppScene Current() const { return current_; }
    void Request(AppScene next) { requested_ = next; }
    void ApplyIfRequested();

private:
    AppScene current_ = AppScene::Title;
    AppScene requested_ = AppScene::Title;
};
