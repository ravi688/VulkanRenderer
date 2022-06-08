
#version 450

#include <v3d.h>

layout(set = GLOBAL_SET, binding = LIGHT_BINDING) uniform DirectionalLight light;
layout(set = MATERIAL_SET, binding = MATERIAL_PROPERTIES_BINDING) uniform Parameters
{
	vec4 color;
} parameters;
layout(set = MATERIAL_SET, binding = TEXTURE_BINDING0) uniform sampler2D albedo;
layout(set = RENDER_SET, binding = TEXTURE_BINDING0) uniform sampler2D shadowMap;

layout(location = 0) in vec3 normal;
layout(location = 1) in vec2 texcoord;
layout(location = 2) in vec4 lightClipPos;

layout(location = 0) out vec4 color;

const float shadowBias = 0.0006f;
const float minShadow = 0.1f;

float lerp(float min, float max, float t)
{
	return min * (1 - t) + max * t;
}

void main()
{
	float dp = dot(normal, -light.direction);

	float litAmount = dp * 0.5 + 0.5;
	vec4 diffuse = litAmount * vec4(light.color, 1) * parameters.color * texture(albedo, texcoord);


	vec2 shadowMapCoord = vec2(lightClipPos.x / lightClipPos.w, -lightClipPos.y / lightClipPos.w);
	shadowMapCoord = shadowMapCoord * 0.5 + 0.5;
	float depth = texture(shadowMap, shadowMapCoord).r;
	float shadowedDepth = lightClipPos.z / lightClipPos.w;

	float shadow = 1.0f;
	if(dp <= 0.1f)
		shadow = lerp(minShadow, 1.0, max(1, (1.0 + dp)));
	else if(shadowedDepth > (depth + shadowBias))
 		shadow = 0.1f;

	color = diffuse * shadow;

}
