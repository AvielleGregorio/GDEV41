#include <raylib.h>
#include <raymath.h>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const float FPS = 60;
const float TIMESTEP = 1/FPS;
float speed = 180;
float maxDrag = 150.0f;
float forceScale = 10.0f;
const int NUM_BALLS = 6;

// Boolean to check if Mouse is being dragged
bool isDragging = false;
Vector2 dragStart = {0, 0};
Vector2 dragEnd = {0, 0};

struct Pocket {
    int x;
    int y; 
    int rad;
    Color color;
};

struct Rail {
    Rectangle rail;
    Color color;
};

struct Ball {
    Vector2 position;
    float rad;
    Color color;
    float mass;
    float inverse_mass;
    Vector2 acceleration;
    Vector2 velocity;
};

Pocket pockets[4] = {
    {40, 40, 40, BLACK},
    {760, 40, 40, BLACK},
    {40, 560, 40, BLACK},
    {760, 560, 40, BLACK}
};

Rail rails[4] = {
    {{80, 0, 640, 35}, MAROON}, // Top Rail
    {{0, 80, 35, 440}, MAROON}, // Left Rail
    {{765, 80, 35, 440}, MAROON}, // Right Rail
    {{80, 565, 640, 35}, MAROON} // Bottom Rail
};

int current = 0;

// Collision (can be translated into the main loop)
void BallCollision (Ball &a, Ball &b) {
    Vector2 normal = Vector2Subtract(b.position, a.position);
    float ballDist = Vector2Length(normal);
    if (ballDist == 0.0f) {
        return;
    }

    normal = Vector2Scale(normal, 1.0f/ballDist);

    float repel = (a.rad + b.rad) - ballDist;
    Vector2 correction = Vector2Scale(normal, repel/2.0f);

    a.position = Vector2Subtract(a.position, correction);
    b.position = Vector2Subtract(b.position, correction);

    Vector2 relativeVel = Vector2Subtract(b.velocity, a.velocity);

    float velAlongNormal = Vector2DotProduct(relativeVel, normal);

    if (velAlongNormal > 0) {
        return;
    }

    float restitution = 1.0f;

    float j = -(1 + restitution) * velAlongNormal;
    j /= (a.inverse_mass + b.inverse_mass);

    Vector2 force = Vector2Scale(normal, j);
    a.velocity = Vector2Subtract(a.velocity, Vector2Scale(force, a.inverse_mass));
    b.velocity = Vector2Add(b.velocity, Vector2Scale(force, b.inverse_mass));
}

int main() {
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Billiards!");
    SetTargetFPS(FPS);

    Ball balls[NUM_BALLS] = {
        {{150, 300}, 35, WHITE, 1.0f, 1/1.0f, Vector2Zero(), Vector2Zero()},
        {{700, 300}, 35, DARKBLUE, 1.0f, 1/1.0f, Vector2Zero(), Vector2Zero()},
        {{170, 200}, 35, DARKBLUE, 1.0f, 1/1.0f, Vector2Zero(), Vector2Zero()},
        {{370, 450}, 35, DARKBLUE, 1.0f, 1/1.0f, Vector2Zero(), Vector2Zero()},
        {{420, 150}, 35, DARKBLUE, 1.0f, 1/1.0f, Vector2Zero(), Vector2Zero()}
    };

    


    float accumulator = 0;

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        // Cue Ball Mouse Input
        if (Vector2Length(balls[0].velocity) < 0.7f) {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                Vector2 mousePos = GetMousePosition();
                if (CheckCollisionPointCircle(mousePos, balls[0].position, balls[0].rad)) {
                    isDragging = true;
                    dragStart = balls[0].position;
                }
            }

            if (isDragging && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                dragEnd = GetMousePosition();
            }

            // Limits the Max Drag of the Cue ball
            if (isDragging) {
                dragEnd = GetMousePosition();

                Vector2 dragVec = Vector2Subtract(dragEnd, dragStart);
                float dragLen = Vector2Length(dragVec);

                if (dragLen > maxDrag) {
                    dragVec = Vector2Scale(Vector2Normalize(dragVec), maxDrag);
                    dragEnd = Vector2Add(dragStart, dragVec);
                    dragLen = maxDrag;
                }

                if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                    Vector2 forceDir = Vector2Normalize(Vector2Subtract(dragStart, dragEnd));
                    Vector2 impulse = Vector2Scale(forceDir, dragLen * forceScale);
                    balls[0].velocity = Vector2Add(balls[0].velocity, Vector2Scale(impulse, balls[0].inverse_mass));
                    isDragging = false;
                }
            }

        }

        accumulator += deltaTime;
        while (accumulator >= TIMESTEP) {
            float resistance = 0.4f;
            for (int i = 0; i < NUM_BALLS; i++) {
                balls[i].velocity = Vector2Add(balls[i].velocity, Vector2Scale(balls[i].acceleration, TIMESTEP));
                Vector2 ballFriction = Vector2Scale(balls[i].velocity, -(resistance / balls[i].mass) * TIMESTEP);
                balls[i].velocity = Vector2Add(balls[i].velocity, ballFriction);
                balls[i].position = Vector2Add(balls[i].position, Vector2Scale(balls[i].velocity, TIMESTEP));
                if (Vector2Length(balls[i].velocity) < 0.09f) {
                    balls[i].velocity = Vector2Zero();
                }
            }

            accumulator -= TIMESTEP;

            // Checks for collistion for every pair of ball
            for (int i = 0; i < NUM_BALLS; i++) {
                for (int j = i + 1; j < NUM_BALLS; j++) {
                    float dist = Vector2Distance(balls[i].position, balls[j].position);
                    if (dist < balls[i].rad + balls[j].rad) {
                        BallCollision(balls[i], balls[j]);
                    }
                }
            }

            for (int i = 0; i < NUM_BALLS; i++) {
                // Left Rial Boundary
                if (balls[i].position.x - balls[i].rad < rails[1].rail.x + rails[1].rail.width) {
                    balls[i].position.x = rails[1].rail.x + rails[1].rail.width + balls[i].rad;
                    balls[i].velocity.x  *= -1;
                }

                // Right Rail Boundary
                if (balls[i].position.x + balls[i].rad > rails[2].rail.x) {
                    balls[i].position.x = rails[2].rail.x - balls[i].rad;
                    balls[i].velocity.x  *= -1;
                }

                // Top Rail Boundary
                if (balls[i].position.y - balls[i].rad < rails[0].rail.y + rails[0].rail.height) {
                    balls[i].position.y = rails[0].rail.y + rails[0].rail.height + balls[i].rad;
                    balls[i].velocity.y  *= -1;
                }

                // Bottom Rail Boundary
                if (balls[i].position.y + balls[i].rad > rails[3].rail.y) {
                    balls[i].position.y = rails[3].rail.y - balls[i].rad;
                    balls[i].velocity.y  *= -1;
                }
            }

        }

        // Rendering
        BeginDrawing();
        ClearBackground(LIME);
        for (int i = 0; i < NUM_BALLS; i++) {
            DrawCircle(pockets[i].x, pockets[i].y, pockets[i].rad, pockets[i].color);
            DrawRectangleRec(rails[i].rail, rails[i].color);
            DrawCircleV(balls[i].position, balls[i].rad, balls[i].color);  
            // If you notice, there's one ball that is slightly a different shade of blue.
            // IDK why :'>
        }

        if (isDragging) {
            DrawLineV(dragStart, dragEnd, BLACK);
        }

        EndDrawing();
    }
    CloseWindow();
    return 0;
}

