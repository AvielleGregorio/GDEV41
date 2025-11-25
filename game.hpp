#include <raylib.h>
#include <raymath.h>
#include <cmath>
#include "scene_manager.hpp"

float GHOST_RESPAWN_MIN = 0.5f; //used for random respawn
float GHOST_RESPAWN_MAX = 3.0f;

struct CircleCollider {
    string collider_type = "circle";
    Vector2 position;
    int size;
};
struct AABBCollider {
    string collider_type = "aabb";
    Vector2 center;
    Vector2 size;
};

struct Player : CircleCollider {
    int speed = 200;
    Color color = LIME;
    float mass = 5.0f;
    float inverse_mass = 1/mass;
    Vector2 acceleration = Vector2Zero();
    Vector2 velocity = Vector2Zero();
    bool isActive = true; 
    // To be replaced when Sprite
    // Texture playerTexture;
    // Rectangle textureSource;
    
    Player() {
        // Set Librarian's initial position in the middle (for now)
        position = {WINDOW_WIDTH/2, WINDOW_HEIGHT/2};
        size = 30;
    }
};

struct Flerken : CircleCollider {
    // Set the Flerken Bullet
    Vector2 velocity = Vector2Zero();
    float speed = 800;
    float mass = 2.0f;
    bool isActive = false;
    // To be replaced when Sprite
    // Texture playerTexture;
    // Rectangle textureSource;

    Flerken() {
        position;
        size = 20;
    }
};

struct Ghost : AABBCollider {
    Vector2 velocity;
    // To be replaced when Sprite
    // Texture playerTexture;
    // Rectangle textureSource;
    //Temporary Ghost Dimensions
    float speed = 100;
    float mass = 2.0f;
    bool isActive = true; 
    float respawnCooldown;

    Ghost(int ghostCount) {
        size = {35,40};
        spawn(ghostCount);
    }

    void spawn(int ghostCount) {
        isActive = true;
        center.x = ghostCount % 2 == 0 ?
            GetRandomValue(-10, -1) :
            GetRandomValue(1285, 1300);
        center.y = GetRandomValue(0, 600);
        respawnCooldown = GetRandomValue(GHOST_RESPAWN_MIN * 1000, GHOST_RESPAWN_MAX * 1000) / 1000.0f;
        velocity = center.x < 0 ?
            Vector2({speed, 0}) :
            Vector2({-speed, 0});
        cout << "ghost spawned" << endl;
    }

    void despawn() {
        isActive = false;
        respawnCooldown = GetRandomValue(GHOST_RESPAWN_MIN * 1000, GHOST_RESPAWN_MAX * 1000) / 1000.0f;
        cout << "ghost despawned!" << endl;
    }
};

struct Ectoplasm : CircleCollider {
    // To be replaced when Sprite
    // Texture playerTexture;
    // Rectangle textureSource;
    //Temporary Ghost Dimensions
    float speed = 100;
    bool isActive; 
    float timeOnScreen = 6.0f;
    Color color = RED;
};

struct Shadow{
    Vector2 position;
    Vector2 velocity;
    // To be replaced when Sprite
    // Texture playerTexture;
    // Rectangle textureSource;
};

struct Spirit{
    Vector2 position;
    Vector2 velocity;
    // To be replaced when Sprite
    // Texture playerTexture;
    // Rectangle textureSource;
};

struct Book : AABBCollider {
    bool isActive = true;                  
    float respawnCooldown = 0;          // Timer before the book can spawn back again
    Color color = SKYBLUE;
    // To be replaced when Sprite
    // Texture playerTexture;
    // Rectangle textureSource;
    
    float margin = 50.0f;

    Book() {
        size = {25, 30}; // Width and height (for now)
        spawn();
    }
    
    void spawn() {
        center.x = GetRandomValue(margin, WINDOW_WIDTH - margin);
        center.y = GetRandomValue(margin, WINDOW_HEIGHT - margin);
        isActive = true;
    }
};

bool checkCircleCircleCollision(CircleCollider circle1, CircleCollider circle2) {
    return Vector2DistanceSqr(circle1.position, circle2.position) <=(circle1.size + circle2.size) * (circle1.size + circle2.size);
}
bool checkAABBCircleCollision(AABBCollider aabb, CircleCollider circle) {
    // cout << "ghost pos x inside: " << (*aabb).center.x << endl;
    // cout << "ghost pos y inside: " << (*aabb).center.y << endl;
    // cout << "ghost size x inside: " << (*aabb).size.x << endl;
    // cout << "ghost size y inside: " << (*aabb).size.y << endl;
    Vector2 half = {aabb.size.x/2, aabb.size.y/2};

    // Find closest point on book to player
    float closestX = Clamp(circle.position.x, aabb.center.x - half.x, aabb.center.x + half.x);
    float closestY = Clamp(circle.position.y, aabb.center.y - half.y, aabb.center.y + half.y);

    // Distance from player to closest point
    float distX = circle.position.x - closestX;
    float distY = circle.position.y - closestY;

    return (distX * distX + distY * distY) <= (circle.size * circle.size);
}

bool checkCollision(CircleCollider x, CircleCollider y) {
    return checkCircleCircleCollision(x, y);
}
bool checkCollision(CircleCollider x, AABBCollider y) {
    return checkAABBCircleCollision(y, x);
}
bool checkCollision(AABBCollider x, CircleCollider y) {
    return checkAABBCircleCollision(x, y);
}