#section SETTINGS


#section LAYOUT

per-vertex [0, 0] vec3 position;
per-instance [1, 1] vec3 offset;
per-instance [1, 2] vec3 scale;
per-instance [1, 3] vec3 rotation;

vertex [push_constant] [0] uniform Push
{
	mat4 mvpMatrix;
} push;

fragment [0, 0] uniform Properties
{
	vec4 color;
} properties;

#section SHADER

#stage vertex

#version 450

layout(push_constant) uniform Push
{
	mat4 mvp;
} push;


//Inputs
//per vertex
layout(location = 0) in vec3 norm_position;

//per instance
layout(location = 1) in vec3 offset;
layout(location = 2) in vec3 scale;
layout(location = 3) in vec3 rotation;

void main()
{
	vec3 pos = offset;
	pos += norm_position;
	pos = pos * scale;
	gl_Position = push.mvp * vec4(pos, 1);
}


#stage fragment

#version 450

layout(set = 0, binding = 0) uniform Properties
{
	vec4 color;
} properties;

layout(location = 0) out vec4 color;

void main()
{
	color = properties.color;
}


