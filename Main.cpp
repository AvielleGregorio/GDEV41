#include <raylib.h>
using namespace std;

int main() {
	InitWindow(800, 600, "Pew Pew Pew");

	while (!WindowShouldClose()) {
    BeginDrawing();

    float frametime = GetFrameTime();

    EndDrawing();
  }

  CloseWindow();
  return 0;
}

