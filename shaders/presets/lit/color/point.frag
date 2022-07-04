
#version 450

#include <v3d.h>

layout(set = GLOBAL_SET, binding = LIGHT_BINDING) uniform PointLight light;
layout(set = MATERIAL_SET, binding = MATERIAL_PROPERTIES_BINDING) uniform Parameters
{
	vec4 color;
} parameters;
// layout(set = MATERIAL_SET, binding = TEXTURE_BINDING0) uniform sampler2D albedo;
// layout(set = RENDER_SET, binding = TEXTURE_BINDING0) uniform sampler2D shadowMap;

layout(location = 0) in vec3 normal;
// layout(location = 1) in vec2 texcoord;
layout(location = 1) in vec3 position;

layout(location = 0) out vec4 color;


void main()
{
	vec3 dir = light.position - position;
	float sqrDistance = dot(dir, dir);

	float litAmount = light.intensity / sqrDistance;
	color = litAmount * vec4(light.color, 1) * parameters.color;// * texture(albedo, texcoord);
}
