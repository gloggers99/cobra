#pragma once

#define COBRA_GL_COLOR_CONVERT(color) color.r, color.g, color.b, color.a

namespace cobra {
    class color {
    public:
        float r, g, b, a;

        explicit color(const float scalar) : r(scalar), g(scalar), b(scalar), a(1.0f) {}
        color(const float scalar, const float alpha) : r(scalar), g(scalar), b(scalar), a(alpha) {}
        color(const float r, const float g, const float b) : r(r), g(g), b(b), a(1.0f) {}
        color(const float r, const float g, const float b, const float a) : r(r), g(g), b(b), a(a) {}
    };
}