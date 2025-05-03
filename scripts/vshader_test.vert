#version 460 core

layout (location = 0) in vec3 vertPos;

vec3 pos;

// Not allowed:
//
// const vec4 fn1(vec3 pos)
// highp layout(location = 1) invariant vec4 fn1(vec3 pos) // also complains about the "layout(location = 1)" part

// Allowed:
//
// highp vec4 fn1(vec3 pos)
// highp layout(location = 1) vec4 fn1(vec3 pos)
// highp layout(location = 1) flat vec4 fn1(vec3 pos)
// highp layout(location = 1) smooth vec4 fn1(vec3 pos)
// precise vec4 fn1(vec3 pos)
// precise highp vec4 fn1(vec3 pos)
// highp precise vec4 fn1(vec3 pos)

vec4 fn1(vec3 pos);

vec4 fn1(vec3 pos)
{
    return vec4(pos, 1.0);
}

// precise highp what_is_this;
// precise highp;

struct S
{
    int a;
    float b;
    vec2 v1;
};

S fn2()
{
    S s;
    s.a = 5;
    s.b = 3.4;
    s.v1 = vec2(1.0, 2.0);
    return s;
}

void main()
{
    // pos = vertPos;
    // gl_Position = vec4(pos, 1.0);

    S s = fn2();
    vec4 res = vec4(s.v1, 3.0, 1.0);
    gl_Position = fn1(pos);
}