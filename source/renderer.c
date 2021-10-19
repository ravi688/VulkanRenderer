

#include <engine/renderer/renderer.h>
#include <exception/exception.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string/string.h>		//custom string library
#include <string.h>				//standard string library

#include <garbage_collector/garbage_collector.h>
#include <vulkan/vulkan_wrapper.h>

#include <exception/exception.h>
#include <debug.h>

#include <array/header_config.h>
#include <array/array.h>

#include <defines.h>

typedef struct 
{
	float position[2]; 
	float color[3];
} vertex_t;

typedef vertex_t* pvertex_t;
instantiate_tuple_t(uint32_t, pvertex_t);

typedef struct renderer_t
{
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
	VkFormat vk_format; 
	VkExtent2D vk_extent;
	VkPipelineLayout vk_pipeline_layout;
	VkRenderPass vk_render_pass;
	VkPipeline vk_pipeline;
	VkCommandPool vk_command_pool;
	VkSemaphore vk_image_available_semaphore;
	VkSemaphore vk_render_finished_semaphore;

	VkBuffer vk_vertex_buffer;
	VkDeviceMemory vk_vertex_memory;

	tuple_t(uint32_t, pvertex_t) vertices;
} renderer_t;


instantiate_array_struct(VkFormat); 
instantiate_declaration_array(VkFormat);
instantiate_implementation_array(VkFormat);

void* renderer_get_vulkan_instance(renderer_t* renderer) { return (void*)(&(renderer->vk_instance)); }
void* renderer_get_vulkan_device(renderer_t* renderer) { return (void*)(&(renderer->vk_device)); }
void* renderer_get_vulkan_surface(renderer_t* renderer) { return (void*)(&(renderer->vk_surface)); }
void* renderer_get_vulkan_swapchain(renderer_t* renderer) { return (void*)(&(renderer->vk_swapchain)); }

//TODO: Wrapp this physical device selection & creation of logical device into a single function
renderer_t* renderer_init()
{
	renderer_t* renderer = GC_NEW(renderer_t);
	renderer->vk_instance = vk_create_instance(); 
	renderer->vk_physical_device = vk_get_suitable_physical_device(vk_get_physical_devices(renderer->vk_instance));
	renderer->vk_device = vk_get_device(renderer->vk_physical_device);
	return renderer;
}

