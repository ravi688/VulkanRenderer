
#version 450

#include <v3d.h>

layout(set = GLOBAL_SET, binding = LIGHT_BINDING) uniform DirectionalLight lightInfo;

layout(set = MATERIAL_SET, binding = MATERIAL_PROPERTIES_BINDING) uniform Parameters
{
	vec4 color;
} parameters;


layout(location = 0) in vec3 _normal;

layout(location = 0) out vec4 color;

void main()
{
	float dp = 0.5 * dot(-_normal, lightInfo.direction) + 0.5;
	color = parameters.color * dp * vec4(lightInfo.color, 1);
}
