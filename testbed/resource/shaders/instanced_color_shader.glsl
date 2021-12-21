
#stage vertex

#version 450

layout(push_constant) uniform Push
{
    mat4 mvp_matrix;
} push;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 instanceOffset;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out flat uint instanceIndex;

void main()
{
    vec3 position = inPosition;
    position += vec3(instanceOffset, 0);
    vec4 v = push.mvp_matrix * vec4(position, 1.0);
    instanceIndex = gl_InstanceIndex;
    gl_Position = v;
    fragColor = inColor;
}


#stage fragment

#version 450

layout(location = 0) out vec3 color;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in flat uint instanceIndex;

void main() {
    color = vec3(instanceIndex / 2, 1, instanceIndex);
}

