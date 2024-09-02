/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: spot.frag is a part of VulkanRenderer

	Copyright (C) 2021 - 2024  Author: Ravi Prakash Singh

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>. 
*/


#version 450

#define USE_SPOT_LIGHTS
#include <v3d.h>

layout(SGE_UNIFORM_BUFFER_LAYOUT, set = MATERIAL_SET, binding = MATERIAL_PROPERTIES_BINDING) uniform Parameters
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
	SpotLightType light = spotLights.lights[0];
	vec4 shadowMapCoord = light.proj * light.view * vec4(position, 1.0);
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
