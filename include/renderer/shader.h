
#pragma once

#include <renderer/stage_shader.h>
#include <renderer/defines.h>
#include <renderer/struct_descriptor.h>
#include <buffer.h>

typedef struct shader_resource_descriptor_t
{
	struct_descriptor_t handle;
	bool is_opaque;
	bool is_uniform;
	u8 binding_number;
	u8 set_number;
	//NOTE: These bit descriptions are based on the vulkan_shader_stage_t and shader_stage_t enums
	u8 stage_flags; 	// BIT(0) = vertex shader, BIT(1) = fragment shader, BIT(2) = geometry shader, BIT(3) = tessellation shader
} shader_resource_descriptor_t;

typedef struct shader_t
{
	stage_shader_t** stage_shaders;
	u8 stage_count;
	shader_resource_descriptor_t* descriptors;
	u16 descriptor_count;
} shader_t;

#define shader_load(renderer, file_path) shader_load_and_create(renderer, file_path)

shader_t* shader_new();
shader_t* shader_create(renderer_t* renderer, BUFFER* shader_binary);
shader_t* shader_load_and_create(renderer_t* renderer, const char* file_path);
void shader_destroy(shader_t* shader, renderer_t* renderer);
void shader_release_resources(shader_t* shader);
