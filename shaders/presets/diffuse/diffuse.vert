/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: diffuse.vert is a part of VulkanRenderer

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

#define USE_FAR_LIGHTS
#include <v3d.h>

layout(SGE_UNIFORM_BUFFER_LAYOUT, set = CAMERA_SET, binding = CAMERA_PROPERTIES_BINDING) uniform CameraInfo cameraInfo;
layout(SGE_UNIFORM_BUFFER_LAYOUT, set = OBJECT_SET, binding = TRANSFORM_BINDING) uniform ObjectInfo objectInfo;

layout(location = POSITION_LOCATION) in vec3 position;
layout(location = NORMAL_LOCATION) in vec3 normal;
layout(location = TEXCOORD_LOCATION) in vec2 texcoord;

layout(location = 0) out vec3 _normal;
layout(location = 1) out vec2 _texcoord;

layout(location = 2) out vec4 _lightClipPos;

void main()
{
	vec4 clipPos = cameraInfo.projection * cameraInfo.view * objectInfo.transform * vec4(position, 1);
	clipPos.y = -clipPos.y;
	gl_Position = clipPos;
	_normal = normalize((objectInfo.normal * vec4(normal, 1)).xyz);
	_texcoord = texcoord;

	FarLightType light = farLights.lights[0];
	_lightClipPos = light.proj * light.view * objectInfo.transform * vec4(position, 1);
}
