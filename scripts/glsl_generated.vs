#version 460 core
layout (location = 0) in vec4 pos;
layout (location = 4) in vec2 uv;
layout (location = 0) out vec2 vertUv;
void main() {
    vertUv = uv;
    gl_Position = pos;
}
