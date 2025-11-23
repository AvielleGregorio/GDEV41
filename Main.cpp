#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <iostream>

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;
const float FPS = 60;
const float TIMESTEP = 1/FPS;

const int MAX_BOOKS = 10;
const int SCORE_BOOKS = 10;
const float BOOK_RESPAWN_DELAY = 3.0f;
const float BOOK_PICKUP_RADIUS = 35.0f;

//Ghosts are the harmful entities, touching them reduces the healthbar, and they drop ectoplasm
const int MAX_GHOSTS = 6;
const float GHOST_RESPAWN_DELAY = 3.0f; //previously used for uniform respawn
float GHOST_RESPAWN_MIN = 0.5f; //used for random respawn
float GHOST_RESPAWN_MAX = 3.0f;
int ghostCounter = 0;
int ghostDamage = 10;
int ectoplasmDamage = 5;
int MAX_ECTOPLASM = 4;

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
    //Temporary Ghost Dimensions
    Vector2 size;
    float speed = 100;
    float mass = 2.0f;
    bool isActive; 
    float respawnCooldown;  
};

struct Ectoplasm {
    Vector2 position;
    // To be replaced when Sprite
    // Texture playerTexture;
    // Rectangle textureSource;
    //Temporary Ghost Dimensions
    float size;
    float speed = 100;
    bool isActive; 
    float timeOnScreen = 6.0f;
    Color color = RED;
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

struct UIManager {
    int booksCollected = 0;
    float gameTimer = 0.0f;
    int playerHealth = 100;
    int playerScore = 0;

    void Update(float deltaTime) {
        gameTimer += deltaTime;
    }

