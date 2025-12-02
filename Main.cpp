#include <raylib.h>

#include "scene_manager.hpp"
#include "title_scene.cpp"
#include "game_scene.cpp"
#include "scoreboard_scene.cpp"

int main() {
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "My Library is Haunted, Flerken Help Me!");
    InitAudioDevice();
    SetTargetFPS(60);

    SceneManager scene_manager;

    TitleScene title_scene;
    title_scene.SetSceneManager(&scene_manager);

    GameScene game_scene;
    game_scene.SetSceneManager(&scene_manager);

    ScoreboardScene scoreboard_scene;
    scoreboard_scene.SetSceneManager(&scene_manager);

    scene_manager.RegisterScene(&title_scene, 0);
    scene_manager.RegisterScene(&game_scene, 1);
    scene_manager.RegisterScene(&scoreboard_scene, 2);

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
        
        if (active_scene == nullptr) {
            break;
        }
    }

    Scene* active_scene = scene_manager.GetActiveScene();
    if (active_scene != nullptr) {
        active_scene->End();
    }

    ResourceManager::GetInstance()->UnloadAllTextures();
    ResourceManager::GetInstance()->UnloadAllSounds();

    CloseAudioDevice();
    CloseWindow();
    return 0;
}