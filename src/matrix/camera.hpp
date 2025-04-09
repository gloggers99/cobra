#pragma once

#include <unordered_map>

#include "uniform.hpp"
#include "matrix_interface.hpp"

#include <glm/glm.hpp>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

namespace cobra {
    enum class key_event;
}

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

        std::unordered_map<int, direction> key_map;

        [[nodiscard]] glm::mat4 create_view_matrix() const;
        [[nodiscard]] glm::mat4 create_projection_matrix() const;

        /**
         * Combination of both Camera::createViewMatrix() and Camera::createProjectionMatrix()
         */
        [[nodiscard]] glm::mat4 create_camera_matrix() const;

    public:
        void move(direction direction, float speed);

        /**
         * Call to handle the mouse and update the underlying matrix
         */
        glm::mat4 update() override;

        /**
         * Needs to be called on a frame update loop to handle camera input
         * if necessary
         */
        void input_hook(double speed);

        /**
         * Create an instance of a camera
         * @param renderer Reference to current renderer
         * @param key_map Optional key map to move the camera around with
         */
        explicit camera(
            cobra::renderer &renderer,
            const std::unordered_map<int, direction> &key_map = {}
        );
        ~camera() override;
    };

}
