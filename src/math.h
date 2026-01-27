#pragma once

struct Vec2 {
    float x, y;
};

struct Vec3 {
    float r, g, b;
};

inline Vec2 operator+(const Vec2 a, const Vec2 b) {
    return { a.x + b.x, a.y + b.y };
}

inline Vec2 operator-(const Vec2 a, const Vec2 b) {
    return { a.x - b.x, a.y - b.y };
}

inline Vec2 operator*(const Vec2 v, const float s) {
    return { v.x * s, v.y * s };
}