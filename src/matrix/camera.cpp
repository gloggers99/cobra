#include <stdexcept>

#include "camera.hpp"
#include "../renderer/renderer.hpp"

#include <glm/ext.hpp>

namespace cobra {
    glm::mat4 camera::create_view_matrix() {
        return glm::lookAt(this->camera_position, this->camera_position + this->camera_front, this->camera_up);
    }

    glm::mat4 camera::create_projection_matrix() {
        int width, height;
        glfwGetWindowSize(renderer.get_window(), &width, &height);

        return glm::perspective(glm::radians(this->field_of_view), static_cast<float>(width) / static_cast<float>(height), 0.1f, 1000.0f);
    }

    glm::mat4 camera::create_camera_matrix() {
        return this->create_projection_matrix() * this->create_view_matrix();
    }

    void camera::move(const direction direction, const float speed) {
        switch (direction) {
            case direction::FORWARD:
                this->camera_position += camera_front * speed;
                break;
            case direction::BACKWARD:
                this->camera_position -= camera_front * speed;
                break;
            case direction::LEFT:
                this->camera_position -= glm::normalize(glm::cross(this->camera_front, this->camera_up)) * speed;
                break;
            case direction::RIGHT:
                this->camera_position += glm::normalize(glm::cross(this->camera_front, this->camera_up)) * speed;
                break;
            default:
                throw std::runtime_error("Invalid direction provided.");
        }
    }

    glm::mat4 camera::update(){
        double xpos;
        double ypos;
        glfwGetCursorPos(this->renderer.get_window(), &xpos, &ypos);

        if (first_mouse)
        {
            last_x = xpos;
            last_y = ypos;
            first_mouse = false;
        }

        float xoffset = xpos - last_x;
        float yoffset = last_y - ypos;
        last_x = xpos;
        last_y = ypos;

        xoffset *= mouse_sensitivity;
        yoffset *= mouse_sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        glm::vec3 front;
        front.x = std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch));
        front.y = std::sin(glm::radians(pitch));
        front.z = std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch));
        camera_front = glm::normalize(front);

        return this->create_camera_matrix();
    }

    camera::camera(cobra::renderer &renderer)
        : renderer(renderer),

          camera_position(glm::vec3(0.0f, 0.0f, 0.0f)),
          camera_front(glm::vec3(0.0f, 0.0f, -1.0f)),
          camera_up(glm::vec3(0.0f, 1.0f, 0.0f)),

          field_of_view(90.0f),

          first_mouse(true),
          yaw(-90.0f),
          pitch(0.0f),

          mouse_sensitivity(0.5f) {
        int width, height;
        glfwGetWindowSize(renderer.get_window(), &width, &height);

        this->last_x = static_cast<float>(width) / 2.0f;
        this->last_y = static_cast<float>(height) / 2.0f;
    }

    camera::~camera() = default;
}
