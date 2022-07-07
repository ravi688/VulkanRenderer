
#version 450

#include <v3d.h>

layout(set = GLOBAL_SET, binding = LIGHT_BINDING) uniform PointLight light;
layout(set = MATERIAL_SET, binding = MATERIAL_PROPERTIES_BINDING) uniform Parameters
{
	vec4 color;
} parameters;

// TODO: set must be RENDER_SET
layout(set = MATERIAL_SET, binding = TEXTURE_BINDING0) uniform samplerCube shadowMap;

layout(location = 0) in vec3 normal;
layout(location = 1) in vec3 position;

layout(location = 0) out vec4 color;


const float shadowBias = 0.0006f;
const float minShadow = 0.2;

float linearize(float depth, float n, float f)
{
	float t = ((n - f) * depth + f) * (f - n);
	return n * f * 1 / t;
}

float lerp(float min, float max, float t)
{
	return min * (1 - t) + max * t;
}

float max(float x1, float x2, float x3, float x4, float x5, float x6)
{
	return max(max(max(max(max(x1, x2), x3), x4), x5), x6);
}

const float castedBias = 0.01f;

void main()
{
	vec3 dir = light.position - position;
	float dp = dot(normal, normalize(dir));

	float closestDepth = texture(shadowMap, -vec3(dir.z, dir.y, dir.x)).r;
	closestDepth = linearize(closestDepth, 0.04, 20.0) * (20.0 - 0.04) + 0.04;
	float depth = max(dir.x, dir.y, dir.z, -dir.x, -dir.y, -dir.z);
	float receivedShadow = 1.0f;
	if(dp <= 0.05f)
		receivedShadow = lerp(minShadow, 1.0, max(1.0, 1.0 + dp));	
	else if(closestDepth < (depth + shadowBias))
		receivedShadow = minShadow;

	float sqrDistance = dot(dir, dir);
	float litAmount = light.intensity / sqrDistance;
	float castedShadow = max(0.0, dp) + castedBias;
	vec4 _color = receivedShadow * litAmount * castedShadow * vec4(light.color, 1) * parameters.color;
	color = vec4(_color.r, _color.g, _color.b, 1);
}
