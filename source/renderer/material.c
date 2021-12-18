
#include <renderer/internal/vulkan/vulkan_material.h>

#include <renderer/material.h>
#include <memory_allocator/memory_allocator.h>

#define VERTEX_INFO_COUNT 4

instantiate_static_stack_array(VkFormat);

material_t* material_new() { return vulkan_material_new(); }

material_t* material_create(renderer_t* renderer, u32 shader_count, stage_shader_t** shaders)
{
	VkFormat* attribute_formats1 = stack_array(VkFormat, 1, VK_FORMAT_R32G32B32_SFLOAT);
	VkFormat* attribute_formats2 = stack_array(VkFormat, 1, VK_FORMAT_R32G32B32_SFLOAT);
	VkFormat* attribute_formats3 = stack_array(VkFormat, 1, VK_FORMAT_R32G32B32_SFLOAT);
	VkFormat* attribute_formats4 = stack_array(VkFormat, 1, VK_FORMAT_R32G32_SFLOAT);
	u32* attribute_offsets1 = stack_array(u32, 1, 0);
	u32* attribute_offsets2 = stack_array(u32, 1, 0);
	u32* attribute_offsets3 = stack_array(u32, 1, 0);
	u32* attribute_offsets4 = stack_array(u32, 1, 0);
	vulkan_vertex_info_t* vertex_infos = stack_newv(vulkan_vertex_info_t, VERTEX_INFO_COUNT);
	ref(vulkan_vertex_info_t, vertex_infos, 0) = (vulkan_vertex_info_t) { VK_VERTEX_INPUT_RATE_VERTEX, sizeof(float) * 3, 1, attribute_formats1, attribute_offsets1 };
	ref(vulkan_vertex_info_t, vertex_infos, 1) = (vulkan_vertex_info_t) { VK_VERTEX_INPUT_RATE_VERTEX, sizeof(float) * 3, 1, attribute_formats2, attribute_offsets2 };
	ref(vulkan_vertex_info_t, vertex_infos, 2) = (vulkan_vertex_info_t) { VK_VERTEX_INPUT_RATE_VERTEX, sizeof(float) * 3, 1, attribute_formats3, attribute_offsets3 };
	ref(vulkan_vertex_info_t, vertex_infos, 3) = (vulkan_vertex_info_t) { VK_VERTEX_INPUT_RATE_VERTEX, sizeof(float) * 2, 1, attribute_formats4, attribute_offsets4 };

	vulkan_material_create_info_t material_info =
	{
		.shaders = shaders,
		.shader_count = shader_count,
		.vertex_info_count = VERTEX_INFO_COUNT,
		//NOTE: calling vulkan_material_create() creates a deep copy of vertex_infos
		.vertex_infos = vertex_infos
	};
	vulkan_material_t* material = vulkan_material_create(renderer, &material_info);

	stack_free(attribute_offsets1);
	stack_free(attribute_offsets2);
	stack_free(attribute_offsets3);
	stack_free(attribute_offsets4);
	stack_free(attribute_formats1);
	stack_free(attribute_formats2);
	stack_free(attribute_formats3);
	stack_free(attribute_formats4);
	stack_free(vertex_infos);
	return material;
}

void material_create_no_alloc(renderer_t* renderer, u32 shader_count, stage_shader_t** shaders, material_t* material)
{
	VkFormat* attribute_formats1 = stack_array(VkFormat, 1, VK_FORMAT_R32G32B32_SFLOAT);
	VkFormat* attribute_formats2 = stack_array(VkFormat, 1, VK_FORMAT_R32G32B32_SFLOAT);
	VkFormat* attribute_formats3 = stack_array(VkFormat, 1, VK_FORMAT_R32G32B32_SFLOAT);
	VkFormat* attribute_formats4 = stack_array(VkFormat, 1, VK_FORMAT_R32G32_SFLOAT);
	u32* attribute_offsets1 = stack_array(u32, 1, 0);
	u32* attribute_offsets2 = stack_array(u32, 1, 0);
	u32* attribute_offsets3 = stack_array(u32, 1, 0);
	u32* attribute_offsets4 = stack_array(u32, 1, 0);
	vulkan_vertex_info_t* vertex_infos = stack_newv(vulkan_vertex_info_t, VERTEX_INFO_COUNT);
	ref(vulkan_vertex_info_t, vertex_infos, 0) = (vulkan_vertex_info_t) { VK_VERTEX_INPUT_RATE_VERTEX, sizeof(float) * 3, 1, attribute_formats1, attribute_offsets1 };
	ref(vulkan_vertex_info_t, vertex_infos, 1) = (vulkan_vertex_info_t) { VK_VERTEX_INPUT_RATE_VERTEX, sizeof(float) * 3, 1, attribute_formats2, attribute_offsets2 };
	ref(vulkan_vertex_info_t, vertex_infos, 2) = (vulkan_vertex_info_t) { VK_VERTEX_INPUT_RATE_VERTEX, sizeof(float) * 3, 1, attribute_formats3, attribute_offsets3 };
	ref(vulkan_vertex_info_t, vertex_infos, 3) = (vulkan_vertex_info_t) { VK_VERTEX_INPUT_RATE_VERTEX, sizeof(float) * 2, 1, attribute_formats4, attribute_offsets4 };

	vulkan_material_create_info_t material_info =
	{
		.shaders = shaders,
		.shader_count = shader_count,
		.vertex_info_count = VERTEX_INFO_COUNT,
		//NOTE: calling vulkan_material_create_no_alloc() doesn't creates a deep copy of vertex_infos
		.vertex_infos = vertex_infos
	};
	vulkan_material_create_no_alloc(renderer, &material_info, material);

	stack_free(attribute_offsets1);
	stack_free(attribute_offsets2);
	stack_free(attribute_offsets3);
	stack_free(attribute_offsets4);
	stack_free(attribute_formats1);
	stack_free(attribute_formats2);
	stack_free(attribute_formats3);
	stack_free(attribute_formats4);
	stack_free(vertex_infos);	
}

void material_destroy(material_t* material, renderer_t* renderer)
{
	vulkan_material_destroy(material, renderer);
}

void material_release_resources(material_t* material)
{
	vulkan_material_release_resources(material);
}

void material_bind(material_t* material, renderer_t* renderer)
{
	vulkan_material_bind(material, renderer);
}

void material_push_constants(material_t* material, renderer_t* renderer, void* bytes)
{
	vulkan_material_push_constants(material, renderer, bytes);
}

void material_set_texture(material_t* material, renderer_t* renderer, texture_t* texture)
{
	vulkan_material_set_texture(material, renderer, texture);
}
