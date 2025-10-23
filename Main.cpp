#include <raylib.h>
#include <raymath.h>

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;
const float FPS = 60;
const int MAX_CIRCLES = 1000;

typedef struct {
    Vector2 position;
    float radius;
    float mass;
    float inverse_mass;
    Vector2 velocity;
    Color color;
} Circles;


int main() {
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Uniform Grid");
    SetTargetFPS(FPS);

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        // Rendering
        BeginDrawing();
        ClearBackground(WHITE);
        

        EndDrawing();
    }
    CloseWindow();
    return 0;
}

