#version 460

in vec2 frag_texture_coords;

out vec4 output_color;

uniform sampler2D text_texture;
uniform vec4 text_color;

void main() {
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text_texture, frag_texture_coords).r);
    output_color = text_color * sampled;
}