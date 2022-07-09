
#version 450

#include <v3d.h>

layout(set = GLOBAL_SET, binding = LIGHT_BINDING) uniform SpotLight light;
layout(set = MATERIAL_SET, binding = MATERIAL_PROPERTIES_BINDING) uniform Parameters
{
	vec4 color;
} parameters;
layout(set = RENDER_SET, binding = TEXTURE_BINDING0) uniform sampler2D shadowMap;

layout(location = 0) in vec3 normal;
layout(location = 1) in vec3 position;

layout(location = 0) out vec4 color;

const float shadowBias = 0.0006f;
const float minShadow = 0.001f;
const float ambient = 0.02f;
const float bias = 0.001f;

void main()
{
	vec4 shadowMapCoord = light.projection * light.view * vec4(position, 1.0);
	shadowMapCoord /= shadowMapCoord.w;
	shadowMapCoord = shadowMapCoord * 0.5 + 0.5;
	float closestDistance = texture(shadowMap, vec2(shadowMapCoord.x, 1 - shadowMapCoord.y)).r;

	vec3 dir = position - light.position;
	float distance = length(dir);

	float receivedShadow = 1.0f;

	if(distance > ((closestDistance * (20.0 - 0.04)) + 0.001))
		receivedShadow = minShadow;

	vec3 normalized_dir = dir / distance;
	float litAmount = 1;
	if(cos(light.angle * 0.5) < dot(light.direction, normalized_dir))
		litAmount = light.intensity / (distance * distance);
	else
		litAmount = 0;

	float dp = dot(normal, -normalized_dir);
	float castedShadow = max(0, dp) + bias;
	color = max(receivedShadow * castedShadow * litAmount, ambient) * vec4(light.color, 1) * parameters.color;
}
