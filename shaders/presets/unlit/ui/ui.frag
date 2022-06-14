
#version 450

#include <v3d.h>

MATERIAL_PROPERTIES
{
	vec4 color;
} parameters;


layout(location = 0) out vec4 color;

void main()
{
	color = parameters.color;
}

