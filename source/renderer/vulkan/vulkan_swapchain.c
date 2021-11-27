
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_swapchain.h>
#include <renderer/render_window.h>
#include <memory_allocator/memory_allocator.h>
#include <memory.h>
#include <renderer/assert.h>

static void create_swapchain(vulkan_swapchain_t* swapchain, renderer_t* renderer);
static void destroy_swapchain(vulkan_swapchain_t* swapchain, renderer_t* renderer);
static void destroy_semaphores(vulkan_swapchain_t* swapchain, renderer_t* renderer);
static void release_resources(vulkan_swapchain_t* swapchain);

vulkan_swapchain_t* vulkan_swapchain_create(render_window_t* window, renderer_t* renderer)
{
	ASSERT(renderer->vk_device != VK_NULL_HANDLE, "renderer->vk_device == VK_NULL_HANDLE\n");
	vulkan_swapchain_t* swapchain = heap_new(vulkan_swapchain_t);
	memset(swapchain, 0, sizeof(vulkan_swapchain_t));
	swapchain->window = window;
	swapchain->image_count = 3;
	render_window_get_vulkan_surface(window, &(renderer->vk_instance), &(swapchain->surface));
	create_swapchain(swapchain, renderer);
	swapchain->image_available_semaphore = vk_get_semaphore(renderer->vk_device);
	swapchain->render_finished_semaphore = vk_get_semaphore(renderer->vk_device);
	return swapchain;
}


void vulkan_swapchain_refresh(vulkan_swapchain_t* swapchain, renderer_t* renderer)
{
	destroy_swapchain(swapchain, renderer);
	VkSwapchainCreateInfoKHR create_info =
	{
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.minImageCount = swapchain->image_count,
		.imageFormat = VK_FORMAT_B8G8R8A8_SRGB,
		.imageExtent = { swapchain->window->width, swapchain->window->height },
		.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
		.imageArrayLayers = 1,
		.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
		.presentMode = VK_PRESENT_MODE_MAILBOX_KHR,
		.clipped = VK_TRUE,
		.surface = swapchain->surface,
		.oldSwapchain = VK_NULL_HANDLE
	};
	swapchain->swapchain = vk_get_swapchain(renderer->vk_device, &create_info);
	vk_get_images_out(renderer->vk_device, swapchain->swapchain, swapchain->images, swapchain->image_count);
	vk_get_image_views_out(renderer->vk_device, VK_FORMAT_B8G8R8A8_SRGB, swapchain->image_count, swapchain->images, swapchain->image_views);
	vk_get_framebuffers_out(renderer->vk_device, swapchain->image_count, renderer->vk_render_pass, (VkExtent2D) { renderer->window->width, renderer->window->height }, 1, swapchain->image_views, swapchain->framebuffers);
}

void vulkan_swapchain_destroy(vulkan_swapchain_t* swapchain, renderer_t* renderer)
{
	ASSERT(renderer->vk_device != VK_NULL_HANDLE, "renderer->vk_device == VK_NULL_HANDLE\n");
	vkDestroySurfaceKHR(renderer->vk_instance, swapchain->surface, NULL);
	destroy_semaphores(swapchain, renderer);
	destroy_swapchain(swapchain, renderer);
	release_resources(swapchain);
}

u32 vulkan_swapchain_acquire_next_image(vulkan_swapchain_t* swapchain, renderer_t* renderer)
{
	ASSERT(renderer->vk_device != VK_NULL_HANDLE, "renderer->vk_device == VK_NULL_HANDLE\n");
	vkAcquireNextImageKHR(renderer->vk_device, swapchain->swapchain, UINT64_MAX, swapchain->image_available_semaphore, VK_NULL_HANDLE, &(swapchain->current_image_index));
	return swapchain->current_image_index;
}

static void create_swapchain(vulkan_swapchain_t* swapchain, renderer_t* renderer)
{
	ASSERT(swapchain->surface != VK_NULL_HANDLE, "swapchain->surface == VK_NULL_HANDLE\n");
	ASSERT(swapchain->window != NULL, "swapchain->window == NULL\n");
	ASSERT(renderer->vk_render_pass != VK_NULL_HANDLE, "renderer->vk_render_pass == VK_NULL_HANDLE\n");
	VkSwapchainCreateInfoKHR create_info =
	{
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.minImageCount = swapchain->image_count,
		.imageFormat = VK_FORMAT_B8G8R8A8_SRGB,
		.imageExtent = { swapchain->window->width, swapchain->window->height },
		.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
		.imageArrayLayers = 1,
		.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
		.presentMode = VK_PRESENT_MODE_MAILBOX_KHR,
		.clipped = VK_TRUE,
		.surface = swapchain->surface,
		.oldSwapchain = VK_NULL_HANDLE
	};

	swapchain->swapchain = vk_get_swapchain(renderer->vk_device, &create_info);
	swapchain->images = vk_get_images(renderer->vk_device, swapchain->swapchain).value2;
	swapchain->image_views = vk_get_image_views(renderer->vk_device, VK_FORMAT_B8G8R8A8_SRGB, swapchain->image_count, swapchain->images).value2;
	swapchain->framebuffers = vk_get_framebuffers(renderer->vk_device, swapchain->image_count, renderer->vk_render_pass, (VkExtent2D) { renderer->window->width, renderer->window->height }, 1, swapchain->image_views).value2;
	swapchain->current_image_index = 0;
}

static void release_resources(vulkan_swapchain_t* swapchain)
{
	heap_free(swapchain->image_views);
	heap_free(swapchain->images);
	heap_free(swapchain->framebuffers);
	heap_free(swapchain);
}

static void destroy_semaphores(vulkan_swapchain_t* swapchain, renderer_t* renderer)
{
	vkDestroySemaphore(renderer->vk_device, swapchain->image_available_semaphore, NULL);
	vkDestroySemaphore(renderer->vk_device, swapchain->render_finished_semaphore, NULL);
}

static void destroy_swapchain(vulkan_swapchain_t* swapchain, renderer_t* renderer)
{
	vkDestroySwapchainKHR(renderer->vk_device, swapchain->swapchain, NULL);
	for(u32 i = 0; i < swapchain->image_count; i++)
		vkDestroyFramebuffer(renderer->vk_device, swapchain->framebuffers[i], NULL);
	for(u32 i = 0; i < swapchain->image_count; i++)
		vkDestroyImageView(renderer->vk_device, swapchain->image_views[i], NULL);
}
