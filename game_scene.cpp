#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <iostream>
#include "scene_manager.hpp"
#include "game.hpp"

struct UIManager {
    int booksCollected = 0;
    float gameTimer = 0.0f;
    int playerHealth = 100;
    int playerScore = 0;
    // float playerSpeed = 0;

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
        // DrawText(TextFormat("Speed: %.2f", playerSpeed), 20, 180, 22, WHITE);

    }
};


void DrawCenteredRectangle(Vector2 center, Vector2 size, Color color) {
    Vector2 topLeft = {
        center.x - size.x/2,
        center.y - size.y/2
    };
    DrawRectangleV(topLeft, size, color);
}

class GameScene : public Scene {
    const float FPS = 60;
    const float TIMESTEP = 1/FPS;
    
    const int MAX_BOOKS = 10;
    const int SCORE_BOOKS = 10;
    const float BOOK_RESPAWN_DELAY = 3.0f;
    const float BOOK_PICKUP_RADIUS = 35.0f;

    //Ghosts are the harmful entities, touching them reduces the healthbar, and they drop ectoplasm
    const int MAX_GHOSTS = 6;
    const int MAX_SHADOWS = 7;
    const int MAX_SPIRITS = 10;
    const float GHOST_RESPAWN_DELAY = 3.0f; //previously used for uniform respawn
    int ghostCounter = 0;
    int shadowCounter = 0;
    int spiritCounter = 0;
    int ghostDamage = 10;
    int ectoplasmDamage = 5;
    int MAX_ECTOPLASM = 4;

    // Boolean to check for mouse dragging
    bool isDragging = false;
    Vector2 dragStart = {0,0};
    Vector2 dragEnd = {0,0};
    float maxDrag = 200.0f;
    float launchMultiplier = 15.0f;

    Player librarian;
    Flerken flerken;
    UIManager ui;

    std::vector<Book> books;
    std::vector<Ghost> ghosts;
    std::vector<Ectoplasm> ectoplasms;
    std::vector<Shadow> shadows;
    
    float accumulator = 0;

    int texture_base = 32;
    float texture_scale = 4;

    float animation_timer = 0;
    float animation_FPS = 4;

    Texture ghost_texture;
    bool ghost_spin = true;
    float ghost_spin_timer = 0;
    float ghost_rotation = 0;

    Texture book_texture;

public:
    void Begin() override {
        for (int i = 0; i < MAX_BOOKS; i ++) {
            Book b;
            books.push_back(b);
        }
        for (int i = 0; i < MAX_GHOSTS; i ++) {
            Ghost g(ghostCounter);
            ghostCounter++;
            ghosts.push_back(g);
        }
        for (int i = 0; i < MAX_SHADOWS; i ++) {
            Shadow s(shadowCounter);
            shadowCounter ++;
            shadows.push_back(s);
        }

        librarian.texture = ResourceManager::GetInstance()->GetTexture("librarian.png");
        librarian.texture_source = {0, 0, (float)texture_base, (float)texture_base};

        flerken.sitting_texture = ResourceManager::GetInstance()->GetTexture("flerken_sitting.png");
        flerken.sitting_texture_source = {0, 0, (float)texture_base, (float)texture_base};
        flerken.active_texture = ResourceManager::GetInstance()->GetTexture("flerken_active.png");
        flerken.tentacles_texture = ResourceManager::GetInstance()->GetTexture("flerken_tentacles.png");

        ghost_texture = ResourceManager::GetInstance()->GetTexture("ghost.png");

        book_texture = ResourceManager::GetInstance()->GetTexture("book.png");
    }   

    void End() override {}

