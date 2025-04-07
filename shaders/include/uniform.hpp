#pragma once

#include <string>
#include <utility>
#include <stdexcept>
#include <glm/glm.hpp>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

template<typename T>
class uniform {
private:
    std::string name;
    GLuint *parent_program;

public:
    void operator=(const T &uniform) const {
        GLint location = glGetUniformLocation(*this->parent_program, this->name.c_str());
        if (location == -1) {
            throw std::runtime_error("Uniform location not found: " + this->name);
        }

        if constexpr (std::is_same_v<T, glm::mat4>) {
            glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(uniform));
        } else if constexpr (std::is_same_v<T, glm::vec3>) {
            glUniform3fv(location, glm::value_ptr(uniform));
        } else if constexpr (std::is_same_v<T, glm::vec2>) {
            glUniform2fv(location, glm::value_ptr(uniform));
        } else if constexpr (std::is_arithmetic_v<T>) {
            glUniform1f(location, static_cast<GLfloat>(uniform));
        } else {
            throw std::runtime_error("Invalid uniform type!");
        }
    }

    uniform(std::string name, GLuint *parent_program) : name(std::move(name)), parent_program(parent_program) {}
};
