

#version 450

struct PvkDirectionalLight
{
	vec3 color;
	float intensity;
	vec3 dir;
};

struct PvkAmbientLight
{
	vec3 color;
	float intensity;
};

layout(set = 0, binding = 1) uniform PvkGlobalData
{
	mat4 projectionMatrix;		// projection matrix of the camera
	mat4 viewMatrix;	 		// view matrix of the camera
	mat4 lightProjectionMatrix; // projection matrix of the light
	mat4 lightViewMatrix; 		// view matrix of the light
	PvkDirectionalLight dirLight;
	PvkAmbientLight ambLight;
} pvkGlobalData;

layout(set = 1, binding = 2) uniform PvkObjectData
{
	mat4 modelMatrix;			// model matrix of the object being rendered
	mat4 normalMatrix;			// normal matrix of the object being rendered
} pvkObjectData;

layout(set = 2, binding = 3) uniform sampler2D shadowMap;			// shadow map depth buffer sampler

layout(location = 0) in vec2 _texcoord;
layout(location = 1) in vec3 _normal;
layout(location = 2) in vec4 _color;
layout(location = 3) in vec4 _shadowPos;
layout(location = 0) out vec4 color;


void main()
{
	float lightSpaceDepth = _shadowPos.z / _shadowPos.w;
	vec2 shadowCoord = vec2(_shadowPos.x / _shadowPos.w, -_shadowPos.y / _shadowPos.w);
	shadowCoord.x = (shadowCoord.x + 1.0) / 2.0;			// map 0 -> 1
	shadowCoord.y = (shadowCoord.y + 1.0) / 2.0;			// map 0 -> 1
	float dist = texture(shadowMap, shadowCoord).r;
	if(lightSpaceDepth > dist)
		color = vec4(0.1, 0.1, 0.1, 1.0);
	else
	{
		float t = max(0, dot(-pvkGlobalData.dirLight.dir, _normal));
		vec3 light1 = t  * pvkGlobalData.dirLight.color * pvkGlobalData.dirLight.intensity;
		vec3 light2 = pvkGlobalData.ambLight.color * pvkGlobalData.ambLight.intensity;
		vec4 lighting = vec4(light1 + light2, 1);
		color = _color * lighting;
	}
}

