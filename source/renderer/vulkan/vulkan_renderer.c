//TODO: Write a numeric type conversion warning systme; because of this printf("%u", <float> value) => outputs zero if <float> value < 1

#include <renderer/renderer.h>
#include <renderer/render_window.h>
#include <renderer/debug.h>
#include <renderer/defines.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_swapchain.h>
#include <memory_allocator/memory_allocator.h>

#include <exception/exception.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string/string.h>		//custom string library
#include <string.h>				//standard string library

#include <math.h>
#include <hpml/memory/header_config.h>
#include <hpml/memory/memory.h>
#include <hpml/vec2/header_config.h>
#include <hpml/vec2/vec2.h>
#include <hpml/vec3/header_config.h>
#include <hpml/vec3/vec3.h>
#include <hpml/vec4/header_config.h>
#include <hpml/vec4/vec4.h>
#include <hpml/mat4/header_config.h>
#include <hpml/mat4/mat4.h>
#include <hpml/affine_transformation/header_config.h>
#include <hpml/affine_transformation/affine_transformation.h>

// #define DEG2RAD 0.01745f
// #define RAD2DEG 57.29577f


// typedef struct
// {
// 	union
// 	{
// 		vec2_t(float) position;
// 		struct
// 		{
// 			float x;
// 			float y;
// 		};
// 	};

// 	union
// 	{
// 		vec3_t(float) color;
// 		struct
// 		{
// 			float r;
// 			float g;
// 			float b;
// 		};
// 	};
// } vertex2d_t;


// typedef struct
// {
// 	vec3_t(float) position;
// 	vec3_t(float) color;
// } vertex3d_t;

// instantiate_stack_array(vertex3d_t);

// vec3_t(float) vec4_vec3(vec4_t(float) v)
// {
// 	if(v.w == 0)
// 		return (vec3_t(float)) { v.x, v.y, v.z };
// 	else
// 	{
// 		float t = 1 / v.w;
// 		return (vec3_t(float))  { v.x * t, v.y * t, v.z * t };
// 	}
// }

// static mat4_t(float) mat4_transform(vec3_t(float) position, vec3_t(float) eulerRotation) { return mat4_mul(float)(2, mat4_translation(float)(position.x, position.y, position.z), mat4_rotation(float)(eulerRotation.x, eulerRotation.y, eulerRotation.z)); }


// static void convert_to_vulkan_clipspace(vertex2d_t *  const vertices, u32 count)
// {
// 	for(u32 i = 0; i < count; i++)
// 		vertices[i].position.y = -vertices[i].position.y;
// }

// static vertex2d_t* foreach_vertex3d(u32 count, vertex3d_t* vertices, mat4_t(float) m)
// {
// 	vertex2d_t* vertices2d = heap_newv(vertex2d_t, count);
// 	for(u32 i = 0; i < count; i++)
// 	{
// 		vertex3d_t vertex = ref(vertex3d_t, vertices, i);
// 		vec3_t(float) v = vec4_vec3(mat4_mul_vec4(float)(m, vertex.position.x, vertex.position.y, vertex.position.z, 1));
// 		ref(vertex2d_t, vertices2d, i).position.x = v.z;
// 		ref(vertex2d_t, vertices2d, i).position.y = v.y;

// 		//TODO: this should be like vertices2d[i].color = vertices[i].color
// 		//but compiler denies because of the array type; write a function array_copy(T, count)(vertices2d[i].color, vertices[i].color)
// 		ref(vertex2d_t, vertices2d, i).color.x = vertices[i].color.x;
// 		ref(vertex2d_t, vertices2d, i).color.y = vertices[i].color.y;
// 		ref(vertex2d_t, vertices2d, i).color.z = vertices[i].color.z;
// 	}
// 	return vertices2d;
// }
static void renderer_on_window_resize(render_window_t* window);

//TODO: Wrapp this physical device selection & creation of logical device into a single function
renderer_t* renderer_init(u32 width, u32 height, const char* title)
{
	renderer_t* renderer = heap_new(renderer_t);
	memset(renderer, 0, sizeof(renderer_t));

	//Create vulkan context
	renderer->vk_instance = vk_create_instance();
	renderer->vk_physical_device = vk_get_suitable_physical_device(vk_get_physical_devices(renderer->vk_instance));
	renderer->vk_device = vk_get_device(renderer->vk_physical_device);

	//Create render window
	renderer->window = render_window_init(width, height, title);
	renderer->window->user_data = renderer;
	render_window_subscribe_on_resize(renderer->window, renderer_on_window_resize);

	//Create Renderpass
	renderer->vk_render_pass = vk_get_render_pass(renderer->vk_device, VK_FORMAT_B8G8R8A8_SRGB);

	//Create Swapchain
	renderer->swapchain = vulkan_swapchain_create(renderer->window, renderer);

	//Create command buffers
	renderer->vk_command_pool = vk_get_command_pool(renderer->vk_device, vk_get_graphics_queue_family_index(renderer->vk_physical_device));
	renderer->vk_command_buffers = vk_get_command_buffers(renderer->vk_device, renderer->vk_command_pool, renderer->swapchain->image_count);

	//Set up graphics queue
	renderer->vk_graphics_queue = vk_get_device_queue(renderer->vk_device, vk_get_graphics_queue_family_index(renderer->vk_physical_device), 0);

	return renderer;
}


