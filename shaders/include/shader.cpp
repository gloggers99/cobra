#include "shader.hpp"

#include <stdexcept>
#include <format>
#include <utility>

shader::shader(std::string vertex_shader_source,
               std::string fragment_shader_source) :
               vertex_shader_source(std::move(vertex_shader_source)),
               fragment_shader_source(std::move(fragment_shader_source)),
               compiled(false) {}

void shader::compile(shader &shader) {
    const char *vertexSource = shader.vertex_shader_source.c_str();
    shader.vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(shader.vertex_shader, 1, &vertexSource, nullptr);
    glCompileShader(shader.vertex_shader);

    const char *fragmentSource = shader.fragment_shader_source.c_str();
    shader.fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(shader.fragment_shader, 1, &fragmentSource, nullptr);
    glCompileShader(shader.fragment_shader);

    shader.shader_program = glCreateProgram();
    glAttachShader(shader.shader_program, shader.vertex_shader);
    glAttachShader(shader.shader_program, shader.fragment_shader);
    glLinkProgram(shader.shader_program);

    int success;
    char infoLog[512];
    glGetProgramiv(shader.shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader.shader_program, 512, nullptr, infoLog);
        throw std::runtime_error(std::format("\n{}", infoLog));
    }

    shader.compiled = true;
}

void shader::use(shader &shader) {
    if (shader.compiled)
        glUseProgram(shader.shader_program);
}