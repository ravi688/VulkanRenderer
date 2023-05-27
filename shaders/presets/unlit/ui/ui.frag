/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: ui.frag is a part of VulkanRenderer

	Copyright (C) 2023  Author: Ravi Prakash Singh

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

