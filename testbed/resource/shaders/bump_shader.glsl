
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
layout(location = 1) out mat3 _tbn_matrix;

mat3 build_tbn_matrix(vec3 tangent, vec3 normal)
{
    return mat3(tangent, cross(tangent, normal), normal);
}

void main()
{
	mat4 normal_matrix = transpose(inverse(model_matrix));
	vec3 t = normalize((normal_matrix * vec4(tangent, 0)).xyz);
    vec3 n = normalize((normal_matrix * vec4(normal, 0)).xyz);
    _tbn_matrix = build_tbn_matrix(t, n);
	_texcoord = texcoord;
	gl_Position = mvp_matrix * vec4(position, 1);
}


#stage fragment

#version 450

layout(set = 0, binding = 0) uniform sampler2D albedo;
layout(set = 0, binding = 1) uniform sampler2D normal_map;
layout(set = 0, binding = 2) uniform Light
{
	vec3 dir;
	float intensity;
	vec3 color;
} light;

layout(location = 0) in vec2 _texcoord;
layout(location = 1) in mat3 _tbn_matrix;

layout(location = 0) out vec3 color;

vec3 unpack_normal(sampler2D normal_map, vec2 uv)
{
    return normalize(_tbn_matrix * normalize(texture(normal_map, uv).xyz * 2 - 1));
}

void main()
{
	vec3 normal = unpack_normal(normal_map, _texcoord);
	float t = 0.5f * dot(normal, -light.dir) + 0.5f;
	color = texture(albedo, _texcoord).xyz * light.color * light.intensity * (t + (1 - t) * 0);
}
