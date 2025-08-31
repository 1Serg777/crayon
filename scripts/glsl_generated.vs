#version 460 core
layout (location = 0) in float pos;
layout (location = 4) in float uv;
layout (location = 0) out vec2 vertUv;
void main() {
    vertUv = uv;
    gl_Position = pos;
}
