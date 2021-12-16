
#include <renderer/internal/vulkan/vulkan_pipeline_layout.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_swapchain.h>
#include <memory_allocator/memory_allocator.h>
#include <memory.h>

vulkan_pipeline_layout_t* vulkan_pipeline_layout_new()
{
	vulkan_pipeline_layout_t* layout = heap_new(vulkan_pipeline_layout_t);
	memset(layout, 0, sizeof(vulkan_pipeline_layout_t));
	return layout;
}

void vulkan_pipeline_layout_create_no_alloc(renderer_t* renderer, vulkan_pipeline_layout_t* pipeline_layout)
{
	VkPushConstantRange* push_constant_range = stack_new(VkPushConstantRange);
	/*TODO: push_constant_range->size = sizeof(mat4_t(float));*/
	push_constant_range->size = sizeof(float) * 16;
	push_constant_range->offset = 0;
	push_constant_range->stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

	/* Descriptor set layout */
	VkDescriptorSetLayoutBinding layout_binding =
	{
		.binding = 0,
		.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		.descriptorCount = 1,
		.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
		.pImmutableSamplers = NULL, //optional
	};

	VkDescriptorSetLayoutCreateInfo layout_create_info =
	{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.bindingCount = 1,
		.pBindings = &layout_binding
	};
	vkCall(vkCreateDescriptorSetLayout(renderer->vk_device, &layout_create_info, NULL, &(pipeline_layout->descriptor_set_layout)));
	pipeline_layout->pipeline_layout = vk_get_pipeline_layout(renderer->vk_device, 1, &(pipeline_layout->descriptor_set_layout), 1, push_constant_range);
	stack_free(push_constant_range);
}

vulkan_pipeline_layout_t* vulkan_pipeline_layout_create(renderer_t* renderer)
{
	vulkan_pipeline_layout_t* pipeline_layout = vulkan_pipeline_layout_new();
	vulkan_pipeline_layout_create_no_alloc(renderer, pipeline_layout);
	return pipeline_layout;
}


void vulkan_pipeline_layout_destroy(vulkan_pipeline_layout_t* pipeline_layout, renderer_t* renderer)
{
	vkDestroyDescriptorSetLayout(renderer->vk_device, pipeline_layout->descriptor_set_layout, NULL);
	vkDestroyPipelineLayout(renderer->vk_device, pipeline_layout->pipeline_layout, NULL);
}

void vulkan_pipeline_layout_release_resources(vulkan_pipeline_layout_t* pipeline_layout)
{
	heap_free(pipeline_layout);
}


void vulkan_pipeline_layout_push_constants(vulkan_pipeline_layout_t* pipeline_layout, renderer_t* renderer, void* bytes)
{
	vkCmdPushConstants(ref(VkCommandBuffer, renderer->vk_command_buffers.value2, renderer->swapchain->current_image_index), pipeline_layout->pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(float) * 16, bytes);
}