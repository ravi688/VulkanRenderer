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
    int isScreenSpace;
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

    float dpi = 141.0;
    float dpi_inv = 1 / dpi;
    vec2 tex_size = vec2(512, 512);
    vec2 win_size = vec2(800, 800);

    float _scale_x = (texcoord.trtc.x - texcoord.tltc.x) * tex_size.x * dpi_inv * (1.0 / (win_size.x * dpi_inv));
    float _scale_y = (texcoord.bltc.y - texcoord.tltc.y) * tex_size.y * dpi_inv * (1.0 / (win_size.y * dpi_inv));
    float _space = ofst.x * 0.1f;
    gl_Position = vec4(position.x * _scale_x + _space - 0.1 * 8, -position.y * _scale_y, 0, 1.0);

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
