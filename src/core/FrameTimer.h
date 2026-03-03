#pragma once

#include <chrono>

class FrameTimer {
public:
    FrameTimer() : last_(std::chrono::steady_clock::now()) {}

    double TickSeconds() {
        const auto now = std::chrono::steady_clock::now();
        const std::chrono::duration<double> delta = now - last_;
        last_ = now;
        return delta.count();
    }

private:
    std::chrono::steady_clock::time_point last_;
};
