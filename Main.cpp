#include <raylib.h>
#include <raymath.h>
#include <vector>

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;
const float FPS = 60;
const float TIMESTEP = 1/FPS;

const int MAX_BOOKS = 10;
const float BOOK_RESPAWN_DELAY = 3.0f;
const float BOOK_PICKUP_RADIUS = 35.0f;

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
    float mass = 5.0f;
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
    Vector2 velocity = Vector2Zero();
    int size = 20;
    float speed = 800;
    float mass = 2.0f;
    bool isActive = false;
    // To be replaced when Sprite
    // Texture playerTexture;
    // Rectangle textureSource;
};

struct Ghost {
    Vector2 position;
    Vector2 velocity;
    // To be replaced when Sprite
    // Texture playerTexture;
    // Rectangle textureSource;
};

struct Shadow {
    Vector2 position;
    Vector2 velocity;
    // To be replaced when Sprite
    // Texture playerTexture;
    // Rectangle textureSource;
};

struct Spirit{
    Vector2 position;
    Vector2 velocity;
    // To be replaced when Sprite
    // Texture playerTexture;
    // Rectangle textureSource;
};

struct Book {
    Vector2 center;                 // Center position of the book
    Vector2 size;         // Width and height (for now)
    bool isActive;                  
    float respawnCooldown;          // Timer before the book can spawn back again
    Color color = SKYBLUE;
    // To be replaced when Sprite
    // Texture playerTexture;
    // Rectangle textureSource;
};

void DrawCenteredRectangle(Vector2 center, Vector2 size, Color color) {
    Vector2 topLeft = {
        center.x - size.x/2,
        center.y - size.y/2
    };
    DrawRectangleV(topLeft, size, color);
}

Vector2 SpawnRandomBook() {
    float margin = 50.0f;

    Vector2 position;
    position.x = GetRandomValue(margin, WINDOW_WIDTH - margin);
    position.y = GetRandomValue(margin, WINDOW_HEIGHT - margin);

    return position;
}

bool CheckLibrarianBookCollision(Vector2 playerPos, float size, Vector2 bookCenter, Vector2 bookSize) {
    // Compute half-size
    Vector2 half = {bookSize.x/2, bookSize.y/2};

    // Find closest point on book to player
    float closestX = Clamp(playerPos.x, bookCenter.x - half.x, bookCenter.x + half.x);
    float closestY = Clamp(playerPos.y, bookCenter.y - half.y, bookCenter.y + half.y);

    // Distance from player to closest point
    float distX = playerPos.x - closestX;
    float distY = playerPos.y - closestY;

    return (distX * distX + distY * distY) <= (size * size);
}


int main() {
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Midnight Librarian");
    SetTargetFPS(FPS);

    Player librarian;
    Flerken flerken{};

    std::vector<Book> books;
    for (int i = 0; i < MAX_BOOKS; i ++) {
        Book b;
        b.center = SpawnRandomBook();
        b.size = {25,30};
        b.isActive = true;
        b.respawnCooldown = 0;
        books.push_back(b);
    }
    
    float accumulator = 0;


    while (!WindowShouldClose()) {
        // Code Starts here
        float deltaTime = GetFrameTime();

        if (!flerken.isActive) {
            // Flerken Control
            if (IsKeyPressed(KEY_SPACE)) {
                flerken.position = librarian.position;
                Vector2 mousePos = GetMousePosition();

                Vector2 flerkenDir = Vector2Normalize(Vector2Subtract(mousePos, librarian.position));

                // LAUNCH FLERKEN
                flerken.velocity = Vector2Scale(flerkenDir, flerken.speed);
                flerken.isActive = true;
            }
        }

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
            // Librarian Physics
            librarian.velocity = Vector2Add(librarian.velocity, Vector2Scale(librarian.acceleration, TIMESTEP));
            Vector2 friction = Vector2Scale(librarian.velocity, -(resistance / librarian.mass) * TIMESTEP);
            librarian.velocity = Vector2Add(librarian.velocity, friction);
            librarian.position = Vector2Add(librarian.position, Vector2Scale(librarian.velocity, TIMESTEP));
            if (Vector2Length(librarian.velocity) < 0.9f) {
                librarian.velocity = Vector2Zero();
            }

            // Flerken Physics
            if (flerken.isActive) {
                DrawCircleV(flerken.position, flerken.size, ORANGE);
                flerken.position = Vector2Add(flerken.position, Vector2Scale(flerken.velocity, TIMESTEP));
                
                if (flerken.position.x - flerken.size <= 0 || flerken.position.x + flerken.size >= WINDOW_WIDTH) {
                    flerken.velocity.x *= -1;
                }
                
                if (flerken.position.y - flerken.size < 0 || flerken.position.y + flerken.size >= WINDOW_HEIGHT) {
                    flerken.velocity.y *= -1;
                }

                Vector2 flerkenFriction = Vector2Scale(flerken.velocity, -(resistance / flerken.mass) * TIMESTEP);
                flerken.velocity = Vector2Add(flerken.velocity, flerkenFriction);
                if (Vector2Length(flerken.velocity) < 1.0f) {
                    flerken.isActive = false;       // Keep this muna; I'll implement that librarian can "recall" flerken
                }

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
            
            
            for (Book &b : books) {
                // If books is collected, start cooldown timer
                if (!b.isActive) {
                    b.respawnCooldown -= deltaTime;
                    if (b.respawnCooldown <= 0) {
                        b.center = SpawnRandomBook();
                        b.isActive = true;
                    }
                    continue;
                }

                // Check for librarian collision
                if (CheckLibrarianBookCollision(librarian.position, librarian.size, b.center, b.size)) {
                    b.isActive = false;
                    b.respawnCooldown = BOOK_RESPAWN_DELAY;
                }
            }


        }







        // RENDERING
        BeginDrawing();
        ClearBackground(BLACK);

        for (Book &b : books) {
            if (b.isActive) {
                DrawCenteredRectangle(b.center, b.size, b.color);
            }
        }
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
