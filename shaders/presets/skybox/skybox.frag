
#version 450

#include <v3d.h>

layout(set = MATERIAL_SET, binding = TEXTURE_BINDING0) uniform samplerCube skybox;

layout(location = 0) in vec3 uvw;

layout(location = 0) out vec4 color;

void main()
{
    color = texture(skybox, uvw).rgba;
}
