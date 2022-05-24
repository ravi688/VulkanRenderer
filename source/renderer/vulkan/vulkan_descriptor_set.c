#include <renderer/internal/vulkan/vulkan_defines.h>
#include <renderer/internal/vulkan/vulkan_descriptor_set.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_swapchain.h>
#include <renderer/internal/vulkan/vulkan_texture.h>
#include <renderer/internal/vulkan/vulkan_buffer.h>
#include <renderer/internal/vulkan/vulkan_pipeline_layout.h>
#include <renderer/assert.h>
#include <renderer/memory_allocator.h>

RENDERER_API vulkan_descriptor_set_t* vulkan_descriptor_set_new()
{
	vulkan_descriptor_set_t* set = heap_new(vulkan_descriptor_set_t);
	memset(set, 0, sizeof(vulkan_descriptor_set_t));
	return set;
}

RENDERER_API void vulkan_descriptor_set_create_no_alloc(vulkan_renderer_t* renderer,  vulkan_descriptor_set_create_info_t* create_info, vulkan_descriptor_set_t* set)
{
	VkDescriptorSetAllocateInfo alloc_info =
	{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.descriptorPool = create_info->pool,
		.descriptorSetCount = 1,
		.pSetLayouts = &create_info->layout
	};
	vkCall(vkAllocateDescriptorSets(renderer->logical_device->handle, &alloc_info, &set->handle));
	set->pool = create_info->pool;
}

RENDERER_API vulkan_descriptor_set_t* vulkan_descriptor_set_create(vulkan_renderer_t* renderer, vulkan_descriptor_set_create_info_t* create_info)
{
	vulkan_descriptor_set_t* set = vulkan_descriptor_set_new();
	vulkan_descriptor_set_create_no_alloc(renderer, create_info, set);
	return set;
}

RENDERER_API void vulkan_descriptor_set_destroy(vulkan_descriptor_set_t* set, vulkan_renderer_t* renderer)
{
	vkCall(vkFreeDescriptorSets(renderer->logical_device->handle, set->pool, 1, &set->handle));
}

RENDERER_API void vulkan_descriptor_set_release_resources(vulkan_descriptor_set_t* set)
{
	heap_free(set);
}

RENDERER_API void vulkan_descriptor_set_bind(vulkan_descriptor_set_t* set, vulkan_renderer_t* renderer, u32 set_number, vulkan_pipeline_layout_t* pipeline_layout)
{
	u32 image_index = renderer->swapchain->current_image_index;
	vkCmdBindDescriptorSets(renderer->vo_command_buffers[image_index], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout->handle, set_number, 1, &set->handle, 0, NULL);
}

RENDERER_API void vulkan_descriptor_set_write_texture(vulkan_descriptor_set_t* set, vulkan_renderer_t* renderer, u32 binding_index, vulkan_texture_t* texture)
{
	VkDescriptorImageInfo image_info =
	{
		.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		.imageView = texture->image_view->handle,
		.sampler = texture->image_sampler
	};
	VkWriteDescriptorSet descriptor_write =
	{
		.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
		.dstSet = set->handle,
		.dstBinding = binding_index,
		.dstArrayElement = 0,
		.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		.descriptorCount = 1,
		.pImageInfo = &image_info,
	};
	vkUpdateDescriptorSets(renderer->logical_device->handle, 1, &descriptor_write, 0, NULL);
}

RENDERER_API void vulkan_descriptor_set_write_uniform_buffer(vulkan_descriptor_set_t* set, vulkan_renderer_t* renderer, u32 binding_index, vulkan_buffer_t* buffer)
{
	VkDescriptorBufferInfo buffer_info =
	{
		.buffer = buffer->handle,
		.offset = 0,
		.range = buffer->size
	};
	VkWriteDescriptorSet descriptor_write =
	{
		.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
		.dstSet = set->handle,
		.dstBinding = binding_index,
		.dstArrayElement = 0,
		.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		.descriptorCount = 1,
		.pBufferInfo = &buffer_info,
	};
	vkUpdateDescriptorSets(renderer->logical_device->handle, 1, &descriptor_write, 0, NULL);
}
