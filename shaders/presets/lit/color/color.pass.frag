/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: color.pass.frag is a part of VulkanRenderer

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

layout(set = GLOBAL_SET, binding = LIGHT_BINDING) uniform DirectionalLight lightInfo;
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
	float shadowedDepth = _lightClipPos.z / _lightClipPos.w;
	vec2 shadowMapCoord = vec2(_lightClipPos.x / _lightClipPos.w, -_lightClipPos.y / _lightClipPos.w);
	shadowMapCoord = shadowMapCoord * 0.5 + 0.5;
	float depth = texture(shadowMap, shadowMapCoord).r;
	if((shadowedDepth - depth) > 0.001)
		color = parameters.color * 0.01;
	else
	{
		float dp = dot(-_normal, lightInfo.direction) * 0.5 + 0.5;
		color = parameters.color * dp * vec4(lightInfo.color, 1);
	}
}
