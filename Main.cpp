#include <raylib.h>
#include <raymath.h>
#include <vector>  
using namespace std;


const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;
const float FPS = 60;
const int MAX_CIRCLES = 5000;
const int CELL_SIZE = 50;

typedef struct {
    Vector2 position;
    float radius;
    float mass;
    float inverse_mass;
    Vector2 velocity;
    Color color;
    Vector2 currentCell;
} Circle;

typedef struct {
    float cellSize;
    Vector2 position;
} GridCell;

Circle circles[MAX_CIRCLES];
int circleCount = 0;

const int CELL_X = WINDOW_WIDTH / CELL_SIZE + 1; 
const int CELL_Y = WINDOW_HEIGHT / CELL_SIZE + 1; 

GridCell grid[CELL_X][CELL_Y]; 


void SpawnCircles(Circle circles[], int*circleCount, int spawnCount) {
    static int spawnEvents = 0;

    float minRadius = 5;
    float maxRadius = 10;
    float minVelocity = -100.0f;
    float maxVelocity = 200.0f;

    spawnEvents ++;

    if (spawnEvents % 11 == 0) {
        if (*circleCount < MAX_CIRCLES) {
            Circle c = {0};
            c.radius = 25.0f;
            c.mass = 5.0f;
            c.inverse_mass = 1.0f / c.mass;
            c.position = {WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2};
            c.velocity = {0, 0};
            c.color = (Color) {(unsigned char)GetRandomValue(0, 255), (unsigned char)GetRandomValue(0, 255), (unsigned char)GetRandomValue(0, 255), 255};
            circles[(*circleCount)++] = c;
        }
        return;
    }

    for (int i = 0; i < spawnCount && *circleCount < MAX_CIRCLES; i++) {
        Circle c = {0};
        c.radius = minRadius + (float)rand() / (float)RAND_MAX + (maxRadius - minRadius);
        c.mass = 2.0f;
        c.inverse_mass = 1.0f / c.mass;
        c.position = {WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2};
        c.velocity.x = minVelocity + (float)rand() / (float)RAND_MAX * (maxVelocity - minVelocity);
        c.velocity.y = minVelocity + (float)rand() / (float)RAND_MAX * (maxVelocity - minVelocity);
        c.color = (Color) {(unsigned char)GetRandomValue(0, 255), (unsigned char)GetRandomValue(0, 255), (unsigned char)GetRandomValue(0, 255), 255};
        circles[(*circleCount)++] = c;
    }
}

void CircleCollision (Circle circles[], int circleCount) {
    for (int i = 0; i < circleCount; i ++) {
        for (int j = 0; j < circleCount; j++) {
            if (i == j) continue;
            Vector2 normal = Vector2Subtract(circles[j].position, circles[i].position);
            float circleDist = Vector2Length(normal);
            float minDist = circles[i].radius + circles[j].radius;

            if (circleDist < minDist && circleCount > 0) {
                normal = Vector2Scale(normal, 1.0f/circleDist);
                float overlap = minDist - circleDist;
                
                Vector2 correction = Vector2Scale(normal, overlap/2.0f);

                circles[i].position = Vector2Subtract(circles[i].position, correction);
                circles[j].position = Vector2Add(circles[j].position, correction);

                Vector2 relativeVel = Vector2Subtract(circles[j].velocity, circles[i].velocity);
                float velAlongNormal = Vector2DotProduct(relativeVel, normal);

                if (velAlongNormal > 0) continue;

                float elasticity = 1.0f;

                float jImpulse = -(1 + elasticity) * velAlongNormal;
                jImpulse /= (circles[i].inverse_mass + circles[j].inverse_mass);

                Vector2 force = Vector2Scale(normal, jImpulse);
                circles[i].velocity = Vector2Subtract(circles[i].velocity, Vector2Scale(force, circles[i].inverse_mass));
                circles[j].velocity = Vector2Add(circles[j].velocity, Vector2Scale(force, circles[j].inverse_mass));
            }
        }
    }
}


int main() {
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Uniform Grid");
    SetTargetFPS(FPS);


    for (int x = 0; x < CELL_X; x++) {
        for (int y = 0; y < CELL_Y; y++) {
            grid[x][y].cellSize = CELL_SIZE;
            grid[x][y].position = {(float)x * CELL_SIZE, (float)y * CELL_SIZE};
        }
    }

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        if (IsKeyPressed(KEY_SPACE)) {
            SpawnCircles(circles, &circleCount, 25);
        }

        for (int i = 0; i < circleCount; i ++) {
            circles[i].position.x += circles[i].velocity.x * deltaTime;
            circles[i].position.y += circles[i].velocity.y * deltaTime;

            if (circles[i].position.x - circles[i].radius < 0) {
                circles[i].position.x = circles[i].radius;
                circles[i].velocity.x *= -1;
            }
            if (circles[i].position.x + circles[i].radius > WINDOW_WIDTH) {
                circles[i].position.x = WINDOW_WIDTH - circles[i].radius;
                circles[i].velocity.x *= -1;
            }
            if (circles[i].position.y - circles[i].radius < 0) {
                circles[i].position.y = circles[i].radius;
                circles[i].velocity.y *= -1;
            }
            if (circles[i].position.y + circles[i].radius > WINDOW_HEIGHT) {
                circles[i].position.y = WINDOW_HEIGHT - circles[i].radius;
                circles[i].velocity.y *= -1;
            }

        }

        CircleCollision(circles, circleCount);

        // Rendering
        BeginDrawing();
        ClearBackground(WHITE);

        //Draw grid
        for (int x = 0; x < CELL_X; x++) {
            for (int y = 0; y < CELL_Y; y++) {
                DrawRectangleLines(
                    (int)grid[x][y].position.x,
                    (int)grid[x][y].position.y,
                    CELL_SIZE,
                    CELL_SIZE,
                    LIGHTGRAY
                );
            }
        }

        //Draw ballz
        for (int i = 0; i < circleCount; i ++) {
            DrawCircleV(circles[i].position, circles[i].radius, circles[i].color);
        }

        EndDrawing();
    }
    CloseWindow();
    return 0;
}

