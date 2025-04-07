#include "renderer.hpp"
#include "../shaders/include/color.hpp"

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#include <iostream>

namespace cobra {
    void renderer::error_callback(int error, const char *description) {
        fprintf(stderr, "GLFW error %i: %s\n", error, description);
    }

    void renderer::framebuffer_size_callback(GLFWwindow *window, int width, int height) {
        glViewport(0,  0, width, height);
    }

    void renderer::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
        auto *user_pointer_renderer = static_cast<renderer*>(glfwGetWindowUserPointer(window));

        if (user_pointer_renderer->current_app->keypress(
            *user_pointer_renderer,
            key,
            action == GLFW_PRESS ? key_event::PRESS : key_event::RELEASE) == app_status::BREAK)
            user_pointer_renderer->run = false;

    }

    renderer::renderer(const window_spec &spec) :
                       window(nullptr),
                       delta_time(0.0),
                       current_app(nullptr),
                       run(false) {
        if (glfwInit() != GLFW_TRUE)
            throw std::runtime_error("Failed to initialize GLFW!");

        glfwSetErrorCallback(renderer::error_callback);

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

        if (spec.fullscreen) {
            GLFWmonitor *primary_monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode *video_mode = glfwGetVideoMode(primary_monitor);

            this->window = glfwCreateWindow(video_mode->width, video_mode->height, spec.title.c_str(), primary_monitor, spec.share);
        }
        else {
            this->window = glfwCreateWindow(spec.width, spec.height, spec.title.c_str(), nullptr, spec.share);
        }

        if (!this->window)
            throw std::runtime_error("Failed to create a window!");

        glfwMakeContextCurrent(this->window);
        gladLoadGL(glfwGetProcAddress);

        glfwSetWindowUserPointer(this->window, this);

        glfwSetFramebufferSizeCallback(this->window, renderer::framebuffer_size_callback);
        glfwSetKeyCallback(this->window, renderer::key_callback);

        int x, y;
        glfwGetWindowSize(this->window, &x, &y);

        glViewport(0, 0, x, y);
    }

    renderer::~renderer() {
        glfwDestroyWindow(this->window);
        glfwTerminate();
    }

    void renderer::clear(const GLbitfield mask) const {
        glClear(mask);
    }

    void renderer::clear_color(const color color) const {
        glClearColor(COBRA_GL_COLOR_CONVERT(color));
    }



    GLFWwindow *renderer::get_window() {
        return this->window;
    }


}