
#version 450

#include <v3d.h>

layout(set = MATERIAL_SET, binding = MATERIAL_PROPERTIES_BINDING) uniform Parameters
{
	vec4 color;
} parameters;

layout(location = 0) out vec4 color;

void main()
{
	color = parameters.color;
}
