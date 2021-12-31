
#section SHADER

#stage vertex

#version 450

layout(push_constant) uniform Push
{
    mat4 mvp_matrix;
} push;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 offset;

void main()
{
    vec2 v = vec2(position.z, position.y);
    v.x += offset.z;
    v.y += offset.y;
    gl_Position =  push.mvp_matrix * vec4(0, v.y, v.x, 1);
}


#stage fragment

#version 450

layout(location = 0) out vec3 color;

void main() {
    color = vec3(1, 1, 1);
}

