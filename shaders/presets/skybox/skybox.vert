/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: skybox.vert is a part of VulkanRenderer

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

layout(set = CAMERA_SET, binding = CAMERA_PROPERTIES_BINDING) uniform CameraInfo cameraInfo;
layout(set = OBJECT_SET, binding = TRANSFORM_BINDING) uniform ObjectInfo objectInfo;

layout(location = POSITION_LOCATION) in vec3 position;

layout(location = 0) out vec3 _uvw;

void main()
{
    _uvw = position;
        	
    // In our renderer, x and z axis are swapped with each other
    _uvw = vec3(_uvw.z, -_uvw.y, _uvw.x);

    mat4 view = cameraInfo.view;
    view[3][0] = 0;
    view[3][1] = 0;
    view[3][2] = 0;
    view[3][3] = 1;

    vec4 clipPos = cameraInfo.projection * view * objectInfo.transform * vec4(position, 1);
    gl_Position = vec4(clipPos.x, -clipPos.y, clipPos.w, clipPos.w);
}
