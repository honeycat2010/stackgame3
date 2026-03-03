#pragma once

#include <string>
#include "core/GameTypes.h"

class InputRouter {
public:
    InputAction Route(const AppScene scene, const InGameSubState subState, const std::string& raw) const;
};
