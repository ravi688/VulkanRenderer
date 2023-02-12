
#include <renderer/internal/vulkan/vulkan_defines.h>
#include <renderer/internal/vulkan/vulkan_command_buffer.h>
#include <renderer/internal/vulkan/vulkan_allocator.h>
#include <renderer/debug.h>

RENDERER_API VkCommandPool vulkan_command_pool_create(vulkan_renderer_t* renderer, u32 queueFamilyIndex, VkCommandPoolCreateFlags flags)
{
	VkCommandPoolCreateInfo command_pool_create_info =
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.queueFamilyIndex = queueFamilyIndex,
		.flags = flags
	};
	VkCommandPool pool;
	vkCall(vkCreateCommandPool(renderer->logical_device->vo_handle, &command_pool_create_info, VULKAN_ALLOCATION_CALLBACKS(renderer), &pool));
	log_msg("CommandPool for the queueFamilyIndex: %u is created successfully\n", queueFamilyIndex);
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
	vkCall(vkAllocateCommandBuffers(renderer->logical_device->vo_handle, &alloc_info, out_buffers));
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

RENDERER_API void vulkan_command_buffer_reset(VkCommandBuffer buffer, VkCommandBufferResetFlags flags) 
{ 
	vkCall(vkResetCommandBuffer(buffer, flags)); 
}

RENDERER_API void vulkan_command_buffer_end(VkCommandBuffer buffer) 
{ 
	vkCall(vkEndCommandBuffer(buffer)); 
}

