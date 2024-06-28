/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: diffuse.frag is a part of VulkanRenderer

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

#include <v3d.h>

layout(SGE_UNIFORM_BUFFER_LAYOUT, set = GLOBAL_SET, binding = LIGHT_BINDING) uniform DirectionalLight light;
layout(SGE_UNIFORM_BUFFER_LAYOUT, set = MATERIAL_SET, binding = MATERIAL_PROPERTIES_BINDING) uniform Parameters
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
const float minShadow = 0.001f;
const float ambient = 0.02f;

void main()
{

	vec2 shadowMapCoord = vec2(lightClipPos.x / lightClipPos.w, -lightClipPos.y / lightClipPos.w);
	shadowMapCoord = shadowMapCoord * 0.5 + 0.5;
	float depth = texture(shadowMap, shadowMapCoord).r;
	float shadowedDepth = lightClipPos.z / lightClipPos.w;

	float shadow = 1.0f;
	if(shadowedDepth > (depth + shadowBias))
 		shadow = minShadow;

	float dp = dot(normal, -light.direction);
	float litAmount = max(0, dp);
	color = max(shadow * litAmount, ambient) * vec4(light.color, 1) * parameters.color * texture(albedo, texcoord);
}
