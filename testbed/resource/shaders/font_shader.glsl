
#stage vertex

#version 450

layout(push_constant) uniform Push
{
    mat4 mvp_matrix;
} push;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 offset;

layout(location = 0) out vec3 fragColor;

void main()
{
    vec2 v = vec2(position.z, position.y);
    v.x += offset.z;
    v.y += offset.y;
    gl_Position =  push.mvp_matrix * vec4(0, v.y, v.x, 1);
    fragColor = color;
}


#stage fragment

#version 450

layout(location = 0) out vec3 color;

layout(location = 0) in vec3 fragColor;

void main() {
    color = fragColor;
}