    void Update() override {
        float deltaTime = GetFrameTime();
        // Updates UI
        ui.Update(deltaTime); 

        // if (ui.gameTimer >= 10) {
        //     GetSceneManager()->SwitchScene(2);
        // }

        if (IsKeyPressed(KEY_ENTER)) {
            if (GetSceneManager() != nullptr) {
                GetSceneManager()->SwitchScene(2);
            }
        }


        if (!flerken.isActive) {
            // Flerken Control
            if (IsKeyPressed(KEY_SPACE)) {
                flerken.position = librarian.position;
                Vector2 mousePos = GetMousePosition();

                Vector2 flerkenDir = Vector2Normalize(Vector2Subtract(mousePos, librarian.position));

                // LAUNCH FLERKEN
                flerken.velocity = Vector2Scale(flerkenDir, flerken.speed);
                flerken.isActive = true;
                ghost_spin = true;
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
            float resistance = 1.0f;
            // Librarian Physics
            librarian.velocity = Vector2Add(librarian.velocity, Vector2Scale(librarian.acceleration, TIMESTEP));
            Vector2 friction = Vector2Scale(librarian.velocity, -(resistance / librarian.mass) * TIMESTEP);
            librarian.velocity = Vector2Add(librarian.velocity, friction);
            librarian.position = Vector2Add(librarian.position, Vector2Scale(librarian.velocity, TIMESTEP));
            if (Vector2Length(librarian.velocity) < 100.0f) {
                librarian.velocity = Vector2Zero();
            }
            // ui.playerSpeed = Vector2Length(librarian.velocity);

            // Flerken Physics
            if (flerken.isActive) {
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
            
            
            for (Book &book : books) {
                // If books is collected, start cooldown timer
                if (!book.isActive) {
                    book.respawnCooldown -= deltaTime;
                    if (book.respawnCooldown <= 0) {
                        book.spawn();
                    }
                    continue;
                }

                // Check for librarian collision
                if (checkCollision(book, librarian)) {
                    book.isActive = false;
                    book.respawnCooldown = BOOK_RESPAWN_DELAY;
                    ui.booksCollected += 1;
                    ui.playerScore += SCORE_BOOKS;
                }

            }
        
            for (Ghost &ghost : ghosts) {
                if (ghost.isActive) {

                    // Ghost moves straight across the screen from where they spawned
                    ghost.center = Vector2Add(ghost.center, Vector2Scale(ghost.velocity, TIMESTEP));

                    // Random ectoplasm drop
                    if (ectoplasms.size() < MAX_ECTOPLASM) {
                        int chance = GetRandomValue(0, 1000);  //chance of ectoplasm per frame
                        if (chance < 2) {                      
                            Ectoplasm e;
                            e.position = ghost.center;
                            e.size = 20;
                            e.isActive = true;
                            e.timeOnScreen = 6.0f;
                            ectoplasms.push_back(e);
                        }
                    }

                    //Collision with Flerken
                    if(checkCollision(ghost, flerken)){
                        //no damage player
                        ghost.despawn();
                        ghost.isEaten = true;
                        cout << "ghost + flerken" << endl;
                        continue;   // skip movement/despawn check for this frame
                    }

                    // Collision with librarian
                    // cout << "ghost pos x: " << ghost.center.x << endl;
                    if (checkCollision(ghost, librarian)) {
                        ui.playerHealth -= ghostDamage;
                        cout << "librarian + ghost " << endl;
                        ghost.despawn();
                        continue;   // skip movement/despawn check for this frame
                    }                    

                    // Despawn once off-screen on opposite side, either left or right depending where the ghost came from
                    if (ghost.center.x < -100 || ghost.center.x > WINDOW_WIDTH + 100) {
                        ghost.despawn();
                    }

                } else {
                    // ghost respawn cool down
                    ghost.respawnCooldown -= TIMESTEP;

                    // Respawn when ready and if max ghosts not hit
                    if (ghost.respawnCooldown <= 0) {
                        ghost.spawn(ghostCounter);
                        ghostCounter++;
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
                if (e.isActive && checkCircleCircleCollision(librarian, e)) {
                    // Deal damage to the player
                    ui.playerHealth -= ectoplasmDamage;  // or whatever damage you want

                    // Remove the ectoplasm after collision
                    e.isActive = false;
                    ectoplasms.erase(ectoplasms.begin() + i);
                }

            }

            for (Shadow &shadow : shadows) {
                if (shadow.isActive) {
                    // Random spawns the shadows


                    //Checks Collision with Flerken
                    if (checkCollision(shadow, flerken)) {
                        //No damage to player
                        shadow.despawn();
                        shadow.isEaten = true;
                        cout << "shadow + flerken" << endl;
                        continue;
                    }

                    //Checks collision with librarian
                    if (checkCollision(shadow, librarian)) {
                        // ADD TO UI 
                        cout << "BLINDED!" << endl;
                        shadow.despawn();
                        continue;
                    }

                } else {
                    shadow.respawnCooldown -= TIMESTEP;

                    if (shadow.respawnCooldown <= 0) {
                        shadow.spawn(shadowCounter);
                        shadowCounter++;
                    }
                }
                
            }

        }
        
        // Sprite Animation Logic
        
        animation_timer += deltaTime;

        // Librarian and Flerken
        if (animation_timer >= (1/(animation_FPS*(Vector2Length(librarian.velocity)/100))) && Vector2Length(librarian.velocity) >= 100.0f) {
            librarian.texture_source.x = (((int)(librarian.texture_source.x/texture_base)+1)%8)*texture_base;
            flerken.sitting_texture_source.x = librarian.texture_source.x;
            animation_timer = 0;
        }
        flerken.active_rotation += deltaTime*Vector2Length(flerken.velocity);

        // Ghosts
        float ghost_spin_length = 0.5; 
        if (flerken.isActive && ghost_spin) {
            ghost_spin_timer += deltaTime;
            ghost_rotation += deltaTime*360*2;
            if (ghost_spin_timer >= ghost_spin_length) {
                ghost_spin_timer = 0;
                ghost_spin = false;
            }
        }
        for (Ghost &ghost : ghosts) {
            if (ghost.isEaten) {
                ghost.tentacles_timer += deltaTime;
                ghost.tentacles_rotation += deltaTime*360;
                if (ghost.tentacles_timer >= 1) {
                    ghost.isEaten = false;
                    ghost.tentacles_timer = 0;
                    ghost.tentacles_rotation = 0;
                }
            }
        }
    }

    void Draw() override {
        ClearBackground(BLACK);

        for (Book &b : books) {
            if (b.isActive) {
                DrawCenteredRectangle(b.center, b.size, b.color);
                DrawTexturePro(
                    book_texture,
                    {0, 0, (float)texture_base, (float)texture_base},
                    {b.center.x, b.center.y, texture_base*texture_scale, texture_base*texture_scale},
                    {texture_base*texture_scale/2, texture_base*texture_scale/2},
                    0,
                    b.color
                );
            }
        }
        
        DrawCircleV(librarian.position, librarian.size, librarian.color);
        DrawTexturePro(
            librarian.texture,
            librarian.texture_source,
            {librarian.position.x, librarian.position.y, texture_base*texture_scale, texture_base*texture_scale},
            {texture_base*texture_scale/2, texture_base*texture_scale/2},
            0,
            WHITE
        );

        if (!flerken.isActive) {
            DrawTexturePro(
                flerken.sitting_texture,
                flerken.sitting_texture_source,
                {librarian.position.x, librarian.position.y, texture_base*texture_scale, texture_base*texture_scale},
                {texture_base*texture_scale/2, texture_base*texture_scale/2},
                0,
                WHITE
            );
        } else {
            DrawCircleV(flerken.position, flerken.size, ORANGE);
            DrawTexturePro(
                flerken.active_texture,
                {0, 0, (float)texture_base, (float)texture_base},
                {flerken.position.x, flerken.position.y, texture_base*texture_scale, texture_base*texture_scale},
                {texture_base*texture_scale/2, texture_base*texture_scale/2},
                flerken.active_rotation,
                WHITE
            );
        }

        for (Ghost &g : ghosts) {
            if (g.isActive) {
                DrawCenteredRectangle(g.center, g.size, LIGHTGRAY);
                DrawTexturePro(
                    ghost_texture,
                    {0, 0, (float)texture_base, (float)texture_base},
                    {g.center.x, g.center.y, texture_base*texture_scale, texture_base*texture_scale},
                    {texture_base*texture_scale/2, texture_base*texture_scale/2},
                    ghost_rotation,
                    WHITE
                );
            }
            if (g.isEaten) {
                DrawTexturePro(
                    flerken.tentacles_texture,
                    {0, 0, (float)texture_base, (float)texture_base},
                    {g.center.x, g.center.y, Lerp( texture_base*texture_scale, 0, g.tentacles_timer), Lerp(texture_base*texture_scale, 0,  g.tentacles_timer)},
                    Vector2Lerp({texture_base*texture_scale/2, texture_base*texture_scale/2}, {0, 0}, g.tentacles_timer),
                    g.tentacles_rotation,
                    WHITE
                );
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
    }
};


// Jai's Compile Code (ignore): clang++ Main.cpp libraylib.a -std=c++17 \-framework Cocoa -framework IOKit -framework CoreVideo -framework OpenGL -framework Foundation -o finalproject
// Avielle Raylib Starter Codes:
// C:\raylib\w64devkit\w64devkit.exe
// cd Documents/"School Stuff"/"[Y4] First Semester 2025"/"GDEV 41"/GDEV41
// g++ Main.cpp -o out -I raylib/ -L raylib/ -lraylib -lopengl32 -lgdi32 -lwinmm


//avielle dumpy
//ghost 
//randomly get position of ghost and drop a ectoplasm (red circle)
//collision for red circle (mimick books)