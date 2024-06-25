/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: text_mesh.vert is a part of VulkanRenderer

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

layout(SGE_UNIFORM_BUFFER_LAYOUT, set = SCENE_SET, binding = SCREEN_BINDING) uniform DisplayInfo
{
    uvec2 resolution;
    uvec2 dpi;
    uvec2 window_size;
    mat4 matrix;
} displayInfo;

layout(SGE_UNIFORM_BUFFER_LAYOUT, set = CAMERA_SET, binding = CAMERA_PROPERTIES_BINDING) uniform CameraInfo cameraInfo;
layout(SGE_UNIFORM_BUFFER_LAYOUT, set = OBJECT_SET, binding = TRANSFORM_BINDING) uniform ObjectInfo objectInfo;

struct Color
{
    float r;
    float g;
    float b;
};

layout(SGE_UNIFORM_BUFFER_LAYOUT, set = MATERIAL_SET, binding = MATERIAL_PROPERTIES_BINDING) uniform Parameters
{
    Color color;
    int space_type;
    int surface_type;
} parameters;

layout(SGE_UNIFORM_BUFFER_LAYOUT, set = MATERIAL_SET, binding = TEXTURE_BINDING2) uniform TSTBuffer
{
    mat4[128] tst_buffer;
};

layout(location = POSITION_LOCATION) in vec3 position;

layout(location = 5) in vec4 ofst_stid;

layout(location = 0) out vec3 out_color;

mat4 mat4_diagonal(float x, float y, float z, float w)
{
    mat4 m = mat4(vec4(0, 0, 0, 0), vec4(0, 0, 0, 0), vec4(0, 0, 0, 0), vec4(0, 0, 0, 0));
    m[0][0] = x;
    m[1][1] = y;
    m[2][2] = z;
    m[3][3] = w;
    return m;
}

mat4 mat4_identity()
{
    return mat4_diagonal(1.0, 1.0, 1.0, 1.0);
}

struct StringTransform
{
    mat4 matrix;
    uint point_size;
};

StringTransform unpack_tst(mat4 tst)
{
    StringTransform trans;
    trans.matrix = tst;
    trans.matrix[3][3] = 1.0;
    trans.point_size = floatBitsToUint(tst[3][3]);
    return trans;
}

void main()
{
    vec3 offset = ofst_stid.xyz;
    uint stid = floatBitsToUint(ofst_stid.w);
    vec3 pos = position + offset;

    StringTransform str_trans = unpack_tst(tst_buffer[stid]);

    vec4 localPos = vec4(pos.x, pos.y, pos.z, 1.0);
    mat4 world = objectInfo.transform * str_trans.matrix;

    vec4 clipPos;
    switch(parameters.space_type)
    {
        case TEXT_RENDER_SPACE_TYPE_2D:
        {
            switch(parameters.surface_type)
            {
                case TEXT_RENDER_SURFACE_TYPE_CAMERA:
                {
                    mat4 scale = mat4_diagonal(str_trans.point_size, str_trans.point_size, str_trans.point_size, 1);
                    clipPos = cameraInfo.screen * world * scale * localPos;
                    break;
                }
                case TEXT_RENDER_SURFACE_TYPE_SCREEN:
                {
                    mat4 scale = mat4_diagonal(str_trans.point_size, str_trans.point_size, str_trans.point_size, 1);
                    clipPos = displayInfo.matrix * world * scale * localPos;
                    break;
                }
            }
            break;
        }
        case TEXT_RENDER_SPACE_TYPE_3D:
        {
            switch(parameters.surface_type)
            {
                case TEXT_RENDER_SURFACE_TYPE_CAMERA:
                {
                    mat4 scale = mat4_diagonal(0.3, 0.3, 0.3, 1);
                    clipPos = cameraInfo.projection * cameraInfo.view * world * scale * localPos;
                    break;
                }
                case TEXT_RENDER_SURFACE_TYPE_SCREEN:
                {
                    mat4 scale = mat4_diagonal(str_trans.point_size, str_trans.point_size, str_trans.point_size, 1);
                    clipPos = displayInfo.matrix * world * scale * localPos;
                    break;
                }
            }
            break;
        }
    }
    gl_Position = vec4(clipPos.x, -clipPos.y, clipPos.z, clipPos.w);
    out_color = vec3(parameters.color.r, parameters.color.g, parameters.color.b);
}
