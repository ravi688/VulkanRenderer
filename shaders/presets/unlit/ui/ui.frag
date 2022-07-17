
#version 450

#include <v3d.h>

MATERIAL_PROPERTIES
{
	vec4 color;
} parameters;

layout(set = MATERIAL_SET, binding = TEXTURE_BINDING0) uniform sampler2D albedo;

layout(location = 0) in vec2 texcoord;

layout(location = 0) out vec4 color;

float linearize(float depth, float n, float f)
{
	float t = ((n - f) * depth + f) * (f - n);
	return n * f * 1 / t;
}

void main()
{
	vec2 uv = vec2(texcoord.x, 1 - texcoord.y);
	float depth = texture(albedo, uv).r;
	float linear_depth = linearize(depth, 0.04, 20.0);
	color = parameters.color * vec4(linear_depth, linear_depth, linear_depth, 1.0);
}

