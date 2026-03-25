// myheader.h
#include "raylib.h"
#include <array>

using std::array;

#ifndef COLLISION_H
#define COLLISION_H

array<Vector2, 4> getCorners(Rectangle rect, float rotation);

bool isOverlap(Rectangle rectA, float rotA, Rectangle rectB, float rotB);

bool isOverlapOnAxis(const array<Vector2, 4>& cornersA, const array<Vector2, 4>& cornersB, Vector2 axis);

Vector2 projectPoints(array<Vector2, 4> points, Vector2 axis);

#endif