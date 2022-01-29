
#section LAYOUT

vertex [push_constant] [0] uniform Push
{
	mat4 mvp_matrix;
	mat4 model_matrix;
} push;

fragment [0, 0] uniform sampler2D albedo;
fragment [0, 1] uniform sampler2D normal_map;
fragment [0, 2] uniform Light
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
layout(location = 3) in vec3 tangent;

layout(location = 0) out vec2 _texcoord;
layout(location = 1) out vec3 _tangent;
layout(location = 2) out vec3 _normal;
layout(location = 3) out vec3 _bitangent;

void main()
{
	mat4 normal_matrix = transpose(inverse(model_matrix));
	_tangent = normalize((normal_matrix * vec4(tangent, 0)).xyz);
 	_normal = normalize((normal_matrix * vec4(normal, 0)).xyz);
    _bitangent = cross(_normal, _tangent);
	_texcoord = texcoord;
	gl_Position = mvp_matrix * vec4(position, 1);
}


#stage fragment

#version 450
#extension GL_EXT_debug_printf : enable

layout(set = 0, binding = 0) uniform sampler2D albedo;
layout(set = 0, binding = 1) uniform sampler2D normal_map;
layout(set = 0, binding = 2) uniform Light
{
	vec3 dir;
	float intensity;
	vec3 color;
} light;

layout(location = 0) in vec2 _texcoord;
layout(location = 1) in vec3 _tangent;
layout(location = 2) in vec3 _normal;
layout(location = 3) in vec3 _bitangent;

layout(location = 0) out vec4 color;

void main()
{
	vec3 n = (texture(normal_map, _texcoord).rgb * 2 - 1);
	vec3 normal = _bitangent * n.g + _tangent * n.r + _normal * n.b;
	float t = 0.5f * dot(normal, -light.dir) + 0.5f;
	t = t + (1 - t) * 0.1f;

	//	Bug description:
	//	 all the calculations are correct but when the result has been assigned to color, it changed!
	//	Fix:
	//	 No fix found till now.
	color = vec4(texture(albedo, _texcoord).rgb * light.color * light.intensity * t, 1);
}
