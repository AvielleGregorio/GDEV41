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

const int MAX_BULLETS = 1;
const float bulletSpeed = 550.0f;

int main() {
  SetConfigFlags(FLAG_WINDOW_HIGHDPI);
	InitWindow(800, 600, "Pew Pew Pew");

  Player player;
  Bullet bullets[MAX_BULLETS];
  int bulletCount = 0;

  float timer = 0.0f;
  bool shotCooldown = false;

	while (!WindowShouldClose()) {
    BeginDrawing();

    float frametime = GetFrameTime();

    DrawRectangle(0, 0, 800, 600, BLACK);

    player.playerDir = {0, 0};

    if (IsKeyDown(KEY_S)){
        player.playerPos.y += player.speed*frametime;
        player.playerDir.y = 1; 
    }
    if (IsKeyDown(KEY_W)){
        player.playerPos.y -= player.speed*frametime;
        player.playerDir.y = -1;
    }
    if (IsKeyDown(KEY_A)){
        player.playerPos.x -= player.speed*frametime;
        player.playerDir.x = -1;
    }
    if (IsKeyDown(KEY_D)){
        player.playerPos.x += player.speed*frametime;
        player.playerDir.x = 1;
    }

    if (shotCooldown) {
      timer -= frametime;
      if (timer <= 0.0f) {
        // TODO: change this implementation for multiple bullets
        bullets[0].isActive = false;
        shotCooldown = false;
      }
    }

    if ((IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsMouseButtonDown(MOUSE_LEFT_BUTTON)) && !shotCooldown && !bullets[0].isActive) {
      // Vector formula
      float len = sqrt(player.playerDir.x * player.playerDir.x + player.playerDir.y * player.playerDir.y);

      // Checks if Player is moving; If yes, draw bullets
      if (len > 0.0f) {
        Bullet b;
        b.bulletPos = player.playerPos;
        Vector2 normDir = {player.playerDir.x / len, player.playerDir.y / len};
        b.bulletVel = {normDir.x * bulletSpeed, normDir.y * bulletSpeed};
        b.isActive = true;

        bullets[0] = b;

        timer = 1;
        shotCooldown = true;
      }
    }

    if (bullets[0].isActive) {
      bullets[0].bulletPos.x += bullets[0].bulletVel.x * frametime;
      bullets[0].bulletPos.y += bullets[0].bulletVel.y * frametime;
      DrawCircleV(bullets[0].bulletPos, 5, RED);
    }

    //DrawCircle(player.x, player.y, player.size, player.color);
    DrawCircleV(player.playerPos, player.size, player.color);

    EndDrawing();
  }

  CloseWindow();
  return 0;
}

