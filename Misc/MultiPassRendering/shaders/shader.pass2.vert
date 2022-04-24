
#version 450

layout(set = 1, binding = 1) uniform PvkGlobalData
{
	mat4 projectionMatrix;
	mat4 viewMatrix;
} pvkGlobalData;

layout(set = 2, binding = 2) uniform PvkObjectData
{
	mat4 modelMatrix;
	mat4 normalMatrix;
} pvkObjectData;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;
layout(location = 3) in vec4 color;

layout(location = 0) out vec2 _texcoord;
layout(location = 1) out vec3 _normal;
layout(location = 2) out vec4 _color;

void main()
{
	vec4 _position = pvkGlobalData.projectionMatrix * pvkGlobalData.viewMatrix * pvkObjectData.modelMatrix * vec4(position, 1.0);
	gl_Position = _position;
	_normal = (pvkObjectData.normalMatrix * vec4(normal, 0)).xyz;
	_texcoord = texcoord;
	_color = color;
}

