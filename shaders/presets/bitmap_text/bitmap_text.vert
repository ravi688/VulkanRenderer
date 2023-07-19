#version 450

#extension GL_EXT_scalar_block_layout : enable

#include <v3d.h>
                
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
    uvec2 win_size;
    uvec2 tex_size;
    Color color;
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
    mat4[] tst_buffer;
};

layout(location = POSITION_LOCATION) in vec3 position;
                
layout(location = 5, component = 0) in vec3 ofst;
layout(location = 5, component = 3) in float indx_f;
layout(location = 6, component = 0) in vec3 rotn;
layout(location = 6, component = 3) in float stid_f;
layout(location = 7, component = 0) in vec3 scal;

const vec3 colors[] = 
{ 
    vec3(1.0, 0.0, 0.0), 
    vec3(0.0, 1.0, 0.0),
    vec3(0.5, 0.5, 0.5),
    vec3(1.0, 0.0, 1.0),
    vec3(1.0, 1.0, 0.0),
    vec3(0.0, 1.0, 1.0),
    vec3(0.5, 0.5, 1.0),
    vec3(0.5, 1.0, 0.5),
    vec3(1.0, 0.5, 0.5)
};

layout(location = 0) out vec2 out_texcoord;
layout(location = 1) out vec3 out_color;
                
void main()
{
    uint indx = floatBitsToUint(indx_f);
    uint stid = floatBitsToUint(stid_f);

    GlyphTexCoord texcoord = gtc_buffer[indx];

    vec2 tex_size = parameters.tex_size;
    vec2 win_size = parameters.win_size;
    vec2 glyph_size = vec2(texcoord.trtc.x - texcoord.tltc.x, texcoord.bltc.y - texcoord.tltc.y) * tex_size;

    float _scale_x = glyph_size.x / win_size.x;
    float _scale_y = glyph_size.y / win_size.y;
    gl_Position = vec4(position.x * _scale_x + ofst.x / win_size.x, -(position.y * _scale_y + ofst.y / win_size.y), 0, 1.0);

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
