#include <raylib.h>
#include <raymath.h>
#include <vector>  
#include <string>
#include <set>
#include <algorithm>
using namespace std;


const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;
const float FPS = 60;
const int MAX_CIRCLES = 5000;
const int CELL_SIZE = 50;

struct GridCell;

typedef struct {
    int id;
    Vector2 position;
    // Vector4 positionMinMax;
    float radius;
    float mass;
    float inverse_mass;
    Vector2 velocity;
    Color color;
    set<GridCell*> currentCells;
} Circle;

struct GridCell {
    float cellSize;
    Vector2 position;
    set<Circle*> circles;
};

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
            c.id = *circleCount;
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
        c.id = *circleCount;
        c.radius = minRadius + (float)rand() / (float)RAND_MAX + (maxRadius - minRadius);
        c.mass = 2.0f;
        c.inverse_mass = 1.0f / c.mass;
        c.position = {WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2};
        c.velocity.x = minVelocity + (float)rand() / (float)RAND_MAX * (maxVelocity - minVelocity);
        c.velocity.y = minVelocity + (float)rand() / (float)RAND_MAX * (maxVelocity - minVelocity);
        // c.velocity.x = 100;
        // c.velocity.y = 100;
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

    bool isPaused = false;
    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        int operations = 0;

        if (IsKeyPressed(KEY_SPACE)) {
            SpawnCircles(circles, &circleCount, 25);
        }

        for (int i = 0; i < circleCount; i ++) {
            int minCircleX = floor((circles[i].position.x - circles[i].radius)/CELL_SIZE);
            int maxCircleX = floor((circles[i].position.x + circles[i].radius)/CELL_SIZE);
            int minCircleY = floor((circles[i].position.y - circles[i].radius)/CELL_SIZE);
            int maxCircleY = floor((circles[i].position.y + circles[i].radius)/CELL_SIZE);
            
            for (int lastCellsX = minCircleX; lastCellsX <= maxCircleX; lastCellsX++) {
                for (int lastCellsY = minCircleY; lastCellsY <= maxCircleY; lastCellsY++) {
                    operations++;
                    circles[i].currentCells.insert(&grid[lastCellsX][lastCellsY]);
                    grid[lastCellsX][lastCellsY].circles.insert(&circles[i]);
                }
            }
        }

        for (int i = 0; i < circleCount; i ++) {
                    operations++;
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

        
        for (int i = 0; i < circleCount; i ++) {
            int minCircleX = floor((circles[i].position.x - circles[i].radius)/CELL_SIZE);
            int maxCircleX = floor((circles[i].position.x + circles[i].radius)/CELL_SIZE);
            int minCircleY = floor((circles[i].position.y - circles[i].radius)/CELL_SIZE);
            int maxCircleY = floor((circles[i].position.y + circles[i].radius)/CELL_SIZE);
            
            set<GridCell*> currCells;
            for (int lastCellsX = minCircleX; lastCellsX <= maxCircleX; lastCellsX++) {
                for (int lastCellsY = minCircleY; lastCellsY <= maxCircleY; lastCellsY++) {
                    operations++;
                    currCells.insert(&grid[lastCellsX][lastCellsY]);
                }
            }
            set<GridCell*> oldCells;
            set_difference(circles[i].currentCells.begin(), circles[i].currentCells.end(), currCells.begin(), currCells.end(), inserter(oldCells, oldCells.end()));
            for (GridCell* gc : oldCells) {
                // printf("removed circle %d from cell (%d, %d)\n", i, (int)gc->position.x/CELL_SIZE, (int)gc->position.y/CELL_SIZE);
                gc->circles.erase(&circles[i]);
                    operations++;
            }
            circles[i].currentCells = currCells;
        }

        for (int i = 0; i < circleCount; i ++) {
            // printf("== checking circle %d at (%d, %d) ==\n", circles[i].id, (int)circles[i].position.x/CELL_SIZE, (int)circles[i].position.y/CELL_SIZE);
            for (GridCell *collisionCell : circles[i].currentCells) {
                // printf("*** checking cell (%d, %d)\n", (int)collisionCell->position.x/CELL_SIZE, (int)collisionCell->position.y/CELL_SIZE);
                for (Circle *collisionCircle : collisionCell->circles) {
                    if (circles[i].id == (*collisionCircle).id) continue;
                    // printf("+ against circle %d ==\n", (*collisionCircle).id);
                    operations++;

                    int j = (*collisionCircle).id;
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

        // Rendering
        BeginDrawing();
        ClearBackground(WHITE);

        //Draw grid
        for (int x = 0; x < CELL_X; x++) {
            for (int y = 0; y < CELL_Y; y++) {
                    operations++;
                DrawRectangleLines(
                    (int)grid[x][y].position.x,
                    (int)grid[x][y].position.y,
                    CELL_SIZE,
                    CELL_SIZE,
                    LIGHTGRAY
                );
                char cellCircleCount[100];
                sprintf(cellCircleCount, "%d", grid[x][y].circles.size());
                DrawText(cellCircleCount, (int)grid[x][y].position.x + 5, (int)grid[x][y].position.y + 5, 5, BLACK);
                char cellCoords[100];
                sprintf(cellCoords, "%d, %d", x, y);
                DrawText(cellCoords, (int)grid[x][y].position.x + 5, (int)grid[x][y].position.y + CELL_SIZE - 15, 5, BLACK);
            }
        }

        //Draw ballz
        for (int i = 0; i < circleCount; i ++) {
                    operations++;
            // DrawRectangleV({circles[i].position.x - circles[i].radius, circles[i].position.y - circles[i].radius}, {circles[i].radius*2,circles[i].radius*2}, RED);
            DrawCircleV(circles[i].position, circles[i].radius, circles[i].color);
        }

        char circleCountText[100];
        sprintf(circleCountText, "%d", circleCount);
        DrawText(circleCountText, 5, 5, 20, RED);
        char ops[100];
        sprintf(ops, "%d", operations);
        DrawText(ops, 5, 720-30, 20, RED);


        EndDrawing();
    }
    CloseWindow();
    return 0;
}

