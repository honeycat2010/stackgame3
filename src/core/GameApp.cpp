#include "core/GameApp.h"

#include <algorithm>
#include <iostream>
#include <sstream>

void GameApp::Run() {
    RecalculateViewport();
    spawnLanes_ = BuildSpawnLanes();

    PrintHelp();
    PrintStatus();

    while (running_) {
        const double dt = timer_.TickSeconds();
        (void)dt;

        PrintSceneGuide();
        std::cout << "Input> ";

        std::string line;
        std::getline(std::cin, line);

        if (line == "exit") {
            running_ = false;
            continue;
        }

        if (line == "status") {
            PrintStatus();
            continue;
        }

        if (line == "help") {
            PrintHelp();
            continue;
        }

        if (line.rfind("resize", 0) == 0) {
            std::istringstream iss(line);
            std::string cmd;
            int w = 0;
            int h = 0;
            iss >> cmd >> w >> h;
            if (w > 0 && h > 0) {
                windowWidth_ = w;
                windowHeight_ = h;
                RecalculateViewport();
                PrintStatus();
            } else {
                std::cout << "Usage: resize <width> <height>\n";
            }
            continue;
        }

        if (scene_.Current() == AppScene::InGame && HandleInGameTickCommand(line)) {
            scene_.ApplyIfRequested();
            continue;
        }

        const InputAction action = router_.Route(scene_.Current(), inGameSubState_, line);
        if (action == InputAction::None) {
            std::cout << "Blocked or unknown input for current scene.\n";
            continue;
        }

        HandleSceneInput(action);
        scene_.ApplyIfRequested();
    }
}

void GameApp::RecalculateViewport() {
    viewport_ = CalculateLetterboxViewport(windowWidth_, windowHeight_, kLogicalWidth, kLogicalHeight);
}

void GameApp::PrintStatus() const {
    std::cout
        << "Logical: " << kLogicalWidth << "x" << kLogicalHeight << "\n"
        << "Window : " << windowWidth_ << "x" << windowHeight_ << "\n"
        << "Viewport: x=" << viewport_.x
        << ", y=" << viewport_.y
        << ", w=" << viewport_.width
        << ", h=" << viewport_.height << "\n";
}

void GameApp::PrintHelp() const {
    std::cout
        << "Global commands:\n"
        << "  status               : print logical/window/viewport\n"
        << "  resize <w> <h>       : update window size and recalc letterbox\n"
        << "  help                 : print commands\n"
        << "  exit                 : quit app\n"
        << "Input commands:\n"
        << "  enter/space/confirm, esc/back, up/down/left/right\n"
        << "InGame simulation commands:\n"
        << "  tick <sec>           : advance spawn/fall simulation\n";
}

void GameApp::PrintSceneGuide() const {
    std::cout << "\n[Scene] " << SceneName(scene_.Current()) << "\n";

    switch (scene_.Current()) {
        case AppScene::Title:
            std::cout << "  allow: confirm/back\n";
            break;
        case AppScene::Menu:
            std::cout << "  allow: up/down/confirm/back\n";
            break;
        case AppScene::InGame:
            std::cout << "  allow: left/right/confirm/back\n";
            std::cout << "  command: tick <sec>\n";
            PrintInGameHudFrame();
            break;
        case AppScene::GameOver:
            std::cout << "  allow: confirm/back\n";
            break;
        case AppScene::Exit:
            std::cout << "  exit scene\n";
            break;
    }
}

void GameApp::PrintInGameHudFrame() const {
    std::cout
        << "  [HUD] Stage=" << hudStage_
        << " | Placed=" << hudPlacedCount_ << "/" << hudTargetPlaced_
        << " | total=" << hudTotal_
        << " | miss=" << hudMiss_ << "/3"
        << " | activeBreads=" << activeBreads_.size()
        << " | lineY=" << worldLineY_ << "\n";
}

