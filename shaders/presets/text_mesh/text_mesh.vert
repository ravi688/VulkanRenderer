#version 450

#extension GL_EXT_scalar_block_layout : enable

#include <v3d.h>

layout(set = GLOBAL_SET, binding = SCREEN_BINDING) uniform DisplayInfo
{
    uvec2 resolution;
    uvec2 dpi;
    uvec2 window_size;
    mat4 matrix;
} displayInfo;

layout(set = CAMERA_SET, binding = CAMERA_PROPERTIES_BINDING) uniform CameraInfo cameraInfo;
layout(set = OBJECT_SET, binding = TRANSFORM_BINDING) uniform ObjectInfo objectInfo;

struct Color
{
    float r;
    float g;
    float b;
};

layout(std430, set = MATERIAL_SET, binding = MATERIAL_PROPERTIES_BINDING) uniform Parameters
{
    uvec2 tex_size;
    Color color;
    int space_type;
    int surface_type;
} parameters;

layout(std430, set = MATERIAL_SET, binding = TEXTURE_BINDING2) uniform TSTBuffer
{
    mat4[128] tst_buffer;
};

layout(location = POSITION_LOCATION) in vec3 position;

layout(location = 5) in vec4 ofst_stid;

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
                case TEXT_RENDER_SURFACE_TYPE_SCREEN:
                {
                    mat4 scale = mat4_identity(); //mat4_diagonal(100, 100, 100, 1);
                    clipPos = cameraInfo.screen * world * scale * localPos;
                    break;
                }
                case TEXT_RENDER_SURFACE_TYPE_CAMERA:
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
                case TEXT_RENDER_SURFACE_TYPE_SCREEN:
                {
                    mat4 scale = mat4_diagonal(0.3, 0.3, 0.3, 1);
                    clipPos = cameraInfo.projection * cameraInfo.view * world * scale * localPos;
                }
                    break;
                case TEXT_RENDER_SURFACE_TYPE_CAMERA:
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
}
