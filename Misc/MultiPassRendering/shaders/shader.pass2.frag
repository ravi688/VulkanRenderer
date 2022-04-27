

#version 450

layout(input_attachment_index = 0, set = 0, binding = 0) uniform subpassInput previousResult;

layout(location = 0) in vec2 _texcoord;
layout(location = 1) in vec3 _normal;
layout(location = 2) in vec4 _color;

layout(location = 0) out vec4 color;

void main()
{
	vec4 previousColor = subpassLoad(previousResult);
	color = previousColor;
}

