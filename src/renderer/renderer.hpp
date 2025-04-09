#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <color.hpp>
#include <memory>

#include "window_spec.hpp"
#include "../app.hpp"

namespace cobra {
    class renderer {
        static void error_callback(
            int error,
            const char *description
        );

        static void framebuffer_size_callback(
            GLFWwindow *window,
            int width,
            int height
        );

        static void key_callback(
            GLFWwindow *window,
            int key,
            int scancode,
            int action,
            int mods
        );

        GLFWwindow *window;

        double delta_time;

        app *current_app;
        bool run;

    public:
        explicit renderer(const window_spec &spec = {});
        ~renderer();

        template <app_type app_class>
        void run_app() {
            app_class a { *this };

            double last_time = glfwGetTime();

            // TODO: figure out how to not use a pointer here!!!
            this->current_app = &a;

            this->run = true;
            while (run) {
                const double current_time = glfwGetTime();
                this->delta_time = current_time - last_time;

                if (a.update(*this, delta_time) == app_status::BREAK)
                    run = false;

                glfwSwapBuffers(this->window);
                glfwPollEvents();

                if (glfwWindowShouldClose(this->window))
                    run = false;

                last_time = current_time;
            }

            this->current_app = nullptr;
            this->run = false;

        }

        void clear(GLbitfield mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT) const;
        void clear_color(color color) const;

        GLFWwindow *get_window() const;

        [[nodiscard]] int get_window_width() const;
        [[nodiscard]] int get_window_height() const;
    };

}
