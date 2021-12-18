
#stage vertex

#version 450

layout(push_constant) uniform Push
{
    mat4 mvp_matrix;
} push;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 normal;
layout(location = 2) out vec2 texCoord;
layout(location = 3) out flat uint instanceIndex;

void main()
{
    vec4 v = push.mvp_matrix * vec4(inPosition, 1.0);
    v.x -= 0.8 * gl_InstanceIndex;
    instanceIndex = gl_InstanceIndex;
    gl_Position = v;
    fragColor = inColor;
    normal = inNormal;
    texCoord = inTexCoord;
}


#stage fragment

#version 450

layout(location = 0) out vec3 color;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in flat uint instanceIndex;

void main() {
    color = vec3(instanceIndex / 2, 1, instanceIndex);
}

