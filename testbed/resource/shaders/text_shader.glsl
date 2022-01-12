#section SETTINGS


#section LAYOUT

vertex [0, 0] uniform UBO
{
	float time;
} ubo;


#section SHADER

#stage vertex

#version 450

layout(binding = 0) uniform UBO
{
	float time;
} ubo;

layout(push_constant) uniform Push
{
	mat4 mvp;
} push;


//Inputs
//per vertex
layout(location = 0) in vec3 position;

//per instance
layout(location = 1) in vec3 offset;


void main()
{
	gl_Position = push.mvp * vec4(position + offset + vec3(0, cos(ubo.time), sin(ubo.time)), 1);
}


#stage fragment

#version 450

layout(location = 0) out vec3 color;

void main()
{
	color = vec3(1, 1, 1);
}


