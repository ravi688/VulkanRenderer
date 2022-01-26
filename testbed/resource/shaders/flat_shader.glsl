
#section LAYOUT

vertex fragment [push_constant] [0] uniform Push
{
	mat4 mvp_matrix;
	mat4 model_matrix;
} push;

fragment [0, 0] uniform sampler2D albedo;
fragment [0, 1] uniform Light
{
	vec3 dir;
	float intensity;
	vec3 color;
} light;

#section SHADER

#stage vertex

#version 450

layout(push_constant) uniform Push
{
	mat4 mvp_matrix;
	mat4 model_matrix;
};

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;

layout(location = 0) out vec3 _normal;
layout(location = 1) out vec2 _texcoord;

void main()
{
	_normal = normalize((transpose(inverse(model_matrix)) * vec4(normal, 0)).xyz);
	_texcoord = texcoord;
	gl_Position = mvp_matrix * vec4(position, 1);
}


#stage fragment

#version 450

layout(set = 0, binding = 0) uniform sampler2D albedo;
layout(set = 0, binding = 1) uniform Light
{
	vec3 dir;
	float intensity;
	vec3 color;
} light;

layout(location = 0) in vec3 _normal;
layout(location = 1) in vec2 _texcoord;

layout(location = 0) out vec3 color;

void main()
{
	color = vec3(1, 1, 1) * light.color * light.intensity * max(0, dot(-_normal, light.dir));
}
