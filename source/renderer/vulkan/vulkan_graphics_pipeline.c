
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_graphics_pipeline.h>
#include <renderer/internal/vulkan/vulkan_pipeline_layout.h>
#include <renderer/internal/vulkan/vulkan_shader.h>
#include <renderer/internal/vulkan/vulkan_swapchain.h>
#include <renderer/render_window.h>

#include <renderer/assert.h>
#include <memory_allocator/memory_allocator.h>
#include <memory.h>

vulkan_graphics_pipeline_t* vulkan_graphics_pipeline_create(renderer_t* renderer, vulkan_graphics_pipeline_create_info_t* create_info)
{
	ASSERT(renderer->vk_device != VK_NULL_HANDLE, "renderer->vk_device == VK_NULL_HANDLE\n");
	ASSERT(renderer->vk_render_pass != VK_NULL_HANDLE, "renderer->vk_render_pass == VK_NULL_HANDLE\n");
	ASSERT(renderer->window != NULL, "renderer->window == NULL\n");

	vulkan_graphics_pipeline_t* pipeline = heap_new(vulkan_graphics_pipeline_t);
	memset(pipeline, 0, sizeof(vulkan_graphics_pipeline_t));

	pipeline->layout = vulkan_pipeline_layout_create(renderer);

	VkPipelineShaderStageCreateInfo* shader_stages = stack_newv(VkPipelineShaderStageCreateInfo, create_info->shader_count);
	for(u32 i = 0; i < create_info->shader_count; i++)
		shader_stages[i] = create_info->shaders[i]->stage;
	VkPipelineVertexInputStateCreateInfo vertexInputInfo = vk_get_pipeline_vertex_input_state_create_info(	create_info->vertex_info.attribute_count,
																											create_info->vertex_info.size,
																											VK_VERTEX_INPUT_RATE_VERTEX,
																											create_info->vertex_info.attribute_formats,
																											create_info->vertex_info.attribute_offsets);


	VkPipelineInputAssemblyStateCreateInfo inputAssembly = vk_get_pipeline_input_assembly_state_create_info();
	VkPipelineRasterizationStateCreateInfo rasterizer = vk_get_pipeline_rasterization_state_create_info();

	VkPipelineViewportStateCreateInfo viewportState = vk_get_pipeline_viewport_state_create_info(renderer->window->width, renderer->window->height);
	VkPipelineMultisampleStateCreateInfo multisampling = vk_get_pipeline_multisample_state_create_info();
	VkPipelineColorBlendStateCreateInfo colorBlending = vk_get_pipeline_color_blend_state_create_info();
	pipeline->pipeline = vk_get_graphics_pipeline(	renderer->vk_device, pipeline->layout->layout, renderer->vk_render_pass,
														shader_stages,
														&vertexInputInfo,
														&inputAssembly,
														&viewportState,
														&rasterizer,
														&multisampling,
														&colorBlending
													);
	stack_free(shader_stages);
	return pipeline;
}

void vulkan_graphics_pipeline_bind(vulkan_graphics_pipeline_t* pipeline, renderer_t* renderer)
{
	ASSERT(renderer->vk_device != VK_NULL_HANDLE, "renderer->vk_device == VK_NULL_HANDLE\n");
	ASSERT(renderer->swapchain != NULL, "renderer->swapchain == NULL\n");

	u32 image_index = renderer->swapchain->current_image_index;
	vkCmdBindPipeline(renderer->vk_command_buffers.value2[image_index], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipeline);
}

void vulkan_graphics_pipeline_destroy(vulkan_graphics_pipeline_t* pipeline, renderer_t* renderer)
{
	ASSERT(renderer->vk_device != VK_NULL_HANDLE, "renderer->vk_device == VK_NULL_HANDLE\n");
	vulkan_pipeline_layout_destroy(pipeline->layout, renderer);
	vkDestroyPipeline(renderer->vk_device, pipeline->pipeline, NULL);
	heap_free(pipeline);
}

