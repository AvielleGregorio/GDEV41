#include <raylib.h>
#include <raymath.h>

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;
const float FPS = 60;
const float TIMESTEP = 1/FPS;

int main() {
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Midnight Librarian");
    SetTargetFPS(FPS);


    while (!WindowShouldClose()) {
        // Code Starts here


        // RENDERING
        BeginDrawing();
        ClearBackground(BLACK);

        DrawCircle(520,350, 25, LIME);

        EndDrawing();
    
    }
    CloseWindow();
    return 0;
}


// Jai's Compile Code (ignore): clang++ Main.cpp libraylib.a -std=c++17 \-framework Cocoa -framework IOKit -framework CoreVideo -framework OpenGL -framework Foundation -o finalproject
