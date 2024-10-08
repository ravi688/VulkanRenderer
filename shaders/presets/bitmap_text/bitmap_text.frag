/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: bitmap_text.frag is a part of VulkanRenderer

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

#define REQUIRE_BITMAP_TEXT
#include <v3d.h>
                
layout(location = 0) in vec2 in_texcoord;
layout(location = 1) in vec3 in_color;

layout(location = 0) out vec4 color;

void main()
{
    vec2 texcoord = vec2(in_texcoord.x, in_texcoord.y);
    color = vec4(in_color.x, in_color.y, in_color.z, 1.0) * texture(bga, texcoord).r;
}
