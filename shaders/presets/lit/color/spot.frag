
#version 450

#include <v3d.h>

layout(set = GLOBAL_SET, binding = LIGHT_BINDING) uniform SpotLight light;
layout(set = MATERIAL_SET, binding = MATERIAL_PROPERTIES_BINDING) uniform Parameters
{
	vec4 color;
} parameters;
layout(set = RENDER_SET, binding = TEXTURE_BINDING0) uniform sampler2D shadowMap;

layout(location = 0) in vec3 normal;
layout(location = 1) in vec4 lightClipPos;
layout(location = 2) in vec3 position;

layout(location = 0) out vec4 color;

const float shadowBias = 0.0006f;
const float minShadow = 0.001f;
const float ambient = 0.02f;

void main()
{
	vec2 shadowMapCoord = vec2(lightClipPos.x / lightClipPos.w, -lightClipPos.y / lightClipPos.w);
	shadowMapCoord = shadowMapCoord * 0.5 + 0.5;
	float depth = texture(shadowMap, shadowMapCoord).r;
	float shadowedDepth = lightClipPos.z / lightClipPos.w;

	vec3 dir = position - light.position;
	float receivedShadow = 1.0f;

	vec3 normalized_dir = normalize(dir);
	float litAmount = 1;
	if(cos(light.angle * 0.5) < dot(light.direction, normalized_dir))
		litAmount = light.intensity / dot(dir, dir);
	else
		litAmount = 0;

	float dp = dot(normal, -normalized_dir);
	float castedShadow = max(0, dp);
	color = max(receivedShadow * castedShadow * litAmount, ambient) * vec4(light.color, 1) * parameters.color;
}
