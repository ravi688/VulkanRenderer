
#version 450

layout(set = 0, binding = 1) uniform PvkGlobalData
{
	mat4 projectionMatrix;			// projection matrix of the light
	mat4 viewMatrix;				// view matrix of the light
} pvkGlobalData;

layout(set = 1, binding = 2) uniform PvkObjectData
{
	mat4 modelMatrix;				// model matrix of the object being rendered
	mat4 normalMatrix;				// normal matrix of the object being rendered
} pvkObjectData;

layout(location = 0) in vec3 position;

void main()
{
	vec4 _position = pvkGlobalData.projectionMatrix * pvkGlobalData.viewMatrix * pvkObjectData.modelMatrix * vec4(position, 1.0);
	gl_Position = _position;
}

