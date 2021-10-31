//TODO: Write a numeric type conversion warning systme; because of this printf("%u", <float> value) => outputs zero if <float> value < 1

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

#include <math.h>

#include <vec2/header_config.h>
#include <vec2/vec2.h>

#include <vec3/header_config.h>
#include <vec3/vec3.h>

#include <vec4/header_config.h>
#include <vec4/vec4.h>

#include <memory/header_config.h>
#include <memory/memory.h>

#include <mat4/header_config.h>
#include <mat4/mat4.h>

#include <affine_transformation/header_config.h>
#include <affine_transformation/affine_transformation.h>


#define DEG2RAD 0.01745f
#define RAD2DEG 57.29577f


typedef struct 
{
	union
	{
		vec2_t(float) position;
		struct 
		{
			float x; 
			float y; 
		};
	};

	union 
	{
		vec3_t(float) color;
		struct
		{
			float r; 
			float g;
			float b;
		};
	};
} vertex2d_t;

typedef struct  
{
	vec3_t(float) position;
	vec3_t(float) color;
} vertex3d_t;

typedef vertex2d_t* pvertex_t;
instantiate_tuple_t(uint32_t, pvertex_t);
typedef VkPipelineShaderStageCreateInfo* pVkPipelineShaderStageCreateInfo;
instantiate_tuple_t(u32, pVkPipelineShaderStageCreateInfo);

typedef struct renderer_t
{
	uint32_t width; 
	uint32_t height;

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

	tuple_t(uint32_t, pvertex_t) vertices;
} renderer_t;


instantiate_array_struct(VkFormat); 
instantiate_declaration_array(VkFormat);
instantiate_implementation_array(VkFormat);

void* renderer_get_vulkan_instance(renderer_t* renderer) { return (void*)(&(renderer->vk_instance)); }
void* renderer_get_vulkan_device(renderer_t* renderer) { return (void*)(&(renderer->vk_device)); }
void* renderer_get_vulkan_surface(renderer_t* renderer) { return (void*)(&(renderer->vk_surface)); }
void* renderer_get_vulkan_swapchain(renderer_t* renderer) { return (void*)(&(renderer->vk_swapchain)); }

vec3_t(float) vec4_vec3(vec4_t(float) v)
{
	if(v.w == 0)
		return (vec3_t(float)) { v.x, v.y, v.z }; 
	else
	{
		float t = 1 / v.w;
		return (vec3_t(float))  { v.x * t, v.y * t, v.z * t };
	}
}

static mat4_t(float) mat4_transform(vec3_t(float) position, vec3_t(float) eulerRotation) { return mat4_mul(float)(2, mat4_translation(float)(position.x, position.y, position.z), mat4_rotation(float)(eulerRotation.x, eulerRotation.y, eulerRotation.z)); }


static void convert_to_vulkan_clipspace(vertex2d_t *  const vertices, u32 count)
{
	for(u32 i = 0; i < count; i++)
		vertices[i].position.y = -vertices[i].position.y;
}

static vertex2d_t* foreach_vertex3d(u32 count, vertex3d_t* vertices, mat4_t(float) m)
{
	vertex2d_t* vertices2d = GC_NEWV(vertex2d_t, count);
	for(u32 i = 0; i < count; i++)
	{
		vec3_t(float) v = vec4_vec3(mat4_mul_vec4(float)(m, vertices[i].position.x, vertices[i].position.y, vertices[i].position.z, 1));
		vertices2d[i].position.x = v.z; 
		vertices2d[i].position.y = v.y;

		//TODO: this should be like vertices2d[i].color = vertices[i].color
		//but compiler denies because of the array type; write a function array_copy(T, count)(vertices2d[i].color, vertices[i].color)
		vertices2d[i].color.x = vertices[i].color.x;
		vertices2d[i].color.y = vertices[i].color.y;
		vertices2d[i].color.z = vertices[i].color.z;
	}
	return vertices2d;
}

