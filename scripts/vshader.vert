#version 460 core

layout (location = 0) in vec3 vertPos;

// 1. Test this later!
// vec3 pos;

void main()
{
    // 1. Test this later!
    // 
    // pos = vertPos;
    // gl_Position = vec4(pos, 1.0);

    gl_Position = vec4(vertPos, 1.0);
}