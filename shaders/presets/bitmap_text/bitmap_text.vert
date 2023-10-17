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

struct GlyphTexCoord
{
    vec2 tltc;
    vec2 trtc;
    vec2 brtc;
    vec2 bltc;
};

layout(std430, set = MATERIAL_SET, binding = TEXTURE_BINDING1) uniform GTCBuffer
{
    GlyphTexCoord[512] gtc_buffer;
};

layout(std430, set = MATERIAL_SET, binding = TEXTURE_BINDING2) uniform TSTBuffer
{
    mat4[128] tst_buffer;
};

layout(location = POSITION_LOCATION) in vec3 position;

layout(location = 5, component = 0) in vec3 ofst;
layout(location = 5, component = 3) in float indx_f;
layout(location = 6, component = 0) in vec3 rotn; // not required
layout(location = 6, component = 3) in float stid_f;
layout(location = 7, component = 0) in vec3 scal; // not required

layout(location = 0) out vec2 out_texcoord;
layout(location = 1) out vec3 out_color;

void main()
{
    uint indx = floatBitsToUint(indx_f);
    uint stid = floatBitsToUint(stid_f);

    GlyphTexCoord texcoord = gtc_buffer[indx];

    vec2 tex_size = parameters.tex_size;
    vec2 win_size = vec2(displayInfo.window_size);
    vec2 glyph_size = vec2(texcoord.trtc.x - texcoord.tltc.x, texcoord.bltc.y - texcoord.tltc.y) * tex_size;

    vec4 pos = vec4((position.x * glyph_size.x + ofst.x) * 0.5, (position.y * glyph_size.y + ofst.y) * 0.5, 0, 1.0);

    vec4 world = objectInfo.transform * tst_buffer[stid] * pos.zyxw;

    vec4 clipPos;

    switch(parameters.space_type)
    {
        case TEXT_RENDER_SPACE_TYPE_2D:
        {
            switch(parameters.surface_type)
            {
                case TEXT_RENDER_SURFACE_TYPE_CAMERA:
                    clipPos = cameraInfo.screen * world;
                    break;
                case TEXT_RENDER_SURFACE_TYPE_SCREEN:
                    clipPos = displayInfo.matrix * world;
                    break;
            }
            break;
        }
        case TEXT_RENDER_SPACE_TYPE_3D:
        {
            switch(parameters.surface_type)
            {
                case TEXT_RENDER_SURFACE_TYPE_CAMERA:
                    clipPos = cameraInfo.projection * cameraInfo.view * world;
                    break;
                case TEXT_RENDER_SURFACE_TYPE_SCREEN:
                    clipPos = displayInfo.matrix * world;
                    break;
            }
            break;
        }
    }

    gl_Position = vec4(clipPos.x, -clipPos.y, clipPos.z, clipPos.w);

    if(gl_VertexIndex == 0)
        out_texcoord = texcoord.tltc;
    else if(gl_VertexIndex == 1)
        out_texcoord = texcoord.trtc;
    else if(gl_VertexIndex == 2)
        out_texcoord = texcoord.brtc;
    else if(gl_VertexIndex == 3)
        out_texcoord = texcoord.bltc;

    out_color = vec3(parameters.color.r, parameters.color.g, parameters.color.b);
}
