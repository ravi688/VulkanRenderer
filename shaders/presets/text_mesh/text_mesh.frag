#version 450

//#extension GL_EXT_scalar_block_layout: enable

#include <v3d.h>

//struct Color
//{
//    float r;
//    float g;
//    float b;
//};
//
//layout(std430, set = MATERIAL_SET, binding = MATERIAL_PROPERTIES_BINDING) uniform Parameters
//{
//    Color color;
//    int space_type;
//    int surface_type;
//} parameters;

layout(location = 0) in vec3 in_color;
layout(location = 0) out vec4 color;

void main()
{
    color = vec4(in_color.x, in_color.y, in_color.z, 1.0);
}
