import re
import os


def embed(name, vertex_shader_code, fragment_shader_code):
    # Safety check shader name
    forbidden_names = ["main", "default"]

    for forbidden_name in forbidden_names:
        if forbidden_name == name:
            print("Shader has forbidden name")
            raise SystemError

    # Extract uniform variables
    uniform_pattern = re.compile(r'uniform\s+(mat4|float|int|vec[234])\s+(\w+);')
    uniforms = uniform_pattern.findall(vertex_shader_code)

    # Generate the C++ class header using raw strings for shader sources
    header = f"""
#pragma once

#include <glm/glm.hpp>

#include "../Shader.hpp"

class {name}_class : public shader {{
protected:

public:
    {name}_class() : shader(R"(\n{vertex_shader_code})", R"(\n{fragment_shader_code})") {{}}
    """

    # Add Uniforms to the C++ class
    for uniform_type, uniform_name in uniforms:
        glm_type = {
            'mat4': 'glm::mat4',
            'float': 'float',
            'int': 'int',
            'vec2': 'glm::vec2',
            'vec3': 'glm::vec3',
            'vec4': 'glm::vec4'
        }[uniform_type]
        header += f'\n\tuniform<{glm_type}> {uniform_name} = uniform<{glm_type}>("{uniform_name}", &this->shader_program);'

    header += "\n};\n"

    header += f"\n[[maybe_unused]] static {name}_class {name} = {name}_class();\n"

    return header


def embed_all(directory):
    files = [f for f in os.listdir(directory) if os.path.isfile(os.path.join(directory, f))]

    # Group .vert and .frag files based on the name
    shader_groups = {}
    for file in files:
        name, ext = os.path.splitext(file)
        if ext in ['.vert', '.frag']:
            if name not in shader_groups:
                shader_groups[name] = {}
            shader_groups[name][ext] = os.path.join(directory, file)

    # Run the embed function with the respective content
    for name, paths in shader_groups.items():
        vertex_shader_code = ""
        fragment_shader_code = ""

        if '.vert' in paths:
            with open(paths['.vert'], 'r') as file:
                vertex_shader_code = file.read()

        if '.frag' in paths:
            with open(paths['.frag'], 'r') as file:
                fragment_shader_code = file.read()

        hcs_file = open(directory + "/include/shaders/{file_name}.hcs".format(file_name=name), "w")
        hcs_file.truncate(0)
        hcs_file.write(embed(name, vertex_shader_code, fragment_shader_code))


embed_all("./shaders")
