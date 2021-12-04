
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_graphics_pipeline.h>
#include <renderer/internal/vulkan/vulkan_pipeline_layout.h>
#include <renderer/internal/vulkan/vulkan_shader.h>
#include <renderer/internal/vulkan/vulkan_swapchain.h>
#include <renderer/render_window.h>

#include <renderer/assert.h>
#include <memory_allocator/memory_allocator.h>
#include <memory.h>

vulkan_graphics_pipeline_t* vulkan_graphics_pipeline_new()
{
	vulkan_graphics_pipeline_t* pipeline = heap_new(vulkan_graphics_pipeline_t);
	memset(pipeline, 0, sizeof(vulkan_graphics_pipeline_t));
	refp(vulkan_graphics_pipeline_t, pipeline, 0)->layout = vulkan_pipeline_layout_new();
	return pipeline;
}

void vulkan_graphics_pipeline_create_no_alloc(renderer_t* renderer, vulkan_graphics_pipeline_create_info_t* create_info, vulkan_graphics_pipeline_t* pipeline)
{
	ASSERT(renderer->vk_device != VK_NULL_HANDLE, "renderer->vk_device == VK_NULL_HANDLE\n");
	ASSERT(renderer->vk_render_pass != VK_NULL_HANDLE, "renderer->vk_render_pass == VK_NULL_HANDLE\n");
	ASSERT(renderer->window != NULL, "renderer->window == NULL\n");

	vulkan_pipeline_layout_create_no_alloc(renderer, refp(vulkan_pipeline_layout_t, refp(vulkan_graphics_pipeline_t, pipeline, 0)->layout, 0));

	VkPipelineShaderStageCreateInfo* shader_stages = stack_newv(VkPipelineShaderStageCreateInfo, create_info->shader_count);
	for(u32 i = 0; i < create_info->shader_count; i++)
		ref(VkPipelineShaderStageCreateInfo, shader_stages, i) = (*refp(vulkan_shader_t*, create_info->shaders, i))->stage;

	vertex_attribute_binding_info_t* vertex_attribute_infos = stack_alloc(sizeof(vertex_attribute_binding_info_t) * create_info->vertex_info_count);
	memset(vertex_attribute_infos, 0, sizeof(vertex_attribute_binding_info_t) * create_info->vertex_info_count);
	uint32_t* strides = stack_alloc(sizeof(uint32_t) * create_info->vertex_info_count);
	memset(strides, 0, sizeof(uint32_t) * create_info->vertex_info_count);
	for(uint32_t i = 0; i < create_info->vertex_info_count; i++)
	{
		vertex_attribute_infos[i].attribute_count = create_info->vertex_infos[i].attribute_count;
		vertex_attribute_infos[i].attribute_offsets = create_info->vertex_infos[i].attribute_offsets;
		vertex_attribute_infos[i].attribute_formats = create_info->vertex_infos[i].attribute_formats;
		strides[i] = create_info->vertex_infos[i].size;
	}
	VkPipelineVertexInputStateCreateInfo vertexInputInfo = vk_get_pipeline_vertex_input_state_create_info(create_info->vertex_info_count, strides, vertex_attribute_infos);

	VkPipelineInputAssemblyStateCreateInfo inputAssembly = vk_get_pipeline_input_assembly_state_create_info();
	VkPipelineRasterizationStateCreateInfo rasterizer = vk_get_pipeline_rasterization_state_create_info();

	VkPipelineViewportStateCreateInfo viewportState = vk_get_pipeline_viewport_state_create_info(renderer->window->width, renderer->window->height);
	VkPipelineMultisampleStateCreateInfo multisampling = vk_get_pipeline_multisample_state_create_info();
	VkPipelineColorBlendStateCreateInfo colorBlending = vk_get_pipeline_color_blend_state_create_info();
	pipeline->pipeline = vk_get_graphics_pipeline(	renderer->vk_device, refp(vulkan_pipeline_layout_t, refp(vulkan_graphics_pipeline_t, pipeline, 0)->layout, 0)->layout, renderer->vk_render_pass,
														refp(VkPipelineShaderStageCreateInfo, shader_stages, 0),
														&vertexInputInfo,
														&inputAssembly,
														&viewportState,
														&rasterizer,
														&multisampling,
														&colorBlending
													);
	stack_free(shader_stages);
	stack_free(vertex_attribute_infos);
	stack_free(strides);
}

vulkan_graphics_pipeline_t* vulkan_graphics_pipeline_create(renderer_t* renderer, vulkan_graphics_pipeline_create_info_t* create_info)
{
	vulkan_graphics_pipeline_t* pipeline = vulkan_graphics_pipeline_new();
	vulkan_graphics_pipeline_create_no_alloc(renderer, create_info, refp(vulkan_graphics_pipeline_t, pipeline, 0));
	return pipeline;
}


void vulkan_graphics_pipeline_bind(vulkan_graphics_pipeline_t* pipeline, renderer_t* renderer)
{
	ASSERT(renderer->vk_device != VK_NULL_HANDLE, "renderer->vk_device == VK_NULL_HANDLE\n");
	ASSERT(renderer->swapchain != NULL, "renderer->swapchain == NULL\n");

	u32 image_index = renderer->swapchain->current_image_index;
	vkCmdBindPipeline(renderer->vk_command_buffers.value2[image_index], VK_PIPELINE_BIND_POINT_GRAPHICS, refp(vulkan_graphics_pipeline_t, pipeline, 0)->pipeline);
}

void vulkan_graphics_pipeline_destroy(vulkan_graphics_pipeline_t* pipeline, renderer_t* renderer)
{
	ASSERT(renderer->vk_device != VK_NULL_HANDLE, "renderer->vk_device == VK_NULL_HANDLE\n");
	vulkan_pipeline_layout_destroy(refp(vulkan_pipeline_layout_t, refp(vulkan_graphics_pipeline_t, pipeline, 0)->layout, 0), renderer);
	vkDestroyPipeline(renderer->vk_device, refp(vulkan_graphics_pipeline_t, pipeline, 0)->pipeline, NULL);
}

void vulkan_graphics_pipeline_release_resources(vulkan_graphics_pipeline_t* pipeline)
{
	heap_free(pipeline->layout);
	heap_free(pipeline);
}