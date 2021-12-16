
#include <renderer/internal/vulkan/vulkan_shader.h>
#include <renderer/stage_shader.h>

stage_shader_t* stage_shader_new() { return vulkan_shader_new(); }

stage_shader_t* stage_shader_create(renderer_t* renderer, void* spirv, uint32_t length, shader_stage_t shader_stage)
{
	return vulkan_shader_create(renderer, spirv, length, shader_stage);
}

void stage_shader_create_no_alloc(renderer_t* renderer, void* spirv, uint32_t length, shader_stage_t shader_stage, stage_shader_t* shader)
{
	vulkan_shader_create_no_alloc(renderer, spirv, length, shader_stage, shader);
}

stage_shader_t* stage_shader_load_and_create(renderer_t* renderer, const char* file_name, shader_stage_t shader_stage)
{
	return vulkan_shader_load_and_create(renderer, file_name, shader_stage);
}

void stage_shader_load_and_create_no_alloc(renderer_t* renderer, const char* file_name, shader_stage_t shader_stage, stage_shader_t* shader)
{
	vulkan_shader_load_and_create_no_alloc(renderer, file_name, shader_stage, shader);
}

void stage_shader_destroy(stage_shader_t* shader, renderer_t* renderer)
{
	vulkan_shader_destroy(shader, renderer);
}

void stage_shader_release_resources(stage_shader_t* shader)
{
	vulkan_shader_release_resources(shader);
}

