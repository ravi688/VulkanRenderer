
#version 450

layout(push_constant) uniform Push
{
    mat4 mvp_matrix;
} push;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 normal;

void main()
{
    vec4 v = push.mvp_matrix * vec4(inPosition, 1.0);
    gl_Position = v;
    fragColor = inColor;
    normal = inNormal;
}