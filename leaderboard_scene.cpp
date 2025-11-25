#include <raylib.h>
#include "scene_manager.hpp"

class LeaderboardScene : public Scene {

public:
    void Begin() override {

    }

    void End() override {}

    void Update() override {

    }

    void Draw() override {
        DrawRectangle(50, 50, 100, 100, RED);
    }
};