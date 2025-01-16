#version 410 core

uniform vec2 window_size;

layout (location = 0) in vec3 pos;

void main() {
    gl_Position = vec4(window_size*pos.xy, 1.0, 1.0);
}