//TODO: Wrapp this physical device selection & creation of logical device into a single function
renderer_t* renderer_init()
{
	renderer_t* renderer = GC_NEW(renderer_t);
	renderer->vk_instance = vk_create_instance(); 
	renderer->vk_physical_device = vk_get_suitable_physical_device(vk_get_physical_devices(renderer->vk_instance));
	renderer->vk_device = vk_get_device(renderer->vk_physical_device);
	return renderer;
}

void renderer_init_surface(renderer_t* renderer, void* surface, uint32_t screen_width, uint32_t screen_height)
{
	//Surfaces and Swapchains
	renderer->vk_surface = *((VkSurfaceKHR*)surface);
	renderer->width = screen_width; 
	renderer->height = screen_height;
	VkSwapchainCreateInfoKHR swapchainCreateInfo  = 
	{
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.minImageCount = 3, 
		.imageFormat = VK_FORMAT_B8G8R8A8_SRGB,
		.imageExtent = { renderer->width, renderer->height }, 
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
	renderer->vk_framebuffers = vk_get_framebuffers(renderer->vk_device, 3, renderer->vk_render_pass, (VkExtent2D) { renderer->width, renderer->height }, 1, renderer->vk_image_views.value2);
	renderer->vk_command_pool = vk_get_command_pool(renderer->vk_device, vk_get_graphics_queue_family_index(renderer->vk_physical_device)); 
	renderer->vk_command_buffers = vk_get_command_buffers(renderer->vk_device, renderer->vk_command_pool, 3);
	renderer->vk_image_available_semaphore = vk_get_semaphore(renderer->vk_device); 
	renderer->vk_render_finished_semaphore = vk_get_semaphore(renderer->vk_device);
	renderer->vk_graphics_queue = vk_get_device_queue(renderer->vk_device, vk_get_graphics_queue_family_index(renderer->vk_physical_device), 0);

	//Graphics Pipeline
	renderer->vk_vertex_shader_module = vk_get_shader_module(renderer->vk_device, "shaders/vertexShader.vert");
	renderer->vk_fragment_shader_module = vk_get_shader_module(renderer->vk_device, "shaders/fragmentShader.frag");
	
	renderer->vk_pipeline_layout = vk_get_pipeline_layout(renderer->vk_device);
	renderer->vk_shader_stages.value1 = 2;
	renderer->vk_shader_stages.value2 = GC_NEWV(VkPipelineShaderStageCreateInfo, 2);
	VkPipelineShaderStageCreateInfo info0 = vk_get_pipeline_shader_stage_create_info(renderer->vk_vertex_shader_module, VERTEX_SHADER, "main");
	VkPipelineShaderStageCreateInfo info1 = vk_get_pipeline_shader_stage_create_info(renderer->vk_fragment_shader_module, FRAGMENT_SHADER, "main");
	memcpy(&(renderer->vk_shader_stages.value2[0]), &info0, sizeof(VkPipelineShaderStageCreateInfo));
	memcpy(&(renderer->vk_shader_stages.value2[1]), &info1, sizeof(VkPipelineShaderStageCreateInfo));
	VkFormat formats[2] = { VK_FORMAT_R32G32_SFLOAT, VK_FORMAT_R32G32B32_SFLOAT };
	uint32_t offsets[2] = { offsetof(vertex2d_t, position), offsetof(vertex2d_t, color) };
	VkPipelineVertexInputStateCreateInfo vertexInputInfo = vk_get_pipeline_vertex_input_state_create_info(	3, 
																											sizeof(vertex2d_t), 
																											VK_VERTEX_INPUT_RATE_VERTEX, 
																											formats, 
																											offsets);
	VkPipelineInputAssemblyStateCreateInfo inputAssembly = vk_get_pipeline_input_assembly_state_create_info();
	VkPipelineRasterizationStateCreateInfo rasterizer = vk_get_pipeline_rasterization_state_create_info();
	VkPipelineViewportStateCreateInfo viewportState = vk_get_pipeline_viewport_state_create_info(renderer->width, renderer->height);
	VkPipelineMultisampleStateCreateInfo multisampling = vk_get_pipeline_multisample_state_create_info();
	VkPipelineColorBlendStateCreateInfo colorBlending = vk_get_pipeline_color_blend_state_create_info();
	renderer->vk_pipeline = vk_get_graphics_pipeline(	renderer->vk_device, renderer->vk_pipeline_layout, renderer->vk_render_pass,
														renderer->vk_shader_stages.value2,
														&vertexInputInfo,
														&inputAssembly,
														&viewportState,
														&rasterizer,
														&multisampling,
														&colorBlending
													);
	//Geometry Data
	// vertex3d_t geometry3D[4] = 
	// {
	// 	{ { 0.5f, 0,  0.5f }, { 1, 1, 1 } }, 
	//  	{ { 0.5f, 0,  -0.5f }, { 1, 0, 0} },
	//  	{ { -0.5f, 0,  -0.5f }, { 0, 0, 0 } },
	//  	{ { -0.5f, 0, 0.5f }, { 0, 0,0 } }
	// }; 

	vertex3d_t geometry3D[] = 
	{
		 //Bottom
		 { { -0.5f, 0, 0.5f },  { 0.1f, 0.7f, 0 } },
		 { { -0.5f, 0, -0.5f },  { 0.1f, 0.7f, 0 } },
		 { { 0.5f, 0, -0.5f },  { 0.1f, 0.7f, 0 } },
		 { { 0.5f, 0, 0.5f },  { 0.1f, 0.7f, 0 } },

		 //Top
		 { { 0.5f, 1, 0.5f },  { 0.4f, 0.2f, 1 } },
		 { { 0.5f, 1, -0.5f },  { 0.4f, 0.2f, 1 } },
		 { { -0.5f, 1, -0.5f },  { 0.4f, 0.2f, 1 } },
		 { { -0.5f, 1, 0.5f },  { 0.4f, 0.2f, 1 } },

		 //Front
		 { { -0.5f, 1, -0.5f },  { 0, 0, 1 } },
		 { { -0.5f, 1, 0.5f },  { 0, 0, 1 } },
		 { { -0.5f, 0, 0.5f },  { 0, 0, 1 } },
		 { { -0.5f, 0, -0.5f },  { 0, 0, 1 } },

		 //Left
		 { { 0.5f, 1, -0.5f },  { 0.1f, 0, 0.4f } },
		 { { -0.5f, 1, -0.5f },  { 0.1f, 0, 0.4f } },
		 { { -0.5f, 0, -0.5f },  { 0.1f, 0, 0.4f } },
		 { { 0.5f, 0, -0.5f },  { 0.1f, 0, 0.4f } },

		 //Right
		 { { -0.5f, 1, 0.5f },  { 0.1f, 0, 0.4f } },
		 { { 0.5f, 1, 0.5f },  { 0.1f, 0, 0.4f } },
		 { { 0.5f, 0, 0.5f },  { 0.1f, 0, 0.4f } },
		 { { -0.5f, 0, 0.5f },  { 0.1f, 0, 0.4f } },

		 //Back
		 { { 0.5f, 1, 0.5f },  { 1, 0, 0.3 } },
		 { { 0.5f, 1, -0.5f }, { 1, 0, 0.3 } },
		 { { 0.5f, 0, -0.5f }, { 1, 0, 0.3 } },
		 { { 0.5f, 0, 0.5f },  { 1, 0, 0.3 } },
	};

	u32 vertexCount = sizeof(geometry3D) / sizeof(vertex3d_t);
	mat4_t(float) cameraTransform = mat4_transform((vec3_t(float)) { -3, 2, 0 }, (vec3_t(float)) { 0, 0, -30 * DEG2RAD } );
	mat4_t(float) viewMatrix = mat4_inverse(float)(cameraTransform);
	mat4_t(float) modelMatrix = mat4_transform((vec3_t(float)) { 0, 0, 0 }, (vec3_t(float)) { 0, -30 * DEG2RAD, 0 });
	mat4_t(float) projectionMatrix = mat4_persp_projection(float)(0, 10, 65 * DEG2RAD, (float)renderer->width/renderer->height);
	vertex2d_t* vertices = foreach_vertex3d(vertexCount, geometry3D, mat4_mul(float)(3, projectionMatrix, viewMatrix, modelMatrix));

	convert_to_vulkan_clipspace(vertices, vertexCount); 
	u32 indices[] = 
	{
		//clockwise order
		2, 1, 0, 3, 2, 0, 
		6, 5, 4, 7, 6, 4, 
		8, 9, 10, 8, 10, 11, 
		12, 13, 14, 12, 14, 15, 
		16, 17, 18, 16, 18, 19, 
		20, 21, 22, 20, 22, 23
	}; 
	u32 indexCount = sizeof(indices) / sizeof(u32);

	//Staging buffer preparation
	renderer->vk_staging_buffer = vk_get_buffer(renderer->vk_device, vertexCount * sizeof(vertex2d_t), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_SHARING_MODE_EXCLUSIVE); 
	renderer->vk_staging_memory = vk_get_device_memory_for_buffer(renderer->vk_device, renderer->vk_physical_device, renderer->vk_staging_buffer, vertexCount * sizeof(vertex2d_t), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	void* data;
	vkMapMemory(renderer->vk_device, renderer->vk_staging_memory, 0, vertexCount * sizeof(vertex2d_t), 0, &data);
	memcpy(data, vertices, sizeof(vertex2d_t) * vertexCount);
	vkUnmapMemory(renderer->vk_device, renderer->vk_staging_memory);


	//Vertex Buffer
	renderer->vk_vertex_buffer = vk_get_buffer(renderer->vk_device, vertexCount * sizeof(vertex2d_t), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_SHARING_MODE_EXCLUSIVE);
	renderer->vk_vertex_memory = vk_get_device_memory_for_buffer(renderer->vk_device, renderer->vk_physical_device, renderer->vk_vertex_buffer, vertexCount * sizeof(vertex2d_t), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);


	//Recording commands to copy the staging buffer to vertex buffer
	VkCommandBufferAllocateInfo allocInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO, 
		.commandPool = renderer->vk_command_pool, 
		.commandBufferCount = 1, 
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY
	}; 
	VkCommandBuffer transferCommandBuffer;
	vkCall(vkAllocateCommandBuffers(renderer->vk_device, &allocInfo, &transferCommandBuffer));

	VkCommandBufferBeginInfo beginInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT 
	}; 

	vkCall(vkBeginCommandBuffer(transferCommandBuffer, &beginInfo));
	VkBufferCopy copyRegion = 
	{
		.srcOffset = 0, 
		.dstOffset = 0, 
		.size = vertexCount * sizeof(vertex2d_t)
	}; 

	vkCmdCopyBuffer(transferCommandBuffer, renderer->vk_staging_buffer, renderer->vk_vertex_buffer, 1, &copyRegion); 
	vkCall(vkEndCommandBuffer(transferCommandBuffer));

	VkSubmitInfo submitInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO, 
		.commandBufferCount = 1, 
		.pCommandBuffers = &transferCommandBuffer,
	}; 
	vkQueueSubmit(renderer->vk_graphics_queue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(renderer->vk_graphics_queue);
	//Destroy Staging Buffer
	vkDestroyBuffer(renderer->vk_device, renderer->vk_staging_buffer, NULL); 
	//Free the staging buffer memory 
	vkFreeMemory(renderer->vk_device, renderer->vk_staging_memory, NULL); 
	//Destroy the transfer Command buffer
	vkFreeCommandBuffers(renderer->vk_device, renderer->vk_command_pool, 1, &transferCommandBuffer);


	//TODO: We can record the copy buffer commands of vertex and index buffers both in a record pass ^^ above
	renderer->vk_staging_buffer = vk_get_buffer(renderer->vk_device, sizeof(indices), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_SHARING_MODE_EXCLUSIVE); 
	renderer->vk_staging_memory = vk_get_device_memory_for_buffer(renderer->vk_device, renderer->vk_physical_device, renderer->vk_staging_buffer, sizeof(indices), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT); 

	vkMapMemory(renderer->vk_device, renderer->vk_staging_memory, 0, sizeof(indices), 0, &data);
	memcpy(data, indices, sizeof(indices)); 
	vkUnmapMemory(renderer->vk_device, renderer->vk_staging_memory);

	renderer->vk_index_buffer = vk_get_buffer(renderer->vk_device, sizeof(indices), VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE); 
	renderer->vk_index_memory = vk_get_device_memory_for_buffer(renderer->vk_device, renderer->vk_physical_device, renderer->vk_index_buffer, sizeof(indices), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);


	vkCall(vkAllocateCommandBuffers(renderer->vk_device, &allocInfo, &transferCommandBuffer));
	vkCall(vkBeginCommandBuffer(transferCommandBuffer, &beginInfo));
	copyRegion.size = sizeof(indices); 
	vkCmdCopyBuffer(transferCommandBuffer, renderer->vk_staging_buffer, renderer->vk_index_buffer, 1, &copyRegion);
	vkCall(vkEndCommandBuffer(transferCommandBuffer)); 
	vkQueueSubmit(renderer->vk_graphics_queue, 1, &submitInfo, VK_NULL_HANDLE); 
	vkQueueWaitIdle(renderer->vk_graphics_queue); 
	vkDestroyBuffer(renderer->vk_device, renderer->vk_staging_buffer, NULL); 
	vkFreeMemory(renderer->vk_device, renderer->vk_staging_memory, NULL); 
	vkFreeCommandBuffers(renderer->vk_device, renderer->vk_command_pool, 1, &transferCommandBuffer);


	//Recording commands
	for(uint32_t i = 0; i < 3; i++)
	{
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO }; 
		vkCall(vkBeginCommandBuffer(renderer->vk_command_buffers.value2[i], &beginInfo));
		VkClearValue clearValue;
		clearValue.color.float32[0] = 0;
		clearValue.color.float32[1] = 0;
		clearValue.color.float32[2] = 0;
		clearValue.color.float32[3] = 1;
		VkRenderPassBeginInfo renderPassBeginInfo =  
		{
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
			.renderArea.offset = (VkOffset2D) { 0, 0 }, 
			.renderArea.extent = (VkExtent2D) { renderer->width, renderer->height }, 
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
		vkCmdBindIndexBuffer(renderer->vk_command_buffers.value2[i], renderer->vk_index_buffer, 0, VK_INDEX_TYPE_UINT32);
		vkCmdDrawIndexed(renderer->vk_command_buffers.value2[i], sizeof(indices), 1, 0, 0, 0);
		// vkCmdDraw(renderer->vk_command_buffers.value2[i], vertexCount, 1, 0, 0);
		vkCmdEndRenderPass(renderer->vk_command_buffers.value2[i]);
		vkCall(vkEndCommandBuffer(renderer->vk_command_buffers.value2[i]));
	}
}

