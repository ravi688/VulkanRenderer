
#version 450

#include <v3d.h>

layout(
	input_attachment_index = INPUT_ATTACHMENT_BINDING0, 
	set = SUB_RENDER_SET, 
	binding = INPUT_ATTACHMENT_BINDING0
	) uniform subpassInput previousColor;

layout(location = 0) out vec4 color;

void main()
{
	// 0.2126 R + 0.7152 G + 0.0722 B
	float luminosity = dot(subpassLoad(previousColor).rgba, vec4(0.2126, 0.7152, 0.0722, 0));
	color = vec4(luminosity, luminosity, luminosity, 1);
}

