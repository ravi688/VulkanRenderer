
#version 450

#include <v3d.h>

layout(set = GLOBAL_SET, binding = LIGHT_BINDING) uniform LightInfo lightInfo;
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

void main()
{
	vec2 shadowMapCoord = vec2(lightClipPos.x / lightClipPos.w, -lightClipPos.y / lightClipPos.w);
	shadowMapCoord = shadowMapCoord * 0.5 + 0.5;
	float shadowedDepth = lightClipPos.z / lightClipPos.w;
	float depth = texture(shadowMap, shadowMapCoord).r;

	if((shadowedDepth - depth) >= 0.004)
		color = vec4(lightInfo.color, 1) * parameters.color * texture(albedo, texcoord).rgba * 0.1;
	else
	{
		float dp = dot(-normal, lightInfo.dir) * 0.5 + 0.5;
		color = dp * vec4(lightInfo.color, 1) * parameters.color * texture(albedo, texcoord);
	}
}