void renderer_init_surface(renderer_t* renderer, void* surface)
{
	//Surfaces and Swapchains
	renderer->vk_surface = *((VkSurfaceKHR*)surface);
	VkSwapchainCreateInfoKHR swapchainCreateInfo  = 
	{
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.minImageCount = 3, 
		.imageFormat = VK_FORMAT_B8G8R8A8_SRGB,
		.imageExtent = { 600, 500 }, 
		.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
		.imageArrayLayers = 1, 
		.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, 
		.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE, 
		.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR, 
		.presentMode = VK_PRESENT_MODE_MAILBOX_KHR, 
		.clipped = VK_TRUE,
		.surface = renderer->vk_surface,
		.oldSwapchain = VK_NULL_HANDLE
	};
	renderer->vk_swapchain = vk_get_swapchain(renderer->vk_device, &swapchainCreateInfo);
	renderer->vk_render_pass = vk_get_render_pass(renderer->vk_device, VK_FORMAT_B8G8R8A8_SRGB);
	renderer->vk_images = vk_get_images(renderer->vk_device, renderer->vk_swapchain);
	renderer->vk_image_views = vk_get_image_views(renderer->vk_device, VK_FORMAT_B8G8R8A8_SRGB, renderer->vk_images.value1, renderer->vk_images.value2);
	renderer->vk_framebuffers = vk_get_framebuffers(renderer->vk_device, 3, renderer->vk_render_pass, (VkExtent2D) { 600, 500 }, 1, renderer->vk_image_views.value2);
	renderer->vk_command_pool = vk_get_command_pool(renderer->vk_device, vk_get_graphics_queue_family_index(renderer->vk_physical_device)); 
	renderer->vk_command_buffers = vk_get_command_buffers(renderer->vk_device, renderer->vk_command_pool, 3);
	renderer->vk_image_available_semaphore = vk_get_semaphore(renderer->vk_device); 
	renderer->vk_render_finished_semaphore = vk_get_semaphore(renderer->vk_device);
	renderer->vk_graphics_queue = vk_get_device_queue(renderer->vk_device, vk_get_graphics_queue_family_index(renderer->vk_physical_device), 0);

	//Graphics Pipeline
	renderer->vk_pipeline_layout = vk_get_pipeline_layout(renderer->vk_device);

	VkPipelineShaderStageCreateInfo shaderStages[2] = 
	{
		vk_get_pipeline_shader_stage_create_info(vk_get_shader_module(renderer->vk_device, "shaders/vertexShader.vert"), VERTEX_SHADER, "main"),
		vk_get_pipeline_shader_stage_create_info(vk_get_shader_module(renderer->vk_device, "shaders/fragmentShader.frag"), FRAGMENT_SHADER, "main")
	};

	VkFormat formats[2] = { VK_FORMAT_R32G32_SFLOAT, VK_FORMAT_R32G32B32_SFLOAT };
	uint32_t offsets[2] = { offsetof(vertex_t, position), offsetof(vertex_t, color) };
	VkPipelineVertexInputStateCreateInfo vertexInputInfo = vk_get_pipeline_vertex_input_state_create_info(	3, 
																											sizeof(vertex_t), 
																											VK_VERTEX_INPUT_RATE_VERTEX, 
																											formats, 
																											offsets);
	VkPipelineInputAssemblyStateCreateInfo inputAssembly = vk_get_pipeline_input_assembly_state_create_info();
	VkPipelineRasterizationStateCreateInfo rasterizer = vk_get_pipeline_rasterization_state_create_info();
	VkPipelineViewportStateCreateInfo viewportState = vk_get_pipeline_viewport_state_create_info(600, 500);
	VkPipelineMultisampleStateCreateInfo multisampling = vk_get_pipeline_multisample_state_create_info();
	VkPipelineColorBlendStateCreateInfo colorBlending = vk_get_pipeline_color_blend_state_create_info();
	renderer->vk_pipeline = vk_get_graphics_pipeline(	renderer->vk_device, renderer->vk_pipeline_layout, renderer->vk_render_pass,
														shaderStages,
														&vertexInputInfo,
														&inputAssembly,
														&viewportState,
														&rasterizer,
														&multisampling,
														&colorBlending
													);
	//Vertex Buffer
	vertex_t vertices[3] = 
	{
		{ { 0.0f, 0.5f 	}, { 	0.0f, 0.0f, 1.0f 	} },
		{ { -0.5f, 0.0f }, {	0.0f, 1.0f, 0.0f 	} },
		{ { 0.5f, 0.0f 	}, { 	1.0f, 0.0f, 0.0f 	} }
	}; 
	renderer->vk_vertex_buffer = vk_get_buffer(renderer->vk_device, sizeof(vertices), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE);
	renderer->vk_vertex_memory = vk_get_device_memory_for_buffer(renderer->vk_device, renderer->vk_physical_device, renderer->vk_vertex_buffer, sizeof(vertices), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	void* data;
	vkMapMemory(renderer->vk_device, renderer->vk_vertex_memory, 0, sizeof(vertices), 0, &data);
	memcpy(data, vertices, sizeof(vertices));
	vkUnmapMemory(renderer->vk_device, renderer->vk_vertex_memory);


	//Recording commands
	for(uint32_t i = 0; i < 3; i++)
	{
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO }; 
		vkCall(vkBeginCommandBuffer(renderer->vk_command_buffers.value2[i], &beginInfo));
		VkClearValue clearValue;
		clearValue.color.float32[0] = 0;
		clearValue.color.float32[1] = 0.3f;
		clearValue.color.float32[2] = 1;
		clearValue.color.float32[3] = 1;
		VkRenderPassBeginInfo renderPassBeginInfo =  
		{
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
			.renderArea.offset = (VkOffset2D) { 0, 0 }, 
			.renderArea.extent = (VkExtent2D) { 600, 500 }, 
			.framebuffer = renderer->vk_framebuffers.value2[i], 
			.renderPass = renderer->vk_render_pass,
			.clearValueCount = 1,
			.pClearValues = &clearValue
		};
		vkCmdBeginRenderPass(renderer->vk_command_buffers.value2[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE); 
		vkCmdBindPipeline(renderer->vk_command_buffers.value2[i], VK_PIPELINE_BIND_POINT_GRAPHICS, renderer->vk_pipeline);
		VkBuffer vertexBuffers[1] =  { renderer->vk_vertex_buffer };
		VkDeviceSize offsets[1] = { 0 };
		vkCmdBindVertexBuffers(renderer->vk_command_buffers.value2[i], 0, 1, vertexBuffers, offsets);
		vkCmdDraw(renderer->vk_command_buffers.value2[i], 3, 1, 0, 0);
		vkCmdEndRenderPass(renderer->vk_command_buffers.value2[i]);
		vkCall(vkEndCommandBuffer(renderer->vk_command_buffers.value2[i]));
	}
}

void renderer_update(renderer_t* renderer)
{
	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(renderer->vk_device, renderer->vk_swapchain, UINT64_MAX, renderer->vk_image_available_semaphore, VK_NULL_HANDLE, &imageIndex);
	if(result == VK_ERROR_OUT_OF_DATE_KHR)
		log_err("VK_ERROR_OUT_OF_DATE_KHR");

	uint32_t waitDstMask = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	VkSubmitInfo submitInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.commandBufferCount = 1, 
		.pCommandBuffers = &(renderer->vk_command_buffers.value2[imageIndex]),
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &(renderer->vk_image_available_semaphore),
		.pWaitDstStageMask = &waitDstMask,
		.signalSemaphoreCount = 1, 
		.pSignalSemaphores = &(renderer->vk_render_finished_semaphore)
	}; 

	vkCall(vkQueueSubmit(renderer->vk_graphics_queue, 1, &submitInfo, VK_NULL_HANDLE));

	VkPresentInfoKHR presentInfo =  
	{
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.swapchainCount = 1, 
		.pSwapchains = &(renderer->vk_swapchain),
		.waitSemaphoreCount = 1, 
		.pWaitSemaphores = &(renderer->vk_render_finished_semaphore),
		.pImageIndices = &imageIndex
	};

	vkCall(vkQueuePresentKHR(renderer->vk_graphics_queue, &presentInfo));
}

void renderer_terminate(renderer_t* renderer)
{

}