void renderer_update(renderer_t* renderer)
{
	uint32_t imageIndex;
	vkAcquireNextImageKHR(renderer->vk_device, renderer->vk_swapchain, UINT64_MAX, renderer->vk_image_available_semaphore, VK_NULL_HANDLE, &imageIndex);
	
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

	vkQueueSubmit(renderer->vk_graphics_queue, 1, &submitInfo, VK_NULL_HANDLE);

	VkPresentInfoKHR presentInfo =  
	{
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.swapchainCount = 1, 
		.pSwapchains = &(renderer->vk_swapchain),
		.waitSemaphoreCount = 1, 
		.pWaitSemaphores = &(renderer->vk_render_finished_semaphore),
		.pImageIndices = &imageIndex
	};

	vkQueuePresentKHR(renderer->vk_graphics_queue, &presentInfo);
}

static void renderer_destroy_swapchain(renderer_t* renderer);
static void renderer_create_swapchain(renderer_t* renderer);

void renderer_on_window_resize(renderer_t* renderer, u32 width, u32 height)
{
	log_msg("Window is resized: %u, %u", width, height);
	renderer_destroy_swapchain(renderer); 
	renderer->width = width;
	renderer->height = height;
	renderer_create_swapchain(renderer);
}

static void renderer_destroy_swapchain(renderer_t* renderer)
{
	vkDestroySwapchainKHR(renderer->vk_device, renderer->vk_swapchain, NULL);
	for(u32 i = 0; i < 3; i++)
		vkDestroyFramebuffer(renderer->vk_device, renderer->vk_framebuffers.value2[i], NULL);
	for(u32 i = 0; i < 3; i++)
		vkDestroyImageView(renderer->vk_device, renderer->vk_image_views.value2[i], NULL);
	
	//NOTE: VkImage(s) were already destroyed by vkDestroySwapchainKHR
	// for(u32 i = 0; i < 3; i++)
	// 	vkDestroyImage(renderer->vk_device, renderer->vk_images.value2[i], NULL);

	vkDestroyRenderPass(renderer->vk_device, renderer->vk_render_pass, NULL);
	vkDestroyPipelineLayout(renderer->vk_device, renderer->vk_pipeline_layout, NULL);
	vkDestroyPipeline(renderer->vk_device, renderer->vk_pipeline, NULL);
	vkFreeCommandBuffers(renderer->vk_device, renderer->vk_command_pool, renderer->vk_command_buffers.value1, renderer->vk_command_buffers.value2);	
}

