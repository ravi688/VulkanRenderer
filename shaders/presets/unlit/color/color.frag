
#version 450

layout(set = 3, binding = 0) uniform Parameters
{
	vec4 color;
} parameters;

layout(location = 0) out vec4 color;

void main()
{
	color = parameters.color;
}
