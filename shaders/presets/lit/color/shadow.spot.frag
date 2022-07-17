
#version 450

#include <v3d.h>

layout(set = GLOBAL_SET, binding = LIGHT_BINDING) uniform SpotLight light;

layout(location = 0) in vec3 position;

void main()
{
 	gl_FragDepth = length(position - light.position) / (20.0 - 0.04);
}
