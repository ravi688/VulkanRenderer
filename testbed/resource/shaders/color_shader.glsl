
#section SHADER

#stage vertex

#version 450

layout(push_constant) uniform Push
{
    mat4 mvp_matrix;
} push;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

void main()
{
    vec4 v = push.mvp_matrix * vec4(inPosition, 1.0);
    gl_Position = v;
    fragColor = inColor;
}


#stage fragment

#version 450

layout(location = 0) out vec3 color;

layout(location = 0) in vec3 fragColor;


void main() {
    color = fragColor;
}

