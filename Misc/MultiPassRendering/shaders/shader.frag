

#version 450

struct PvkDirectionalLight
{
	vec3 color;
	vec3 dir;
	float intensity;
};

struct PvkAmbientLight
{
	vec3 color;
	float intensity;
};

layout(set = 0, binding = 1) uniform PvkGlobalData
{
	mat4 _pad1;
	mat4 _pad2;
	PvkDirectionalLight dirLight;
	PvkAmbientLight ambLight;
} pvkGlobalData;


layout(location = 0) in vec2 _texcoord;
layout(location = 1) in vec3 _normal;
layout(location = 2) in vec4 _color;

layout(location = 0) out vec4 color;

void main()
{
	float t = max(0, dot(-pvkGlobalData.dirLight.dir, _normal));
	vec3 light1 = t  * pvkGlobalData.dirLight.color * pvkGlobalData.dirLight.intensity;
	vec3 light2 = pvkGlobalData.ambLight.color * pvkGlobalData.ambLight.intensity;
	vec4 lighting = vec4(light1 + light2, 1);
	color = _color * lighting;	
}

