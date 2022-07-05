
#version 450

#include <v3d.h>

layout(set = GLOBAL_SET, binding = LIGHT_BINDING) uniform PointLight lightInfo;
layout(set = CAMERA_SET, binding = CAMERA_PROPERTIES_BINDING) uniform CameraInfo camera;

layout(set = MATERIAL_SET, binding = TEXTURE_BINDING0) uniform samplerCube reflectionMap;

layout(set = MATERIAL_SET, binding = MATERIAL_PROPERTIES_BINDING) uniform Parameters
{
	vec4 color;
	float reflectance;
} parameters;


layout(location = 0) in vec3 normal;
layout(location = 1) in vec3 position;

layout(location = 0) out vec4 color;


float linearize(float depth, float n, float f)
{
	float t = ((n - f) * depth + f) * (f - n);
	return n * f * 1 / t;
}


void main()
{
	mat4 camera_transform = camera.transform;
	vec3 camera_position = vec3(camera_transform[3][0], camera_transform[3][1], camera_transform[3][2]);

	vec3 eye_vector = normalize(camera_position - position);

	vec3 reflection_vector = 2 * dot(eye_vector, normal) * normal - eye_vector;

	float depth = texture(reflectionMap, vec3(position.z, position.y, position.x)).r;
	depth = linearize(depth, 0.04, 20.0);
	vec4 reflection_color = mix(vec4(depth, depth, depth, 1), vec4(1, 1, 1, 1), 1 - parameters.reflectance);

	vec4 specular_color = mix(parameters.color, vec4(1, 1, 1, 1), 0.5 * dot(normalize(reflection_vector), normal) + 0.5);

	float dp = 1.0; //0.5 * dot(-normal, lightInfo.direction) + 0.5;
	color = dp * specular_color * vec4(lightInfo.color, 1) * reflection_color;
}