void GameApp::HandleSceneInput(const InputAction action) {
    const AppScene current = scene_.Current();

    if (current == AppScene::Title) {
        if (action == InputAction::Confirm) {
            scene_.Request(AppScene::Menu);
        } else if (action == InputAction::Back) {
            scene_.Request(AppScene::Exit);
            running_ = false;
        }
        return;
    }

    if (current == AppScene::Menu) {
        if (action == InputAction::Confirm) {
            scene_.Request(AppScene::InGame);
            inGameSubState_ = InGameSubState::Running;
            ResetInGameSession();
        } else if (action == InputAction::Back) {
            scene_.Request(AppScene::Title);
        }
        return;
    }

    if (current == AppScene::InGame) {
        if (action == InputAction::Confirm) {
            scene_.Request(AppScene::GameOver);
        } else if (action == InputAction::Back) {
            scene_.Request(AppScene::Menu);
        }
        return;
    }

    if (current == AppScene::GameOver) {
        if (action == InputAction::Confirm) {
            scene_.Request(AppScene::InGame);
            inGameSubState_ = InGameSubState::Running;
            ResetInGameSession();
        } else if (action == InputAction::Back) {
            scene_.Request(AppScene::Title);
        }
        return;
    }
}

bool GameApp::HandleInGameTickCommand(const std::string& line) {
    if (line.rfind("tick", 0) != 0) {
        return false;
    }

    std::istringstream iss(line);
    std::string cmd;
    double sec = 0.0;
    iss >> cmd >> sec;

    if (sec <= 0.0) {
        std::cout << "Usage: tick <sec> (sec > 0)\n";
        return true;
    }

    UpdateInGameSimulation(sec);
    return true;
}

void GameApp::UpdateInGameSimulation(const double dtSec) {
    spawnTimerSec_ += dtSec;
    while (spawnTimerSec_ >= spawnIntervalSec_) {
        spawnTimerSec_ -= spawnIntervalSec_;
        SpawnBread();
    }

    for (auto& bread : activeBreads_) {
        bread.prevBottomY = bread.y + bread.height;
        bread.y += breadFallSpeed_ * dtSec;
        const double currBottomY = bread.y + bread.height;

        if (!bread.judged
            && bread.prevBottomY < worldLineY_
            && currBottomY >= worldLineY_) {
            bread.judged = true;
            hudMiss_ += 1;
            std::cout << "[MISS] breadId=" << bread.id
                      << " prevBottom=" << bread.prevBottomY
                      << " currBottom=" << currBottomY
                      << " lineY=" << worldLineY_ << "\n";
        }
    }

    activeBreads_.erase(
        std::remove_if(activeBreads_.begin(), activeBreads_.end(),
            [](const ActiveBread& bread) { return bread.judged; }),
        activeBreads_.end());

    if (hudMiss_ >= 3) {
        std::cout << "[GameOver] miss reached 3\n";
        scene_.Request(AppScene::GameOver);
    }
}

void GameApp::SpawnBread() {
    if (spawnLanes_.empty()) {
        return;
    }

    std::uniform_int_distribution<int> pick(0, static_cast<int>(spawnLanes_.size()) - 1);

    ActiveBread bread;
    bread.id = nextBreadId_++;
    bread.x = spawnLanes_[pick(rng_)];
    bread.y = -bread.height;
    bread.prevBottomY = bread.y + bread.height;

    activeBreads_.push_back(bread);

    std::cout << "[SPAWN] breadId=" << bread.id
              << " laneX=" << bread.x
              << " y=" << bread.y << "\n";
}

void GameApp::ResetInGameSession() {
    hudPlacedCount_ = 0;
    hudTargetPlaced_ = 10;
    hudMiss_ = 0;
    hudTotal_ = 0;

    nextBreadId_ = 1;
    spawnTimerSec_ = 0.0;
    activeBreads_.clear();
}

std::vector<double> GameApp::BuildSpawnLanes() const {
    std::vector<double> lanes;
    lanes.reserve(5);

    for (int i = 0; i < 5; ++i) {
        const double x = (static_cast<double>(i) + 1.0) * (static_cast<double>(kLogicalWidth) / 6.0);
        lanes.push_back(x);
    }

    return lanes;
}

std::string GameApp::SceneName(const AppScene scene) {
    switch (scene) {
        case AppScene::Title: return "Title";
        case AppScene::Menu: return "Menu";
        case AppScene::InGame: return "InGame";
        case AppScene::GameOver: return "GameOver";
        case AppScene::Exit: return "Exit";
    }
    return "Unknown";
}
