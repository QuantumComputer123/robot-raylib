#include <cmath>
#include <array>
#include <vector>
#include "raylib.h"
#include "collision.h"

using std::array;
using std::vector;

class Robot {  
public:
    Rectangle rect;
    Vector2 pos;
    float rot = 0;

    Vector2 velocity = { 0, 0 };
    float angularVelocity = 0;
    
    float mass = 10.0f;
    float inertia;

    Robot(Rectangle rect, float rotation) : rect(rect), rot(rotation) {
        pos = { rect.x, rect.y };
        inertia = (1.0f / 12.0f) * mass * (rect.width * rect.width + rect.height * rect.height);
    }

    void updateRect() {
        rect.x = pos.x;
        rect.y = pos.y;
    }

    array<Vector2, 4> getCorners(Vector2 position, float rotation) {
        float rad = rotation * (PI / 180.0f);
        float cosA = cos(rad);
        float sinA = sin(rad);

        float w2 = rect.width / 2.0f;
        float h2 = rect.height / 2.0f;

        array<Vector2, 4> offsets = {
            Vector2{-w2, -h2}, Vector2{w2, -h2},
            Vector2{w2, h2}, Vector2{-w2, h2}
        };

        array<Vector2, 4> corners;
        for (int i = 0; i < 4; i++) {
            corners[i].x = position.x + (offsets[i].x * cosA - offsets[i].y * sinA);
            corners[i].y = position.y + (offsets[i].x * sinA + offsets[i].y * cosA);
        }
        return corners;
    }

    void collide(Vector2 screen, array<Vector2, 4> corners) {
        for (const Vector2& c : corners) {
            Vector2 normal = { 0, 0 };
            float penetration = 0;

            if (c.x < 0) { normal.x = 1;  penetration = -c.x; }
            else if (c.x > screen.x) { normal.x = -1; penetration = c.x - screen.x; }
            
            if (c.y < 0) { normal.y = 1;  penetration = -c.y; }
            else if (c.y > screen.y) { normal.y = -1; penetration = c.y - screen.y; }

            if (normal.x != 0 || normal.y != 0) {
                Vector2 r = { c.x - pos.x, c.y - pos.y };

                float angRad = angularVelocity * (PI / 180.0f);
                Vector2 vCorner = {
                    velocity.x - angRad * r.y, 
                    velocity.y + angRad * r.x 
                };
                float velAlongNormal = vCorner.x * normal.x + vCorner.y * normal.y;

                if (velAlongNormal < 0) {
                    float e = 0.0f;

                    float rCrossN = (r.x * normal.y - r.y * normal.x);

                    float j = -(1.0f + e) * velAlongNormal;
                    j /= (1.0f / mass) + (rCrossN * rCrossN / inertia);

                    velocity.x += (j * normal.x) / mass;
                    velocity.y += (j * normal.y) / mass;

                    float angularImpulse = (rCrossN * j) / inertia;
                    angularVelocity += angularImpulse * (180.0f / PI);
                }

                pos.x += normal.x * penetration;
                pos.y += normal.y * penetration;
            }
        }
    }

    void move(Vector2 screen, float dt) {
        pos.x += velocity.x * dt;
        pos.y += velocity.y * dt;
        rot += angularVelocity * dt;

        collide(screen, getCorners(pos, rot));

        velocity.x *= 0.85f;
        velocity.y *= 0.85f;
        angularVelocity *= 0.85f;

        updateRect();
    }

    void turn(float num) {
        angularVelocity += num;
    }
};

class Obstacle {
public:
    Rectangle rect;
    Vector2 pos;
    float rot = 0;
    array<Vector2, 4> corners;

    Obstacle(Rectangle rect, float rotation) : rect(rect), rot(rotation) {
        pos = { rect.x, rect.y };
        corners = getCorners(rect, rot);
    }
};

int main() {
    Vector2 WINDOW_SIZE = {800,800};
    InitWindow(WINDOW_SIZE.x, WINDOW_SIZE.y, "Game");
    SetTargetFPS(60);
    Robot player = {{300,200,100,100}, 0};
    double speed = 2;
    std::array<bool, 6> pressedKeys = {0,0,0,0,0,0};
    std::vector<Obstacle> obstacles;
    int pixelsPerSec = 60 * speed;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        float acceleration = 1500.0f;
        float rotationAccel = 1500.0f;

        float rad = player.rot * (PI / 180.0f);
        if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) {
            player.velocity.x += cos(rad) * acceleration * dt;
            player.velocity.y += sin(rad) * acceleration * dt;
        }
        if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) {
            player.velocity.x -= cos(rad) * acceleration * dt;
            player.velocity.y -= sin(rad) * acceleration * dt;
        }
        if (IsKeyDown(KEY_Q) || IsKeyDown(KEY_COMMA)) {
            player.velocity.x += cos(rad - PI/2.0f) * acceleration * dt;
            player.velocity.y += sin(rad - PI/2.0f) * acceleration * dt;
        }
        if (IsKeyDown(KEY_E) || IsKeyDown(KEY_PERIOD)) {
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