#pragma once

#include <random>
#include <string>
#include <vector>

#include "core/FrameTimer.h"
#include "core/GameTypes.h"
#include "core/InputRouter.h"
#include "core/RenderLayout.h"
#include "core/SceneManager.h"

class GameApp {
public:
    void Run();

private:
    struct ActiveBread {
        int id = 0;
        double x = 0.0;
        double y = 0.0;
        double width = 64.0;
        double height = 32.0;
        double prevBottomY = 0.0;
        bool judged = false;
    };

    struct Rgb {
        int r = 0;
        int g = 0;
        int b = 0;
    };

private:
    void RecalculateViewport();
    void PrintStatus() const;
    void PrintHelp() const;
    void PrintSceneGuide() const;
    void PrintInGameHudFrame() const;

    void HandleSceneInput(InputAction action);
    bool HandleInGameTickCommand(const std::string& line);
    void UpdateInGameSimulation(double dtSec);
    void SpawnBread();
    void ResetInGameSession();

    std::vector<double> BuildSpawnLanes() const;
    bool IsCatchHitByCenterPoint(const ActiveBread& bread, int& outHitmapX, int& outHitmapY, Rgb& outColor) const;
    Rgb SampleCatcherHitmap(int hitmapX, int hitmapY) const;

    static std::string SceneName(AppScene scene);

private:
    static constexpr int kLogicalWidth = 480;
    static constexpr int kLogicalHeight = 720;

    int windowWidth_ = 1280;
    int windowHeight_ = 720;
    bool running_ = true;

    ViewportRect viewport_{};
    FrameTimer timer_;
    SceneManager scene_;
    InputRouter router_;

    InGameSubState inGameSubState_ = InGameSubState::Running;

    int hudStage_ = 1;
    int hudPlacedCount_ = 0;
    int hudTargetPlaced_ = 10;
    int hudTotal_ = 0;
    int hudMiss_ = 0;

    int nextBreadId_ = 1;
    double spawnTimerSec_ = 0.0;
    double spawnIntervalSec_ = 1.0;
    double breadFallSpeed_ = 180.0;
    double camY_ = 0.0;
    double catcherLineScreenY_ = 620.0;
    std::vector<double> spawnLanes_;
    std::vector<ActiveBread> activeBreads_;

    double catcherX_ = 240.0;
    double catcherTopScreenY_ = 590.0;
    int catcherHitmapWidth_ = 120;
    int catcherHitmapHeight_ = 40;
    double catcherMoveSpeed_ = 240.0;

    std::mt19937 rng_{12345};
};
