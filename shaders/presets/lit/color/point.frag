/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: point.frag is a part of VulkanRenderer

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

#define USE_POINT_LIGHTS
#include <v3d.h>

layout(SGE_UNIFORM_BUFFER_LAYOUT, set = MATERIAL_SET, binding = MATERIAL_PROPERTIES_BINDING) uniform Parameters
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

float max(float x1, float x2, float x3, float x4, float x5, float x6)
{
	return max(max(max(max(max(x1, x2), x3), x4), x5), x6);
}

const float castedBias = 0.01f;

void main()
{
	PointLightType light = pointLights.lights[0];
	
	vec3 dir = light.position - position;

	float closestDepth = texture(shadowMap, -vec3(dir.z, dir.y, dir.x)).r;
	closestDepth = linearize(closestDepth, 0.04, 20.0) * (20.0 - 0.04) + 0.04;
	float depth = max(dir.x, dir.y, dir.z, -dir.x, -dir.y, -dir.z);
	float receivedShadow = 1.0f;
	if(closestDepth < (depth + shadowBias))
		receivedShadow = minShadow;

	float sqrDistance = dot(dir, dir);
	float litAmount = light.intensity / sqrDistance;
	float dp = dot(normal, normalize(dir));
	float castedShadow = max(0.0, dp) + castedBias;
	vec4 _color = receivedShadow * litAmount * castedShadow * vec4(light.color, 1) * parameters.color;
	color = vec4(_color.r, _color.g, _color.b, 1);
}
