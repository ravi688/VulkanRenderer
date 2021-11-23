
#pragma once

#include <vulkan/vulkan.h>

#include <tuple/header_config.h>
#include <tuple/tuple.h>

typedef struct render_window_t render_window_t;
typedef VkPipelineShaderStageCreateInfo* pVkPipelineShaderStageCreateInfo;
instantiate_tuple_t(u32, pVkPipelineShaderStageCreateInfo);


typedef struct renderer_t
{
	uint32_t width; 
	uint32_t height;

	render_window_t* window;

	VkInstance vk_instance;
	VkPhysicalDevice vk_physical_device;
	VkDevice vk_device;
	VkQueue vk_graphics_queue; 
	VkSurfaceKHR vk_surface;
	VkSwapchainKHR vk_swapchain;
	uint32_t vk_graphics_queue_index;
	tuple_t(uint32_t, pVkImage_t) vk_images;
	tuple_t(uint32_t, pVkImageView_t) vk_image_views;
	tuple_t(uint32_t, pVkFramebuffer_t) vk_framebuffers;
	tuple_t(uint32_t, pVkCommandBuffer_t) vk_command_buffers;
	tuple_t(u32, pVkPipelineShaderStageCreateInfo) vk_shader_stages;
	VkFormat vk_format; 
	VkExtent2D vk_extent;
	VkPipelineLayout vk_pipeline_layout;
	VkRenderPass vk_render_pass;
	VkPipeline vk_pipeline;
	VkCommandPool vk_command_pool;
	VkSemaphore vk_image_available_semaphore;
	VkSemaphore vk_render_finished_semaphore;

	//Vertex Buffer
	VkBuffer vk_vertex_buffer;
	VkDeviceMemory vk_vertex_memory;

	//Vertex Staging Buffer
	VkBuffer vk_staging_buffer; 
	VkDeviceMemory vk_staging_memory;

	//Index Buffer
	VkBuffer vk_index_buffer; 
	VkDeviceMemory vk_index_memory;

	//Shader Modules
	VkShaderModule vk_vertex_shader_module; 
	VkShaderModule vk_fragment_shader_module;

} renderer_t;
