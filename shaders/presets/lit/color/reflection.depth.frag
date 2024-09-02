/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: reflection.depth.frag is a part of VulkanRenderer

	Copyright (C) 2021 - 2024  Author: Ravi Prakash Singh

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>. 
*/


#version 450

#define USE_FAR_LIGHTS
#include <v3d.h>

layout(SGE_UNIFORM_BUFFER_LAYOUT, set = CAMERA_SET, binding = CAMERA_PROPERTIES_BINDING) uniform CameraInfo camera;

layout(set = MATERIAL_SET, binding = TEXTURE_BINDING0) uniform samplerCube reflectionMap;

layout(SGE_UNIFORM_BUFFER_LAYOUT, set = MATERIAL_SET, binding = MATERIAL_PROPERTIES_BINDING) uniform Parameters
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

	vec4 irradiance = getFarLightIrradiance(normal);
	color = irradiance * specular_color * reflection_color;
}
