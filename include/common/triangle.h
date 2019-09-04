#pragma once

#include "common/vector2.h"

struct Triangle
{
    Triangle(const Vector2 & x, const Vector2 & y, const Vector2 & z) : a(x), b(y), c(z) {}

    Vector2 a, b, c;
};
