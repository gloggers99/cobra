#include "ortho.hpp"

#include "../renderer/renderer.hpp"

namespace cobra {
    ortho::ortho(const renderer &renderer)
        : internal_matrix(glm::ortho(0.0f,
                                     static_cast<float>(renderer.get_window_width()),
                                     0.0f,
                                     static_cast<float>(renderer.get_window_height()))) {}

    ortho::~ortho() = default;

    glm::mat4 ortho::update() {
        return this->internal_matrix;
    }
}
