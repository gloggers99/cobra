#pragma once

#include "uniform.hpp"
#include "matrix_interface.hpp"

#include <glm/glm.hpp>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

namespace cobra {
    class renderer;

    enum class direction {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT
    };

    class camera final : public matrix_interface {
        renderer &renderer;

        glm::vec3 camera_position;
        glm::vec3 camera_front;
        glm::vec3 camera_up;

        float field_of_view;

        bool first_mouse;
        float yaw;
        float pitch;

        float last_x;
        float last_y;

        float mouse_sensitivity;

        glm::mat4 create_view_matrix();
        glm::mat4 create_projection_matrix();

        /**
         * Combination of both Camera::createViewMatrix() and Camera::createProjectionMatrix()
         */
        glm::mat4 create_camera_matrix();

    public:
        void move(direction direction, float speed);

        /**
         * Call to handle the mouse and update the underlying matrix
         */
        glm::mat4 update() override;

        explicit camera(cobra::renderer &renderer);
        ~camera() override;
    };

}
