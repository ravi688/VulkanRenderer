

#version 450


layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;
layout(location = 3) in vec4 color;

layout(location = 0) out vec2 _texcoord;
layout(location = 1) out vec3 _normal;
layout(location = 2) out vec4 _color;

void main()
{
	gl_Position = vec4(position, 1.0);
	_normal = normal;
	_texcoord = texcoord;
	_color = color;
}

