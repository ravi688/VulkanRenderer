/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: color.frag is a part of VulkanRenderer

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

layout(SGE_UNIFORM_BUFFER_LAYOUT, set = GLOBAL_SET, binding = LIGHT_BINDING) uniform DirectionalLight lightInfo;

layout(SGE_UNIFORM_BUFFER_LAYOUT, set = MATERIAL_SET, binding = MATERIAL_PROPERTIES_BINDING) uniform Parameters
{
	vec4 color;
} parameters;


layout(location = 0) in vec3 _normal;

layout(location = 0) out vec4 color;

void main()
{
	float dp = 0.5 * dot(-_normal, lightInfo.direction) + 0.5;
	color = parameters.color * dp * vec4(lightInfo.color, 1);
}
