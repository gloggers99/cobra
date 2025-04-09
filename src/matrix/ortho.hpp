#pragma once

#include "matrix_interface.hpp"

namespace cobra {
    class renderer;

    class ortho final : public matrix_interface {
        glm::mat4 internal_matrix;
    public:
        explicit ortho(const renderer &renderer);
        ~ortho() override;

        glm::mat4 update() override;
    };
}