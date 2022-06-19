
#version 450

#include <v3d.h>

MATERIAL_PROPERTIES
{
	vec4 color;
} parameters;

layout(set = MATERIAL_SET, binding = TEXTURE_BINDING0) uniform sampler2D albedo;

layout(location = 0) in vec2 texcoord;

layout(location = 0) out vec4 color;

void main()
{
	color = parameters.color * texture(albedo, texcoord).rgba;
}

