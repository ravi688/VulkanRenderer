#version 450

#include <v3d.h>
                
layout(set = MATERIAL_SET, binding = TEXTURE_BINDING0) uniform sampler2D bga;

layout(location = 0) in vec2 in_texcoord;
layout(location = 1) in vec3 in_color;

layout(location = 0) out vec4 color;

void main()
{
    vec2 texcoord = vec2(in_texcoord.x, in_texcoord.y);
    color = vec4(in_color.x, in_color.y, in_color.z, 1.0) * texture(bga, texcoord).r;
}
