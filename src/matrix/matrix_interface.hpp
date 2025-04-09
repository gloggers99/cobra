#pragma once

#include "uniform.hpp"

namespace cobra {
    class matrix_interface {
        glm::mat4 matrix;

    protected:
        /**
         * Update the matrix with new data, this function MUST return a valid glm::mat4
         */
        [[nodiscard]] virtual glm::mat4 update() = 0;

    public:
        virtual ~matrix_interface() = default;

        /**
         * Update a uniform with the corresponding matrix
         *
         * @param uniform The uniform to assign
         */
        void correlate(uniform<glm::mat4> &uniform) {
            this->matrix = this->update();
            uniform = matrix;
        }

        matrix_interface() : matrix(glm::mat4(1.0f)) {}
    };
}