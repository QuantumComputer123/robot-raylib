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
    
    array<Vector2, 4> robotCornerPositions;

    Robot(Rectangle rect, int rotation) : rect(rect), rot(rotation) { }

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

    void move(float distance, Vector2 screenSize = {600, 400}) {
        float rad = rot * (PI / 180.0f);
        float dx = cos(rad) * distance;
        float dy = sin(rad) * distance;

        Vector2 nextX = {pos.x + dx, pos.y};
        auto cornersX = updateCorners(nextX, rot);
        bool xCanMove = true;
        for (const auto& c : cornersX) {
            if (c.x < 0 || c.x > screenSize.x) { xCanMove = false; break; }
        }
        if (xCanMove) pos.x = nextX.x;

        Vector2 nextY = {pos.x, pos.y + dy};
        auto cornersY = updateCorners(nextY, rot);
        bool yCanMove = true;
        for (const auto& c : cornersY) {
            if (c.y < 0 || c.y > screenSize.y) { yCanMove = false; break; }
        }
        if (yCanMove) pos.y = nextY.y;

        updateRect();
    }

    void turn(float num, Vector2 screenSize = {600, 400}) {
        float potentialRot = rot;
        float rad = rot * (PI / 180.0f);
        potentialRot += num;

        array<Vector2, 4> potentialCorners = updateCorners(pos, potentialRot);

        bool insideBounds = true;
        for (int i = 0; i < 4; i++) {
            if (potentialCorners[i].x > screenSize.x || potentialCorners[i].x < 0 || 
                potentialCorners[i].y > screenSize.y || potentialCorners[i].y < 0) {
                insideBounds = false;
                break; 
            }
        }

        if (insideBounds) {
            rot = potentialRot;
            updateRect();
        }
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
        float rotationSpeed = 150.0f;

        if (IsKeyDown(KEY_UP))    { player.move(pixelsPerSec * dt, WINDOW_SIZE); }
        if (IsKeyDown(KEY_DOWN))  { player.move(-pixelsPerSec * dt, WINDOW_SIZE); }
        if (IsKeyDown(KEY_LEFT))  { player.turn(-rotationSpeed * dt); }
        if (IsKeyDown(KEY_RIGHT)) { player.turn(rotationSpeed * dt); }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawRectanglePro(player.rect, (Vector2){player.rect.width/2, player.rect.height/2}, player.rot, BLUE);
        EndDrawing();
}

    CloseWindow();
    return 0;
}