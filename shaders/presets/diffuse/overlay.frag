/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: overlay.frag is a part of VulkanRenderer

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

layout(
	input_attachment_index = INPUT_ATTACHMENT_BINDING0, 
	set = SUB_RENDER_SET, 
	binding = INPUT_ATTACHMENT_BINDING0
	) uniform subpassInput previousColor;

layout(location = 0) out vec4 color;

void main()
{
	// 0.2126 R + 0.7152 G + 0.0722 B
	float luminosity = dot(subpassLoad(previousColor).rgba, vec4(0.2126, 0.7152, 0.0722, 0));
	color = vec4(luminosity, luminosity, luminosity, 1);
}

