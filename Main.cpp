#include <iostream>
#include <raylib.h>
#include <cmath>
using namespace std;

struct Player {
  // Set player's initial position in the middle of the screen
  Vector2 playerPos = {400, 300};
  int size = 35;
  int speed = 200;
  Color color = BLUE;
  Vector2 playerDir;
  Texture texture = LoadTexture("IDLE_WALK.png");
  Rectangle texSource = {0, 0, 16, 16};
};

struct Bullet {
  // Set the Bullets
  Vector2 bulletPos;
  Vector2 bulletVel; 
  bool isActive = false; 
};

struct AOE {
  bool isActive = false;
  float radius = 0.0f;
  float maxRadius = 120.0f;
  float elapsed = 0.0f;
  float duration = 0.5f;
};

const int MAX_BULLETS = 20;
const float bulletSpeed = 300.0f;
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const bool FOLLOW_MOUSE = true;

int main() {
  SetConfigFlags(FLAG_WINDOW_HIGHDPI);
	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Pew Pew Pew");

  Player player;
  Bullet bullets[MAX_BULLETS];
  int bulletCount = 0;

  float timer = 0.0f;
  float aoetimer = 0.0f;
  bool shotCooldown = false;
  bool aoeCooldown = false;

  AOE aoe;

  int currFrame = 0;
  float playerAnimTimer = 0;
  float animFPS = 2;

	while (!WindowShouldClose()) {
    BeginDrawing();

    float frametime = GetFrameTime();

    DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, BLACK);

    player.playerDir = {0, 0};

    // === PLAYER MOVEMENT ===

    // Checks which keys are being pressed
    bool dirPressed[4] = {IsKeyDown(KEY_S), IsKeyDown(KEY_W), IsKeyDown(KEY_A), IsKeyDown(KEY_D)};
    // Assigns movement direction based on keys pressed
    player.playerDir.x = (!dirPressed[2] != !dirPressed[3])*(dirPressed[2]?-1:1);
    player.playerDir.y = (!dirPressed[0] != !dirPressed[1])*(dirPressed[1]?-1:1);
    // Normalizes character movement along diagonals
    bool isDiagonal = (dirPressed[0] || dirPressed[1]) && (dirPressed[2] || dirPressed[3]);
    player.playerPos.x += (isDiagonal?M_SQRT1_2:1)*player.playerDir.x*player.speed*frametime;
    player.playerPos.y += (isDiagonal?M_SQRT1_2:1)*player.playerDir.y*player.speed*frametime;

    // === BULLETS ===

    if (shotCooldown) {
      timer -= frametime;
      if (timer <= 0.0f) {
        shotCooldown = false;
      }
    }
    
    // Vector formula
    Vector2 bulletDir;

    float len = sqrt(player.playerDir.x * player.playerDir.x + player.playerDir.y * player.playerDir.y);
    
    if (FOLLOW_MOUSE) {
      Vector2 mousePos = GetMousePosition();
      float mouseVMag = sqrt(pow(mousePos.x - player.playerPos.x, 2)+pow(mousePos.y - player.playerPos.y, 2));
      bulletDir = {(mousePos.x - player.playerPos.x)/mouseVMag, (mousePos.y - player.playerPos.y)/mouseVMag};
    } else {
      bulletDir = {player.playerDir.x / len, player.playerDir.y / len};
    }

    if (
      (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsMouseButtonDown(MOUSE_LEFT_BUTTON)) 
      && !shotCooldown 
      && !bullets[0].isActive
    ) {

      // Checks if Player is moving; If yes, draw bullets
      if (len > 0.0f) {
        Bullet b;
        b.bulletPos = player.playerPos;
        b.bulletVel = {bulletDir.x * bulletSpeed, bulletDir.y * bulletSpeed};
        b.isActive = true;

        bullets[bulletCount] = b;
        bulletCount += 1;
        if (bulletCount == MAX_BULLETS) {
          bulletCount = 0;
        }

        timer = 1;
        shotCooldown = true;
      }
    }

    for (int i = 0; i <= MAX_BULLETS; i ++) {
      if (bullets[i].isActive) {
        bullets[i].bulletPos.x += bullets[i].bulletVel.x * frametime;
        bullets[i].bulletPos.y += bullets[i].bulletVel.y * frametime;

        // Deactivates when bullet is off screen
        if (
          bullets[i].bulletPos.x < 0 ||
          bullets[i].bulletPos.x > WINDOW_WIDTH ||
          bullets[i].bulletPos.y < 0 ||
          bullets[i].bulletPos.y > WINDOW_HEIGHT
        ) {
          bullets[i].isActive = false;
        }

        DrawCircleV(bullets[i].bulletPos, 5, RED);
      }
    }

    // === AOE SKILL ===

    if (aoeCooldown) {
      aoetimer -= frametime;
      if (aoetimer <= 0.0f) {
        aoeCooldown = false;
      }
    }

    if (IsKeyPressed(KEY_SPACE) && !aoe.isActive && !aoeCooldown) {
        aoe.isActive = true;
        aoe.radius = 0.0f;
        aoe.elapsed = 0.0f;
        aoeCooldown = true;
    }

    if (aoe.isActive && aoeCooldown) {
        aoe.elapsed += frametime;
        float t = aoe.elapsed / aoe.duration;
        if (t >= 1.0f) {
            aoe.isActive = false;
            t = 1.0f;
        }

        aoe.radius = aoe.maxRadius * t;
        DrawCircleV(player.playerPos, aoe.radius, YELLOW);
        aoetimer = 5;
        shotCooldown = true;
    }

    // === CHARACTER ANIMATION ===

    bool isWalking = dirPressed[0] || dirPressed[1] || dirPressed[2] || dirPressed[3];
    if (playerAnimTimer >= 1/animFPS) {
      player.texSource.x = ((int)((player.texSource.x/16)+1)%4)*16+(isWalking*64);
      playerAnimTimer = 0;
    }
    
    if (dirPressed[0]) {
      if (dirPressed[2]) {
        player.texSource.y = 16;
        player.texSource.width = 16;
      } else if (dirPressed[3]) {
        player.texSource.y = 16;
        player.texSource.width = -16;
      } else {
        player.texSource.y = 0;
      }
    } else if (dirPressed[1]) {
      if (dirPressed[2]) {
        player.texSource.y = 48;
        player.texSource.width = 16;
      } else if (dirPressed[3]) {
        player.texSource.y = 48;
        player.texSource.width = -16;
      } else {
        player.texSource.y = 64;
      }
    } else if (dirPressed[2]) {
      player.texSource.y = 32;
        player.texSource.width = 16;
    } else if (dirPressed[3]) {
      player.texSource.y = 32;
      player.texSource.width = -16;
    }

    playerAnimTimer += frametime;

    // === DRAWING ===

    // AOE Cooldown Timer
    DrawCircleSector(
      player.playerPos,
      player.size + 10,
      0,
      ((5-aoetimer)/5)*360,
      0,
      YELLOW
    );

    // Player Circle
    DrawCircleV(player.playerPos, player.size, player.color);

    // Player Sprite
    int destSize = 64;
    DrawTexturePro(
      player.texture,
      player.texSource,
      {player.playerPos.x, player.playerPos.y, 64, 64},
      {(float)(destSize/2), (float)(destSize/2)},
      0,
      WHITE
    );

    // Bullet Direction Indicator
    DrawCircleLinesV(
      {
        player.playerPos.x + (bulletDir.x*65),
        player.playerPos.y + (bulletDir.y*65)
      },
      10,
      RED
    );

    // Bullet Cooldown Timer
    DrawCircleSector(
      {
        player.playerPos.x + (bulletDir.x*65),
        player.playerPos.y + (bulletDir.y*65)
      },
      10,
      0,
      (1-timer)*360,
      1,
      RED
    );

    EndDrawing();
  }

  CloseWindow();
  return 0;
}

