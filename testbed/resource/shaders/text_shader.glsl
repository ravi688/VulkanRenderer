

#stage vertex

#version 450

layout(push_constant) uniform Push
{
	//model view project matrix
	mat4 mvp;
} push;


//Inputs
//per vertex
layout(location = 0) in vec3 position;

//per instance
layout(location = 1) in vec3 offset;


void main()
{
	gl_Position = push.mvp * vec4(position + offset, 1);
}


#stage fragment

#version 450

layout(location = 0) out vec3 color;

void main()
{
	color = vec3(1, 1, 1);
}
