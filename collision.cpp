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

bool isOverlap(Rectangle rectA, float rotA, Rectangle rectB, float rotB) {
    array<Vector2, 4> cornersA = getCorners(rectA, rotA);
    array<Vector2, 4> cornersB = getCorners(rectB, rotB);

    vector<Vector2> axes = {
        Vector2Normalize(Vector2Subtract(cornersA[1], cornersA[0])),
        Vector2Normalize(Vector2Subtract(cornersA[2], cornersA[1])),
        Vector2Normalize(Vector2Subtract(cornersB[1], cornersB[0])),
        Vector2Normalize(Vector2Subtract(cornersB[2], cornersB[1]))
    };

    for (const auto& axis : axes) {
        if (!isOverlapOnAxis(cornersA, cornersB, axis)) {
            return false;
        }
    }

    return true;
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