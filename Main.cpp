#include <iostream>
#include <raylib.h>
#include <cmath>
using namespace std;

struct Player {
  // Set player's initial position in the middle of the screen
  Vector2 playerPos = {400, 300};
  int size = 25;
  int speed = 200;
  Color color = BLUE;
  Vector2 playerDir;
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
  float maxRadius = 100.0f;
  float elapsed = 0.0f;
  float duration = 0.5f;
};

const int MAX_BULLETS = 20;
const float bulletSpeed = 300.0f;
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

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

	while (!WindowShouldClose()) {
    BeginDrawing();

    float frametime = GetFrameTime();

    DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, BLACK);

    player.playerDir = {0, 0};

    bool dirPressed[4] = {IsKeyDown(KEY_S), IsKeyDown(KEY_W), IsKeyDown(KEY_A), IsKeyDown(KEY_D)};
    player.playerDir.x = (!dirPressed[2] != !dirPressed[3])*(dirPressed[2]?-1:1);
    player.playerDir.y = (!dirPressed[0] != !dirPressed[1])*(dirPressed[1]?-1:1);
    bool isDiagonal = (dirPressed[0] || dirPressed[1]) && (dirPressed[2] || dirPressed[3]);
    player.playerPos.x += (isDiagonal?M_SQRT1_2:1)*player.playerDir.x*player.speed*frametime;
    player.playerPos.y += (isDiagonal?M_SQRT1_2:1)*player.playerDir.y*player.speed*frametime;

    if (shotCooldown) {
      timer -= frametime;
      if (timer <= 0.0f) {
        shotCooldown = false;
      }
    }
    
    // Vector formula
    float len = sqrt(player.playerDir.x * player.playerDir.x + player.playerDir.y * player.playerDir.y);
    Vector2 normDir = {player.playerDir.x / len, player.playerDir.y / len};

    if (
      (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsMouseButtonDown(MOUSE_LEFT_BUTTON)) 
      && !shotCooldown 
      && !bullets[0].isActive
    ) {

      // Checks if Player is moving; If yes, draw bullets
      if (len > 0.0f) {
        Bullet b;
        b.bulletPos = player.playerPos;
        b.bulletVel = {normDir.x * bulletSpeed, normDir.y * bulletSpeed};
        b.isActive = true;

        bullets[bulletCount] = b;
        bulletCount += 1;
        if (bulletCount == MAX_BULLETS) {
          bulletCount = 0;
        }

        timer = 0.5;
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

    //AOE Skill!

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


    //DrawCircle(player.x, player.y, player.size, player.color);
    DrawCircleV(player.playerPos, player.size, player.color);
    Color lineColor = (player.playerDir.x != 0 && player.playerDir.y != 0 ? RED : YELLOW);
    DrawCircleLinesV(
      {
        player.playerPos.x + (normDir.x*50),
        player.playerPos.y + (normDir.y*50)
      },
      10,
      lineColor
    );

    EndDrawing();
  }

  CloseWindow();
  return 0;
}

