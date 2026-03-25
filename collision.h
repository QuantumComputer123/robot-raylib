// collision.h
#ifndef COLLISION_H
#define COLLISION_H

#include "raylib.h"
#include <array>

// 1. Add the struct here so Robot and Main can use it
struct CollisionResult {
    bool collided = false;
    Vector2 normal = {0, 0};
    float penetration = 0;
};

std::array<Vector2, 4> getCorners(Rectangle rect, float rotation);

// 2. Add the function declaration here
CollisionResult getCollision(Rectangle rectA, float rotA, Rectangle rectB, float rotB);

bool isOverlap(Rectangle rectA, float rotA, Rectangle rectB, float rotB);
bool isOverlapOnAxis(const std::array<Vector2, 4>& cornersA, const std::array<Vector2, 4>& cornersB, Vector2 axis);
Vector2 projectPoints(std::array<Vector2, 4> points, Vector2 axis);

#endif