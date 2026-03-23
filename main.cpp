#include <cmath>
#include "raylib.h"
#include <array>

using std::array;

class Robot
{
public: 
    Rectangle rect = { 400, 300, 100, 100 };
    Vector2 pos = {rect.x, rect.y};
    float rot = 0;

    Vector2 velocity = {0,0};
    float angularVelocity = 0;
    float mass = 10;
    float inertia;
    
    array<Vector2, 4> robotCornerPositions;

    Robot(Rectangle rect, int rotation) : rect(rect), rot(rotation) { 
        pos = {rect.x, rect.y};
        inertia = (1.0f / 6.0f) * mass * (rect.width * rect.width);
    }

    void updateRect() {
        rect.x = pos.x;
        rect.y = pos.y;

        robotCornerPositions = updateCorners(pos, rot);
    }

    array<Vector2, 4> updateCorners(Vector2 position, float rotation) {
        float rad = rotation * (PI / 180.0f);
        float cosA = cos(rad);
        float sinA = sin(rad);

        array<Vector2, 4> cornerPositions;

        float w2 = rect.width / 2.0f;
        float h2 = rect.height / 2.0f;

        Vector2 offsets[4] = {
            { -w2, -h2 },
            {  w2, -h2 },
            {  w2,  h2 },
            { -w2,  h2 }
        };

        for (int i = 0; i < 4; i++) {
            cornerPositions[i].x = position.x + (offsets[i].x * cosA - offsets[i].y * sinA);
            cornerPositions[i].y = position.y + (offsets[i].x * sinA + offsets[i].y * cosA);
        }

        return cornerPositions;
    }

    bool isLegal(Vector2 p, float r, Vector2 screen) {
        auto corners = updateCorners(p, r);
        for (const auto& c : corners) {
            if (c.x < 0 || c.x > screen.x || c.y < 0 || c.y > screen.y) return false;
        }
        return true;
    }

    void move(Vector2 screenSize, float dt) {
        Vector2 nextX = {pos.x + velocity.x * dt, pos.y};
        if (isLegal(nextX, rot, screenSize)) {
            pos.x = nextX.x;
        } else {
            velocity.x = 0;
        }

        Vector2 nextY = {pos.x, pos.y + velocity.y * dt};
        if (isLegal(nextY, rot, screenSize)) {
            pos.y = nextY.y;
        } else {
            velocity.y = 0;
        }

        float nextRot = rot + angularVelocity * dt;
        if (isLegal(pos, nextRot, screenSize)) {
            rot = nextRot;
        } else {
            angularVelocity = 0;
        }

        velocity.x *= 0.1f * dt; 
        velocity.y *= 0.1f * dt;
        angularVelocity *= 0.1f * dt; 

        updateRect();
    }


    void turn(float num) {
        angularVelocity += num;
    }
};

int main() {
    Vector2 WINDOW_SIZE = {600,400};
    InitWindow(WINDOW_SIZE.x, WINDOW_SIZE.y, "Game");
    SetTargetFPS(60);
    Robot player = {{300,200,100,100}, 0};
    double speed = 2;
    std::array<bool, 6> pressedKeys = {0,0,0,0,0,0};
    int pixelsPerSec = 60 * speed;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        float acceleration = 7000.0f;
        float rotationAccel = 5500.0f;

        float rad = player.rot * (PI / 180.0f);
        if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) {
            player.velocity.x += cos(rad) * acceleration * dt;
            player.velocity.y += sin(rad) * acceleration * dt;
        }
        if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) {
            player.velocity.x -= cos(rad) * acceleration * dt;
            player.velocity.y -= sin(rad) * acceleration * dt;
        }
        if (IsKeyDown(KEY_Q)) {
            player.velocity.x += cos(rad - PI/2.0f) * acceleration * dt;
            player.velocity.y += sin(rad - PI/2.0f) * acceleration * dt;
        }
        if (IsKeyDown(KEY_E)) {
            player.velocity.x += cos(rad + PI/2.0f) * acceleration * dt;
            player.velocity.y += sin(rad + PI/2.0f) * acceleration * dt;
        }

        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))  { 
            player.turn(-rotationAccel * dt);
        }
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) { 
            player.turn(rotationAccel * dt);
        }

        player.move(WINDOW_SIZE, dt); 

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawRectanglePro(player.rect, (Vector2){player.rect.width/2, player.rect.height/2}, player.rot, BLUE);
        EndDrawing();
}

    CloseWindow();
    return 0;
}