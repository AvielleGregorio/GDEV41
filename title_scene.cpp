#include <raylib.h>
#include "scene_manager.hpp"

class TitleScene : public Scene {
    Texture title;

public:
    void Begin() override {
        title = ResourceManager::GetInstance()->GetTexture("title.png");
    }

    void End() override {}

    void Update() override {
        if (IsKeyPressed(KEY_ENTER)) {
            if (GetSceneManager() != nullptr) {
                GetSceneManager()->SwitchScene(1);
            }
        }
        
    }

    void Draw() override {
        DrawTexturePro(
            title,
            {0, 0, 320, 180},
            {0, 0, 1280, 720},
            {0, 0},
            0.0f,
            WHITE
        );
        if (ui_library.Button(0, "Start Game", { 960, 540, 200, 60 }))
        {
            if (GetSceneManager() != nullptr) {
                GetSceneManager()->SwitchScene(1);
            }
        }
        // DrawText("Press ENTER", 960, 540, 30, BLACK);
    }
};