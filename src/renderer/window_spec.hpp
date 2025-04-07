#pragma once

#include <string>

#include <GLFW/glfw3.h>

namespace cobra {
    struct window_spec {
        std::string title = "cobra";
        int width = 640;
        int height = 480;
        bool fullscreen = false;
        GLFWwindow *share = nullptr;
    };
}
