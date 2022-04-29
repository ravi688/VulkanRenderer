
#version 450

layout(set = 0, binding = 1) uniform PvkGlobalData
{
	mat4 projectionMatrix;			// projection matrix of the camera
	mat4 viewMatrix;				// view matrix of the camera
	mat4 lightProjectionMatrix;		// projection matrix of the light
	mat4 lightViewMatrix;			// view matrix of the light
} pvkGlobalData;

layout(set = 1, binding = 2) uniform PvkObjectData
{
	mat4 modelMatrix;				// model matrix of the object being rendered
	mat4 normalMatrix;				// normal matrix of the object being rendered
} pvkObjectData;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;
layout(location = 3) in vec4 color;

layout(location = 0) out vec2 _texcoord;
layout(location = 1) out vec3 _normal;
layout(location = 2) out vec4 _color;
layout(location = 3) out vec4 _shadowPos;

void main()
{
	_shadowPos = pvkGlobalData.lightProjectionMatrix * pvkGlobalData.lightViewMatrix * pvkObjectData.modelMatrix * vec4(position, 1.0);
	gl_Position = pvkGlobalData.projectionMatrix * pvkGlobalData.viewMatrix * pvkObjectData.modelMatrix * vec4(position, 1.0);
	_normal = (pvkObjectData.normalMatrix * vec4(normal, 0)).xyz;
	_texcoord = texcoord;
	_color = color;
}

