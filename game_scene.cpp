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
    const int MAX_SHADOWS = 4;
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
    std::vector<Spirit> spirits;
    
    float accumulator = 0;

    int texture_base = 32;
    float texture_scale = 4;

    float animation_timer = 0;
    float animation_FPS = 4;

    bool ghost_spin = true;
    float ghost_spin_timer = 0;
    float ghost_rotation = 0;
    
    Texture ghost_texture;
    Texture ghost_ectoplasm_texture;
    Texture spirit_texture;
    Texture shadow_texture;
    Texture shadow_splat_texture;

    Texture book_texture;

    Texture background;

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
        for (int i = 0; i < MAX_SPIRITS; i ++) {
            Spirit sp(spiritCounter);
            spiritCounter++;
            spirits.push_back(sp);
        }

        librarian.texture = ResourceManager::GetInstance()->GetTexture("librarian.png");
        librarian.texture_source = {0, 0, (float)texture_base, (float)texture_base};

        flerken.sitting_texture = ResourceManager::GetInstance()->GetTexture("flerken_sitting.png");
        flerken.sitting_texture_source = {0, 0, (float)texture_base, (float)texture_base};
        flerken.active_texture = ResourceManager::GetInstance()->GetTexture("flerken_active.png");
        flerken.tentacles_texture = ResourceManager::GetInstance()->GetTexture("flerken_tentacles.png");

        ghost_texture = ResourceManager::GetInstance()->GetTexture("ghost.png");
        ghost_ectoplasm_texture = ResourceManager::GetInstance()->GetTexture("ectoplasm.png");
        spirit_texture = ResourceManager::GetInstance()->GetTexture("spirit.png");
        shadow_texture = ResourceManager::GetInstance()->GetTexture("shadow.png");
        shadow_splat_texture = ResourceManager::GetInstance()->GetTexture("shadow_splat.png");

        book_texture = ResourceManager::GetInstance()->GetTexture("book.png");
        background = ResourceManager::GetInstance()->GetTexture("background.png");
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
                flerken.isReturning = false;
                flerken.isActive = true;
                ghost_spin = true;
            }
        }

        if (flerken.isActive && Vector2Length(flerken.velocity) <= 1.0f) {
            if (IsKeyPressed(KEY_R)) {
                flerken.isReturning = true;
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

        librarian.hurt_timer -= deltaTime;
        accumulator += deltaTime;
        while (accumulator >= TIMESTEP) {
            float baseResistance = 1.0f;
            float resistance = baseResistance;
            // Librarian Physics
            if (librarian.isSlowed) {
                resistance *= librarian.slowMult;
            }
            librarian.velocity = Vector2Add(librarian.velocity, Vector2Scale(librarian.acceleration, TIMESTEP));
            Vector2 friction = Vector2Scale(librarian.velocity, -(resistance / librarian.mass) * TIMESTEP);
            librarian.velocity = Vector2Add(librarian.velocity, friction);

            librarian.position = Vector2Add(librarian.position, Vector2Scale(librarian.velocity, TIMESTEP));
            float speed = Vector2Length(librarian.velocity);
            cout << speed << endl;
            

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
                          // Keep this muna; I'll implement that librarian can "recall" flerken
                    flerken.isReturning = true;
                }
            }

            if (flerken.isReturning) {
                Vector2 toLibrarian = Vector2Subtract(librarian.position, flerken.position);
                float distance = Vector2Length(toLibrarian);
                Vector2 librarianDir = Vector2Scale(toLibrarian, 1/distance);
                float returnSpeed = flerken.speed;

                flerken.position = Vector2Add(flerken.position, Vector2Scale(librarianDir, returnSpeed * TIMESTEP));

                if (distance < 5.0f) {
                    flerken.position = librarian.position;
                    flerken.velocity = Vector2Zero();
                    flerken.isActive = false;
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
                        librarian.hurt_timer = 1.0f;
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
                        librarian.isBlinded = true;
                        librarian.blindTimer = 3.0f;
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
            if (librarian.isBlinded) {
                librarian.blindTimer -= TIMESTEP;
                if (librarian.blindTimer <= 0) {
                    librarian.isBlinded = false;
                }
            }

            for (Spirit &spirit : spirits) {
                if (spirit.isActive) {

                    // Ghost moves straight across the screen from where they spawned
                    spirit.center = Vector2Add(spirit.center, Vector2Scale(spirit.velocity, TIMESTEP));

                    //Collision with Flerken
                    if(checkCollision(spirit, flerken)){
                        //no damage player
                        spirit.despawn();
                        spirit.isEaten = true;
                        cout << "spirit + flerken" << endl;
                        continue;   // skip movement/despawn check for this frame
                    }

                    // Collision with librarian
                    if (checkCollision(spirit, librarian)) {
                        librarian.isSlowed = true;
                        librarian.slowTimer = 1.5f;
                        cout << " SLOWED DOWN " << endl;
                        spirit.despawn();
                        continue;   // skip movement/despawn check for this frame
                    }                    

                    // Despawn once off-screen on opposite side, either left or right depending where the ghost came from
                    if (spirit.center.x < -100 || spirit.center.x > WINDOW_WIDTH + 100) {
                        spirit.despawn();
                    }

                } else {
                    // ghost respawn cool down
                    spirit.respawnCooldown -= TIMESTEP;

                    // Respawn when ready and if max ghosts not hit
                    if (spirit.respawnCooldown <= 0) {
                        spirit.spawn(spiritCounter);
                        spiritCounter++;
                    }
                }

            }
            if (librarian.isSlowed) {
                librarian.slowTimer -= TIMESTEP;
                if (librarian.slowTimer <= 0) {
                    librarian.isSlowed = false;
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
        for (Shadow &shadow : shadows) {
            if (shadow.isEaten) {
                shadow.tentacles_timer += deltaTime;
                shadow.tentacles_rotation += deltaTime*360;
                if (shadow.tentacles_timer >= 1) {
                    shadow.isEaten = false;
                    shadow.tentacles_timer = 0;
                    shadow.tentacles_rotation = 0;
                }
            }
        }
        for (Spirit &spirit : spirits) {
            if (spirit.isEaten) {
                spirit.tentacles_timer += deltaTime;
                spirit.tentacles_rotation += deltaTime*360;
                if (spirit.tentacles_timer >= 1) {
                    spirit.isEaten = false;
                    spirit.tentacles_timer = 0;
                    spirit.tentacles_rotation = 0;
                }
            }
        }
    }

    void Draw() override {
        ClearBackground(BLACK);
        
        DrawTexturePro(
            background,
            {0, 0, 320, 180},
            {0, 0, 1280, 720},
            {0, 0},
            0,
            WHITE
        );

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
            ColorLerp(WHITE, RED, librarian.hurt_timer)
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
                DrawTexturePro(
                    ghost_ectoplasm_texture,
                    {0, 0, (float)texture_base, (float)texture_base},
                    {e.position.x, e.position.y, texture_base*texture_scale, texture_base*texture_scale},
                    {texture_base*texture_scale/2, texture_base*texture_scale/2},
                    0,
                    WHITE
                );
            }
        }

        for (Shadow &s : shadows) {
            if (s.isActive) {
                DrawCenteredRectangle(s.center, s.size, LIGHTGRAY);
                DrawTexturePro(
                    shadow_texture,
                    {0, 0, (float)texture_base, (float)texture_base},
                    {s.center.x, s.center.y, texture_base*texture_scale, texture_base*texture_scale},
                    {texture_base*texture_scale/2, texture_base*texture_scale/2},
                    ghost_rotation,
                    WHITE
                );
                if (librarian.isBlinded) {
                    float alpha = librarian.blindTimer;  // fades out as timer ends
                    // DrawRectangle(350, 250, 600, 300, Fade(LIGHTGRAY, alpha));  // Fades 
                    DrawTexturePro(
                        shadow_splat_texture,
                        {0, 0, 320, 180},
                        {0, 0, 1280, 720},
                        {0, 0},
                        0,
                        Fade(WHITE, alpha)
                    );
                }

            }
            if (s.isEaten) {
                DrawTexturePro(
                    flerken.tentacles_texture,
                    {0, 0, (float)texture_base, (float)texture_base},
                    {s.center.x, s.center.y, Lerp( texture_base*texture_scale, 0, s.tentacles_timer), Lerp(texture_base*texture_scale, 0,  s.tentacles_timer)},
                    Vector2Lerp({texture_base*texture_scale/2, texture_base*texture_scale/2}, {0, 0}, s.tentacles_timer),
                    s.tentacles_rotation,
                    WHITE
                );
            }
            
        }

        for (Spirit &sp : spirits) {
            if (sp.isActive) {
                DrawCenteredRectangle(sp.center, sp.size, BLUE);
                DrawTexturePro(
                    spirit_texture,
                    {0, 0, (float)texture_base, (float)texture_base},
                    {sp.center.x, sp.center.y, texture_base*texture_scale, texture_base*texture_scale},
                    {texture_base*texture_scale/2, texture_base*texture_scale/2},
                    ghost_rotation,
                    WHITE
                );
                

            }
            if (sp.isEaten) {
                DrawTexturePro(
                    flerken.tentacles_texture,
                    {0, 0, (float)texture_base, (float)texture_base},
                    {sp.center.x, sp.center.y, Lerp( texture_base*texture_scale, 0, sp.tentacles_timer), Lerp(texture_base*texture_scale, 0,  sp.tentacles_timer)},
                    Vector2Lerp({texture_base*texture_scale/2, texture_base*texture_scale/2}, {0, 0}, sp.tentacles_timer),
                    sp.tentacles_rotation,
                    WHITE
                );
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