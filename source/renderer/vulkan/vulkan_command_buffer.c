
#include <renderer/internal/vulkan/vulkan_defines.h>
#include <renderer/internal/vulkan/vulkan_command_buffer.h>


RENDERER_API VkCommandPool vulkan_command_pool_create(vulkan_renderer_t* renderer, u32 queueFamilyIndex, VkCommandPoolCreateFlags flags)
{
	VkCommandPoolCreateInfo command_pool_create_info =
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.queueFamilyIndex = queueFamilyIndex,
		.flags = flags
	};
	VkCommandPool pool;
	vkCall(vkCreateCommandPool(renderer->logical_device->handle, &command_pool_create_info, NULL, &pool));
	return pool;
}

RENDERER_API VkCommandBuffer vulkan_command_buffer_allocate(vulkan_renderer_t* renderer, VkCommandBufferLevel level, VkCommandPool pool)
{
	VkCommandBuffer buffer;
	vulkan_command_buffer_allocatev(renderer, level, pool, 1, &buffer);
	return buffer;
}

RENDERER_API void vulkan_command_buffer_allocatev(vulkan_renderer_t* renderer, VkCommandBufferLevel level, VkCommandPool pool, u32 count, VkCommandBuffer* out_buffers)
{
	VkCommandBufferAllocateInfo alloc_info = 
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.level = level,
		.commandPool = pool,
		.commandBufferCount = count
	};

	VkCommandBuffer buffer;
	vkCall(vkAllocateCommandBuffers(renderer->logical_device->handle, &alloc_info, out_buffers));
}

RENDERER_API void vulkan_command_buffer_begin(VkCommandBuffer buffer, VkCommandBufferUsageFlags flags)
{
	VkCommandBufferBeginInfo begin_info =
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.flags = flags,
	};
	vkCall(vkBeginCommandBuffer(buffer, &begin_info));
}
