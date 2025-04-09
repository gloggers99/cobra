#version 100

layout (position = 0) in vec2 position;
layout (position = 1) in vec2 texture_coords;

out vec2 frag_texture_coords;

uniform mat4 projection;

void main() {
    gl_Position = projection * vec4(position, 0.0, 1.0);
    frag_texture_coords = texture_coords;
}