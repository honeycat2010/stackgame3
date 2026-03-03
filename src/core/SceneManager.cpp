#include "core/SceneManager.h"

void SceneManager::ApplyIfRequested() {
    if (requested_ != current_) {
        current_ = requested_;
    }
}
