#include <raylib.h>
#include <raymath.h>

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;
const float FPS = 60;
const float TIMESTEP = 1/FPS;

// Boolean to check for mouse dragging
bool isDragging = false;
Vector2 dragStart = {0,0};
Vector2 dragEnd = {0,0};
float maxDrag = 200.0f;
float launchMultiplier = 15.0f;

struct Player {
    // Set Librarian's initial position in the middle (for now)
    Vector2 position = {WINDOW_WIDTH/2, WINDOW_HEIGHT/2};
    int size = 30;
    int speed = 200;
    Color color = LIME;
    float mass = 2.0f;
    float inverse_mass = 1/mass;
    Vector2 acceleration = Vector2Zero();
    Vector2 velocity = Vector2Zero();
    bool isActive = true; 
    // To be replaced when Sprite
    // Texture playerTexture;
    // Rectangle textureSource;
};

struct Flerken {
    // Set the Flerken Bullet
    Vector2 position;
    Vector2 velocity;
    bool isActive = false;
};

struct Ghost {
    Vector2 position;
    Vector2 velocity;
};

struct Shadow {
    Vector2 position;
    Vector2 velocity;
};

struct Spirit{
    Vector2 position;
    Vector2 velocity;
};

struct Book {

};



int main() {
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Midnight Librarian");
    SetTargetFPS(FPS);

    Player librarian;
    Flerken flerken;
    
    float accumulator = 0;


    while (!WindowShouldClose()) {
        // Code Starts here
        float deltaTime = GetFrameTime();

        // Mouse Input
        if (Vector2Length(librarian.velocity) < 0.7f) {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                Vector2 mousePos = GetMousePosition();
                if (CheckCollisionPointCircle(mousePos, librarian.position, librarian.size)) {
                    isDragging = true;
                    dragStart = librarian.position;
                }
            }

            if (isDragging && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                dragEnd = GetMousePosition();
            }

            // Limits max drag
            if (isDragging) {
                dragEnd = GetMousePosition();

                Vector2 dragVector = Vector2Subtract(dragEnd, dragStart);
                float dragLen = Vector2Length(dragVector);

                if (dragLen > maxDrag) {
                    dragVector = Vector2Scale(Vector2Normalize(dragVector), maxDrag);
                    dragEnd = Vector2Add(dragStart, dragVector);
                    dragLen = maxDrag;
                }

                if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                    Vector2 launchDir = Vector2Normalize(Vector2Subtract(dragStart, dragEnd));
                    Vector2 appliedForce = Vector2Scale(launchDir, dragLen * launchMultiplier);
                    librarian.velocity = Vector2Add(librarian.velocity, Vector2Scale(appliedForce, librarian.inverse_mass));
                    isDragging = false;
                }
            }
        }

        accumulator += deltaTime;
        while (accumulator >= TIMESTEP) {
            float resistance = 0.6f;
            librarian.velocity = Vector2Add(librarian.velocity, Vector2Scale(librarian.acceleration, TIMESTEP));
            Vector2 friction = Vector2Scale(librarian.velocity, -(resistance / librarian.mass) * TIMESTEP);
            librarian.velocity = Vector2Add(librarian.velocity, friction);
            librarian.position = Vector2Add(librarian.position, Vector2Scale(librarian.velocity, TIMESTEP));
            if (Vector2Length(librarian.velocity) < 0.2f) {
                librarian.velocity = Vector2Zero();
            }

            accumulator -= TIMESTEP;

            // Collision Logic
            // X-Plane Boundary
            if (librarian.position.x - librarian.size <= 0 || librarian.position.x + librarian.size >= WINDOW_WIDTH) {
                librarian.velocity.x *= -1;
            }

            // Y-Plane Boundary
            if (librarian.position.y - librarian.size < 0 || librarian.position.y + librarian.size >= WINDOW_HEIGHT) {
                librarian.velocity.y *= -1;
            }

        }






        // RENDERING
        BeginDrawing();
        ClearBackground(BLACK);

        DrawCircleV(librarian.position, librarian.size, librarian.color);

        if (isDragging) {
            DrawLineEx(dragStart, dragEnd, 2.0f, WHITE);
        }

        EndDrawing();
    
    }
    CloseWindow();
    return 0;
}


// Jai's Compile Code (ignore): clang++ Main.cpp libraylib.a -std=c++17 \-framework Cocoa -framework IOKit -framework CoreVideo -framework OpenGL -framework Foundation -o finalproject
