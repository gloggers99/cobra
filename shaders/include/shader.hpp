#pragma once

#include "glad/gl.h"

#include <string>
#include <vector>

#include "uniform.hpp"

/**
 * Shader is the backbone of all shaders, uniforms and more can be updated from here.
 */
class shader {
protected:
    GLuint vertex_shader, fragment_shader, shader_program;

    std::string vertex_shader_source;
    std::string fragment_shader_source;

    bool compiled;

    /**
     * Construct the shader using a vertex source string and a fragment source string
     *
     * @param vertex_shader_source Vertex Shader Source
     * @param fragment_shader_source Fragment Shader Source
     */
    shader(std::string vertex_shader_source, std::string fragment_shader_source);
public:
    /**
     * Compile the given shader
     *
     * @param shader The shader to compile
     */
    static void compile(shader &shader);
    static void use(shader &shader);

};