    void Draw() { //Drawing the UI elements itself
        // Draw the book counter
        DrawText(TextFormat("Books: %d", booksCollected), 20, 20, 22, YELLOW);

        // Draw the timer
        DrawText(TextFormat("Time: %.2f", gameTimer), 20, 60, 22, SKYBLUE);

        // Draw the Health Bar
        int barX = 20;
        int barY = 100;
        int barWidth = 200;
        int barHeight = 25;

        DrawText("HP", 20, 100, 22, GREEN);

        DrawRectangle(barX + 40, barY, barWidth, barHeight, LIGHTGRAY);
        DrawRectangle(barX + 40, barY, playerHealth * 2, barHeight, GREEN); 

        //Draw Score
        DrawText(TextFormat("Score: %d", playerScore), 20, 140, 22, WHITE);

    }
};


void DrawCenteredRectangle(Vector2 center, Vector2 size, Color color) {
    Vector2 topLeft = {
        center.x - size.x/2,
        center.y - size.y/2
    };
    DrawRectangleV(topLeft, size, color);
}

Vector2 SpawnRandomGhost(int ghostCount) {
    Vector2 position;

    float ghostPositionX = GetRandomValue(-10, -1);
    float ghostPositionAlternateX = GetRandomValue(1285, 1300);
    float ghostPositionY = GetRandomValue(0, 600);

    //Since ghosts should be coming from either side, I will spread them like so
    if(ghostCount % 2 == 0){
        position.x = ghostPositionX;
        position.y = ghostPositionY;
    }else{
        position.x = ghostPositionAlternateX;
        position.y = ghostPositionY;
    }

    return position;
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

bool CheckLibrarianGhostCollision(Vector2 playerPos, float size, Vector2 ghostPosition, Vector2 ghostSize) {
    // Compute half-size
    Vector2 half = {ghostSize.x/2, ghostSize.y/2};

    // Find closest point on book to player
    float closestX = Clamp(playerPos.x, ghostPosition.x - half.x, ghostPosition.x + half.x);
    float closestY = Clamp(playerPos.y, ghostPosition.y - half.y, ghostPosition.y + half.y);

    // Distance from player to closest point
    float distX = playerPos.x - closestX;
    float distY = playerPos.y - closestY;

    return (distX * distX + distY * distY) <= (size * size);
}

bool CheckLibrarianEctoplasmCollision(Vector2 playerPos, float size, Vector2 ectoPosition, float ectoSize) {
    return Vector2DistanceSqr(playerPos, ectoPosition) <= (size + ectoSize)*(size + ectoSize);
}

//Oke, i know this is redundant, but its mostly for readability sake
bool CheckFlerkenGhostCollision(Vector2 flerkenPos, float size, Vector2 ghostPosition, Vector2 ghostSize) {
    // Compute half-size
    Vector2 half = {ghostSize.x/2, ghostSize.y/2};

    // Find closest point on book to player
    float closestX = Clamp(flerkenPos.x, ghostPosition.x - half.x, ghostPosition.x + half.x);
    float closestY = Clamp(flerkenPos.y, ghostPosition.y - half.y, ghostPosition.y + half.y);

    // Distance from player to closest point
    float distX = flerkenPos.x - closestX;
    float distY = flerkenPos.y - closestY;

    return (distX * distX + distY * distY) <= (size * size);
}


int main() {
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Midnight Librarian");
    SetTargetFPS(FPS);

    Player librarian;
    Flerken flerken{};
    UIManager ui;

    std::vector<Book> books;
    for (int i = 0; i < MAX_BOOKS; i ++) {
        Book b;
        b.center = SpawnRandomBook();
        b.size = {25,30};
        b.isActive = true;
        b.respawnCooldown = 0;
        books.push_back(b);
    }

    std::vector<Ghost> ghosts;
    for (int i = 0; i < MAX_GHOSTS; i ++) {
        Ghost g;
        g.position = SpawnRandomGhost(ghostCounter);
        g.size = {35,40};
        g.isActive = true;
        g.respawnCooldown = GetRandomValue(GHOST_RESPAWN_MIN * 1000, GHOST_RESPAWN_MAX * 1000) / 1000.0f;

        if (g.position.x < 0) {
            g.velocity = { g.speed, 0 }; // Move right
        } else {
            g.velocity = { -g.speed, 0 }; // Move left
        }

        ghostCounter++;

        ghosts.push_back(g);
    }

    std::vector<Ectoplasm> ectoplasms;
    
    float accumulator = 0;


    while (!WindowShouldClose()) {
        // Code Starts here
        float deltaTime = GetFrameTime();
        // Updates UI
        ui.Update(deltaTime); 

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
                    ui.booksCollected += 1;
                    ui.playerScore += SCORE_BOOKS;
                }

            }
        
            for (Ghost &g : ghosts) {
                if (g.isActive) {

                    // Ghost moves straight across the screen from where they spawned
                    g.position = Vector2Add(g.position, Vector2Scale(g.velocity, TIMESTEP));

                    // Random ectoplasm drop
                    if (ectoplasms.size() < MAX_ECTOPLASM) {
                        int chance = GetRandomValue(0, 1000);  //chance of ectoplasm per frame
                        if (chance < 2) {                      
                            Ectoplasm e;
                            e.position = g.position;
                            e.size = 20;
                            e.isActive = true;
                            e.timeOnScreen = 6.0f;
                            ectoplasms.push_back(e);
                        }
                    }

                    //Collision with Flerken
                    if(CheckFlerkenGhostCollision(flerken.position, flerken.size, g.position, g.size)){
                        //no damage player
                        g.isActive = false;
                        g.respawnCooldown = GetRandomValue(GHOST_RESPAWN_MIN*1000, GHOST_RESPAWN_MAX*1000) / 1000.0f;
                        continue;   // skip movement/despawn check for this frame
                    }

                    // Collision with librarian
                    if (CheckLibrarianGhostCollision(librarian.position, librarian.size, g.position, g.size)) {
                        ui.playerHealth -= ghostDamage;
                        g.isActive = false;
                        g.respawnCooldown = GetRandomValue(GHOST_RESPAWN_MIN*1000, GHOST_RESPAWN_MAX*1000) / 1000.0f;
                        continue;   // skip movement/despawn check for this frame
                    }                    

                    // Despawn once off-screen on opposite side, either left or right depending where the ghost came from
                    if (g.position.x < -100 || g.position.x > WINDOW_WIDTH + 100) {
                        g.isActive = false;
                        g.respawnCooldown = GetRandomValue(GHOST_RESPAWN_MIN * 1000, GHOST_RESPAWN_MAX * 1000) / 1000.0f;
                        std::cout << "ghost despawned!";
                    }

                } else {
                    // ghost respawn cool down
                    g.respawnCooldown -= TIMESTEP;

                    // Respawn when ready and if max ghosts not hit
                    if (g.respawnCooldown <= 0) {
                        g.position = SpawnRandomGhost(ghostCounter);

                        // Assign direction again depending on side spawned
                        if (g.position.x < 0) {
                            g.velocity = { g.speed, 0 };  // Move right acorss the screen
                        } else {
                            g.velocity = { -g.speed, 0 }; // Move left across the screen
                        }
                        std::cout << "ghost spawned";
                        ghostCounter++;
                        g.isActive = true;
                    }
                }

            }

            //checking the ectoplasms despawn
            for (int i = ectoplasms.size() - 1; i >= 0; i--) {
                Ectoplasm &e = ectoplasms[i];
                //checks ectoplasms despawn
                ectoplasms[i].timeOnScreen -= TIMESTEP;

                if (ectoplasms[i].timeOnScreen <= 0) {
                    
                    ectoplasms.erase(ectoplasms.begin() + i);
                }

                //checks collision with libaraian (circle to circle)
                if (e.isActive && CheckLibrarianEctoplasmCollision(librarian.position, librarian.size, e.position, e.size)) {
                    // Deal damage to the player
                    ui.playerHealth -= ectoplasmDamage;  // or whatever damage you want

                    // Remove the ectoplasm after collision
                    e.isActive = false;
                    ectoplasms.erase(ectoplasms.begin() + i);
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
        
        for (Ghost &g : ghosts) {
            if (g.isActive) {
                DrawCenteredRectangle(g.position, g.size, LIGHTGRAY);
            }
        }

        for (Ectoplasm &e : ectoplasms) {
            if (e.isActive) {
                DrawCircleV(e.position, e.size, RED);
            }
        }

        if (isDragging) {
            DrawLineEx(dragStart, dragEnd, 2.0f, WHITE);
        }

        //Draw the UI
        ui.Draw();

        EndDrawing();
    
    }
    CloseWindow();
    return 0;
}


// Jai's Compile Code (ignore): clang++ Main.cpp libraylib.a -std=c++17 \-framework Cocoa -framework IOKit -framework CoreVideo -framework OpenGL -framework Foundation -o finalproject
// Avielle Raylib Starter Codes:
// C:\raylib\w64devkit\w64devkit.exe
// cd Documents/"School Stuff"/"[Y4] First Semester 2025"/"GDEV 41"/GDEV41
// g++ Main.cpp -o out -I raylib/ -L raylib/ -lraylib -lopengl32 -lgdi32 -lwinmm


//avielle dumpy
//ghost 
//randomly get position of ghost and drop a ectoplasm (red circle)
//collision for red circle (mimick books)