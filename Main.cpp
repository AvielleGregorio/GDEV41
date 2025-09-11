#include <raylib.h>
using namespace std;

struct Player {
  // Set player's initial position in the middle of the screen
  float x = 400.0f;
  float y = 300.0f;
  int size = 25;
  int speed = 200;
  Color color = BLUE;
};


int main() {
	InitWindow(800, 600, "Pew Pew Pew");

  Player player;

	while (!WindowShouldClose()) {
    BeginDrawing();

    float frametime = GetFrameTime();

    DrawRectangle(0, 0, 800, 600, BLACK);

    if (IsKeyDown(KEY_S)){
        player.y += player.speed*frametime;
    }
    if (IsKeyDown(KEY_W)){
        player.y -= player.speed*frametime;
    }
    if (IsKeyDown(KEY_A)){
        player.x -= player.speed*frametime;
    }
    if (IsKeyDown(KEY_D)){
        player.x += player.speed*frametime;
    }

    DrawCircle(player.x, player.y, player.size, player.color);

    EndDrawing();
  }

  CloseWindow();
  return 0;
}

