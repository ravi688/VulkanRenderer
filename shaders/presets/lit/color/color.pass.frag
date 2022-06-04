
#version 450

#include <v3d.h>

layout(set = GLOBAL_SET, binding = LIGHT_BINDING) uniform LightInfo lightInfo;
layout(set = MATERIAL_SET, binding = MATERIAL_PROPERTIES_BINDING) uniform Parameters
{
	vec4 color;
} parameters;
layout(set = RENDER_SET, binding = TEXTURE_BINDING0) uniform sampler2D shadowMap;


layout(location = 0) in vec4 _lightClipPos;
layout(location = 1) in vec3 _normal;

layout(location = 0) out vec4 color;

void main()
{
	float depth = _lightClipPos.z / _lightClipPos.w;
	vec2 shadowMapCoord = vec2(_lightClipPos.x / _lightClipPos.w, -_lightClipPos.y / _lightClipPos.w);
	shadowMapCoord = shadowMapCoord * 0.5 + 0.5;
	float shadowedDepth = texture(shadowMap, shadowMapCoord).r;
	if((depth - shadowedDepth) > 0.001)
		color = parameters.color * 0.01;
	else
	{
		float dp = dot(-_normal, lightInfo.dir) * 0.5 + 0.5;
		color = parameters.color * dp * vec4(lightInfo.color, 1);
	}
}