void renderer_begin_frame(renderer_t* renderer, float r, float g, float b, float a)
{
	vkQueueWaitIdle(renderer->vk_graphics_queue);
	vulkan_swapchain_acquire_next_image(renderer->swapchain, renderer);
	vkCall(vkResetCommandBuffer(renderer->vk_command_buffers.value2[renderer->swapchain->current_image_index], VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT));
	VkCommandBufferBeginInfo begin_info = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
	vkCall(vkBeginCommandBuffer(renderer->vk_command_buffers.value2[renderer->swapchain->current_image_index], &begin_info));

	VkClearValue clear_value;
	clear_value.color.float32[0] = r;
	clear_value.color.float32[1] = g;
	clear_value.color.float32[2] = b;
	clear_value.color.float32[3] = a;
	VkRenderPassBeginInfo render_pass_begin_info =
	{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.renderArea.offset = (VkOffset2D) { 0, 0 },
		.renderArea.extent = (VkExtent2D) { renderer->window->width, renderer->window->height },
		.framebuffer = renderer->swapchain->framebuffers[renderer->swapchain->current_image_index],
		.renderPass = renderer->vk_render_pass,
		.clearValueCount = 1,
		.pClearValues = &clear_value
	};

	vkCmdBeginRenderPass(renderer->vk_command_buffers.value2[renderer->swapchain->current_image_index], &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
}

void renderer_end_frame(renderer_t* renderer)
{
	vkCmdEndRenderPass(renderer->vk_command_buffers.value2[renderer->swapchain->current_image_index]);
	vkCall(vkEndCommandBuffer(renderer->vk_command_buffers.value2[renderer->swapchain->current_image_index]));
	uint32_t wait_destination_mask = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	VkSubmitInfo submit_info =
	{
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.commandBufferCount = 1,
		.pCommandBuffers = &(renderer->vk_command_buffers.value2[renderer->swapchain->current_image_index]),
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &(renderer->swapchain->image_available_semaphore),
		.pWaitDstStageMask = &wait_destination_mask,
		.signalSemaphoreCount = 1,
		.pSignalSemaphores = &(renderer->swapchain->render_finished_semaphore)
	};

	vkQueueSubmit(renderer->vk_graphics_queue, 1, &submit_info, VK_NULL_HANDLE);

	VkPresentInfoKHR present_info =
	{
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.swapchainCount = 1,
		.pSwapchains = &(renderer->swapchain->swapchain),
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &(renderer->swapchain->render_finished_semaphore),
		.pImageIndices = &(renderer->swapchain->current_image_index)
	};

	vkQueuePresentKHR(renderer->vk_graphics_queue, &present_info);
}

void renderer_update(renderer_t* renderer)
{
	render_window_poll_events(renderer->window);
}

bool renderer_is_running(renderer_t* renderer)
{
	return !render_window_should_close(renderer->window);
}

void renderer_terminate(renderer_t* renderer)
{
	render_window_destroy(renderer->window);
	vulkan_swapchain_destroy(renderer->swapchain, renderer);

	vkDestroyRenderPass(renderer->vk_device, renderer->vk_render_pass, NULL);
	vkFreeCommandBuffers(renderer->vk_device, renderer->vk_command_pool, renderer->swapchain->image_count, renderer->vk_command_buffers.value2);
	vkDestroyCommandPool(renderer->vk_device, renderer->vk_command_pool, NULL);
	vkDestroyDevice(renderer->vk_device, NULL);
	vkDestroyInstance(renderer->vk_instance, NULL);

	heap_free(renderer);
}

static void renderer_on_window_resize(render_window_t* window)
{
	renderer_t* renderer = window->user_data;
	log_msg("Window is resized: %u, %u\n", window->width, window->height);
	vulkan_swapchain_refresh(renderer->swapchain, renderer);
}


// static void renderer_init_surface(renderer_t* renderer, void* surface, uint32_t screen_width, uint32_t screen_height)
// {

// 	u32 vertexCount = 24;
// 	vertex3d_t* geometry3D = stack_array(vertex3d_t, vertexCount,
// 		 //Bottom
// 		 (vertex3d_t) { { -0.5f, 0, 0.5f },  { 0.1f, 0.7f, 0 } },
// 		 (vertex3d_t) { { -0.5f, 0, -0.5f },  { 0.1f, 0.7f, 0 } },
// 		 (vertex3d_t) { { 0.5f, 0, -0.5f },  { 0.1f, 0.7f, 0 } },
// 		 (vertex3d_t) { { 0.5f, 0, 0.5f },  { 0.1f, 0.7f, 0 } },

// 		 //Top
// 		 (vertex3d_t) { { 0.5f, 1, 0.5f },  { 0.4f, 0.2f, 1 } },
// 		 (vertex3d_t) { { 0.5f, 1, -0.5f },  { 0.4f, 0.2f, 1 } },
// 		 (vertex3d_t) { { -0.5f, 1, -0.5f },  { 0.4f, 0.2f, 1 } },
// 		 (vertex3d_t) { { -0.5f, 1, 0.5f },  { 0.4f, 0.2f, 1 } },

// 		 //Front
// 		 (vertex3d_t) { { -0.5f, 1, -0.5f },  { 0, 0, 1 } },
// 		 (vertex3d_t) { { -0.5f, 1, 0.5f },  { 0, 0, 1 } },
// 		 (vertex3d_t) { { -0.5f, 0, 0.5f },  { 0, 0, 1 } },
// 		 (vertex3d_t) { { -0.5f, 0, -0.5f },  { 0, 0, 1 } },

// 		 //Left
// 		 (vertex3d_t) { { 0.5f, 1, -0.5f },  { 0.1f, 0, 0.4f } },
// 		 (vertex3d_t) { { -0.5f, 1, -0.5f },  { 0.1f, 0, 0.4f } },
// 		 (vertex3d_t) { { -0.5f, 0, -0.5f },  { 0.1f, 0, 0.4f } },
// 		 (vertex3d_t) { { 0.5f, 0, -0.5f },  { 0.1f, 0, 0.4f } },

// 		 //Right
// 		 (vertex3d_t) { { -0.5f, 1, 0.5f },  { 0.1f, 0, 0.4f } },
// 		 (vertex3d_t) { { 0.5f, 1, 0.5f },  { 0.1f, 0, 0.4f } },
// 		 (vertex3d_t) { { 0.5f, 0, 0.5f },  { 0.1f, 0, 0.4f } },
// 		 (vertex3d_t) { { -0.5f, 0, 0.5f },  { 0.1f, 0, 0.4f } },

// 		 //Back
// 		 (vertex3d_t) { { 0.5f, 1, 0.5f },  { 1, 0, 0.3 } },
// 		 (vertex3d_t) { { 0.5f, 1, -0.5f }, { 1, 0, 0.3 } },
// 		 (vertex3d_t) { { 0.5f, 0, -0.5f }, { 1, 0, 0.3 } },
// 		 (vertex3d_t) { { 0.5f, 0, 0.5f },  { 1, 0, 0.3 } }
// 	);

// 	mat4_t(float) cameraTransform = mat4_transform((vec3_t(float)) { -3, 2, 0 }, (vec3_t(float)) { 0, 0, -30 * DEG2RAD } );
// 	mat4_t(float) viewMatrix = mat4_inverse(float)(cameraTransform);
// 	mat4_t(float) modelMatrix = mat4_transform((vec3_t(float)) { 0, 0, 0 }, (vec3_t(float)) { 0, -30 * DEG2RAD, 0 });
// 	mat4_t(float) projectionMatrix = mat4_persp_projection(float)(0, 10, 65 * DEG2RAD, (float)renderer->width/renderer->height);
// 	vertex2d_t* vertices = foreach_vertex3d(vertexCount, geometry3D, mat4_mul(float)(3, projectionMatrix, viewMatrix, modelMatrix));

// 	convert_to_vulkan_clipspace(vertices, vertexCount);
// 	u32 indices[] =
// 	{
// 		//clockwise order
// 		2, 1, 0, 3, 2, 0,
// 		6, 5, 4, 7, 6, 4,
// 		8, 9, 10, 8, 10, 11,
// 		12, 13, 14, 12, 14, 15,
// 		16, 17, 18, 16, 18, 19,
// 		20, 21, 22, 20, 22, 23
// 	};
// 	u32 indexCount = sizeof(indices) / sizeof(u32);
// }