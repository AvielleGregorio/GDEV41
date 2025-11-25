#include <raylib.h>

#include "scene_manager.hpp"
#include "title_scene.cpp"
#include "game_scene.cpp"
#include "leaderboard_scene.cpp"

int main() {
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Scene and Resource Management");
    SetTargetFPS(60);

    SceneManager scene_manager;

    TitleScene title_scene;
    title_scene.SetSceneManager(&scene_manager);

    GameScene game_scene;
    game_scene.SetSceneManager(&scene_manager);

    LeaderboardScene leaderboard_scene;
    leaderboard_scene.SetSceneManager(&scene_manager);

    scene_manager.RegisterScene(&title_scene, 0);
    scene_manager.RegisterScene(&game_scene, 1);
    scene_manager.RegisterScene(&leaderboard_scene, 2);

    scene_manager.SwitchScene(0);

    while(!WindowShouldClose()) {
        Scene* active_scene = scene_manager.GetActiveScene();

        BeginDrawing();
        ClearBackground(WHITE);

        if (active_scene != nullptr) {
            active_scene->Update();
            active_scene->Draw();
        }

        EndDrawing();
    }

    Scene* active_scene = scene_manager.GetActiveScene();
    if (active_scene != nullptr) {
        active_scene->End();
    }

    ResourceManager::GetInstance()->UnloadAllTextures();

    CloseWindow();
    return 0;
}