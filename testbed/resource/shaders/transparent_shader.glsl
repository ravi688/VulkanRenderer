#section SHADER

#stage vertex

#version 450

layout(push_constant) uniform Push
{
	mat4 mvp;
} push;

layout(location = 0) in vec3 position;


void main()
{
	gl_Position = push.mvp * vec4(position, 1.0);
}


#stage fragment

#version 450

layout(location = 0) out vec4 color;

void main()
{
	color = vec4(0.1f, 0.6f, 0.4f, 0.2f);
}