static void renderer_create_swapchain(renderer_t* renderer)
{
	VkSwapchainCreateInfoKHR swapchainCreateInfo  = 
	{
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.minImageCount = 3, 
		.imageFormat = VK_FORMAT_B8G8R8A8_SRGB,
		.imageExtent = { renderer->width, renderer->height }, 
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
	renderer->vk_framebuffers = vk_get_framebuffers(renderer->vk_device, 3, renderer->vk_render_pass, (VkExtent2D) { renderer->width, renderer->height }, 1, renderer->vk_image_views.value2);	
	renderer->vk_command_buffers = vk_get_command_buffers(renderer->vk_device, renderer->vk_command_pool, 3);

	renderer->vk_pipeline_layout = vk_get_pipeline_layout(renderer->vk_device);
	renderer->vk_shader_stages.value1 = 2;
	renderer->vk_shader_stages.value2 = GC_NEWV(VkPipelineShaderStageCreateInfo, 2);
	VkPipelineShaderStageCreateInfo info0 = vk_get_pipeline_shader_stage_create_info(renderer->vk_vertex_shader_module, VERTEX_SHADER, "main");
	VkPipelineShaderStageCreateInfo info1 = vk_get_pipeline_shader_stage_create_info(renderer->vk_fragment_shader_module, FRAGMENT_SHADER, "main");
	memcpy(&(renderer->vk_shader_stages.value2[0]), &info0, sizeof(VkPipelineShaderStageCreateInfo));
	memcpy(&(renderer->vk_shader_stages.value2[1]), &info1, sizeof(VkPipelineShaderStageCreateInfo));
	VkFormat formats[2] = { VK_FORMAT_R32G32_SFLOAT, VK_FORMAT_R32G32B32_SFLOAT };
	uint32_t offsets[2] = { offsetof(vertex2d_t, position), offsetof(vertex2d_t, color) };
	VkPipelineVertexInputStateCreateInfo vertexInputInfo = vk_get_pipeline_vertex_input_state_create_info(	3, 
																											sizeof(vertex2d_t), 
																											VK_VERTEX_INPUT_RATE_VERTEX, 
																											formats, 
																											offsets);
	VkPipelineInputAssemblyStateCreateInfo inputAssembly = vk_get_pipeline_input_assembly_state_create_info();
	VkPipelineRasterizationStateCreateInfo rasterizer = vk_get_pipeline_rasterization_state_create_info();
	VkPipelineViewportStateCreateInfo viewportState = vk_get_pipeline_viewport_state_create_info(renderer->width, renderer->height);
	VkPipelineMultisampleStateCreateInfo multisampling = vk_get_pipeline_multisample_state_create_info();
	VkPipelineColorBlendStateCreateInfo colorBlending = vk_get_pipeline_color_blend_state_create_info();
	renderer->vk_pipeline = vk_get_graphics_pipeline(	renderer->vk_device, renderer->vk_pipeline_layout, renderer->vk_render_pass,
														renderer->vk_shader_stages.value2,
														&vertexInputInfo,
														&inputAssembly,
														&viewportState,
														&rasterizer,
														&multisampling,
														&colorBlending
													);
	u32 indices[] = 
	{
		//clockwise order
		2, 1, 0, 3, 2, 0, 
		6, 5, 4, 7, 6, 4, 
		8, 9, 10, 8, 10, 11, 
		12, 13, 14, 12, 14, 15, 
		16, 17, 18, 16, 18, 19, 
		20, 21, 22, 20, 22, 23
	}; 
		//Recording commands
	for(uint32_t i = 0; i < 3; i++)
	{
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO }; 
		vkCall(vkBeginCommandBuffer(renderer->vk_command_buffers.value2[i], &beginInfo));
		VkClearValue clearValue;
		clearValue.color.float32[0] = 0;
		clearValue.color.float32[1] = 0;
		clearValue.color.float32[2] = 0;
		clearValue.color.float32[3] = 1;
		VkRenderPassBeginInfo renderPassBeginInfo =  
		{
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
			.renderArea.offset = (VkOffset2D) { 0, 0 }, 
			.renderArea.extent = (VkExtent2D) { renderer->width, renderer->height }, 
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
		vkCmdBindIndexBuffer(renderer->vk_command_buffers.value2[i], renderer->vk_index_buffer, 0, VK_INDEX_TYPE_UINT32);
		vkCmdDrawIndexed(renderer->vk_command_buffers.value2[i], sizeof(indices), 1, 0, 0, 0);
		// vkCmdDraw(renderer->vk_command_buffers.value2[i], vertexCount, 1, 0, 0);
		vkCmdEndRenderPass(renderer->vk_command_buffers.value2[i]);
		vkCall(vkEndCommandBuffer(renderer->vk_command_buffers.value2[i]));
	}
}

void renderer_terminate(renderer_t* renderer)
{
	renderer_destroy_swapchain(renderer);
	vkDestroySurfaceKHR(renderer->vk_instance, renderer->vk_surface, NULL);
	vkDestroyShaderModule(renderer->vk_device, renderer->vk_vertex_shader_module, NULL);
	vkDestroyShaderModule(renderer->vk_device, renderer->vk_fragment_shader_module, NULL);

	

	vkFreeMemory(renderer->vk_device, renderer->vk_vertex_memory, NULL);
	vkFreeMemory(renderer->vk_device, renderer->vk_index_memory, NULL);
	vkDestroyBuffer(renderer->vk_device, renderer->vk_vertex_buffer, NULL);
	vkDestroyBuffer(renderer->vk_device, renderer->vk_index_buffer, NULL);
	
	vkDestroySemaphore(renderer->vk_device, renderer->vk_render_finished_semaphore, NULL);
	vkDestroySemaphore(renderer->vk_device, renderer->vk_image_available_semaphore, NULL);

	vkDestroyCommandPool(renderer->vk_device, renderer->vk_command_pool, NULL);

	//Instance and Device are destroyed at last
	vkDestroyDevice(renderer->vk_device, NULL);
	vkDestroyInstance(renderer->vk_instance, NULL);
}
