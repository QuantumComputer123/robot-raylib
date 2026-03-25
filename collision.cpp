#include <array>
#include <vector>
#include "collision.h"
#include "raylib.h"
#include "raymath.h"

using std::array;
using std::vector;

array<Vector2, 4> getCorners(Rectangle rect, float rotation) {
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
        corners[i].x = rect.x + (offsets[i].x * cosA - offsets[i].y * sinA);
        corners[i].y = rect.y + (offsets[i].x * sinA + offsets[i].y * cosA);
    }
    return corners;
}

CollisionResult getCollision(Rectangle rectA, float rotA, Rectangle rectB, float rotB) {
    auto cornersA = getCorners(rectA, rotA);
    auto cornersB = getCorners(rectB, rotB);
    
    float minOverlap = INFINITY;
    Vector2 smallestAxis = {0, 0};

    // Standard SAT axes (normals of the sides)
    vector<Vector2> axes = {
        Vector2Normalize({-(cornersA[1].y - cornersA[0].y), cornersA[1].x - cornersA[0].x}),
        Vector2Normalize({-(cornersA[2].y - cornersA[1].y), cornersA[2].x - cornersA[1].x}),
        Vector2Normalize({-(cornersB[1].y - cornersB[0].y), cornersB[1].x - cornersB[0].x}),
        Vector2Normalize({-(cornersB[2].y - cornersB[1].y), cornersB[2].x - cornersB[1].x})
    };

    for (Vector2 axis : axes) {
        Vector2 rangeA = projectPoints(cornersA, axis);
        Vector2 rangeB = projectPoints(cornersB, axis);

        float overlap = fmin(rangeA.y, rangeB.y) - fmax(rangeA.x, rangeB.x);
        if (overlap < 0) return {false}; // Gap found!

        if (overlap < minOverlap) {
            minOverlap = overlap;
            smallestAxis = axis;
        }
    }

    // Ensure the normal points from B to A (so we push A out)
    Vector2 dir = Vector2Subtract({rectA.x, rectA.y}, {rectB.x, rectB.y});
    if (Vector2DotProduct(dir, smallestAxis) < 0) {
        smallestAxis = Vector2Scale(smallestAxis, -1);
    }

    return {true, smallestAxis, minOverlap};
}


bool isOverlapOnAxis(const array<Vector2, 4>& cornersA, const array<Vector2, 4>& cornersB, Vector2 axis) {
    Vector2 rangeA = projectPoints(cornersA, axis);
    Vector2 rangeB = projectPoints(cornersB, axis);

    if (rangeA.y < rangeB.x || rangeB.y < rangeA.x) {
        return false; 
    }

    return true;
}

Vector2 projectPoints(array<Vector2, 4> points, Vector2 axis) {
    float minVal = INFINITY;
    float maxVal = -INFINITY;

    for (auto point : points) {
        float dotProduct = Vector2DotProduct(point, axis);

        if (dotProduct < minVal) minVal = dotProduct;
        if (dotProduct > maxVal) maxVal = dotProduct;
    }

    return (Vector2){minVal, maxVal};
}