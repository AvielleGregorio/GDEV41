#include <raylib.h>
#include "scene_manager.hpp"

class ScoreboardScene : public Scene {
    Texture bg;
public:
    void Begin() override {
        bg = ResourceManager::GetInstance()->GetTexture("background_scoreboard.png");
    }

    void End() override {
        GetSceneManager()->Close();
    }

    void Update() override {

    }

    void Draw() override {
        DrawTexturePro(
            bg,
            {0, 0, 320, 180},
            {0, 0, 1280, 720},
            {0, 0},
            0.0f,
            WHITE
        );
        DrawText(TextFormat("Final Score: %d", score), WINDOW_WIDTH/2 - 200, 180, 50, SKYBLUE);
        if (ui_library.Button(1, "Play Again", { 100, 540, 200, 60 }))
        {
            if (GetSceneManager() != nullptr) {
                GetSceneManager()->SwitchScene(1);
            }
        }
        if (ui_library.Button(2, "End Game", { 960, 540, 200, 60 }))
        {
            End();
        }
    }
};