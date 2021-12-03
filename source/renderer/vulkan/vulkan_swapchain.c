
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_swapchain.h>
#include <renderer/render_window.h>
#include <memory_allocator/memory_allocator.h>
#include <memory.h>
#include <renderer/assert.h>

static void create_swapchain(vulkan_swapchain_t* swapchain, renderer_t* renderer);
static void destroy_swapchain(vulkan_swapchain_t* swapchain, renderer_t* renderer);
static void destroy_semaphores(vulkan_swapchain_t* swapchain, renderer_t* renderer);

vulkan_swapchain_t* vulkan_swapchain_new()
{
	vulkan_swapchain_t* swapchain = heap_new(vulkan_swapchain_t);
	memset(swapchain, 0, sizeof(vulkan_swapchain_t));
	return refp(vulkan_swapchain_t, swapchain, 0);
}

vulkan_swapchain_t* vulkan_swapchain_create(render_window_t* window, renderer_t* renderer)
{
	ASSERT(renderer->vk_device != VK_NULL_HANDLE, "renderer->vk_device == VK_NULL_HANDLE\n");
	vulkan_swapchain_t* swapchain = vulkan_swapchain_new();
	refp(vulkan_swapchain_t, swapchain, 0)->window = window;
	refp(vulkan_swapchain_t, swapchain, 0)->image_count = 3;
	render_window_get_vulkan_surface(window, &(renderer->vk_instance), &(refp(vulkan_swapchain_t, swapchain, 0)->surface));
	create_swapchain(refp(vulkan_swapchain_t, swapchain, 0), renderer);
	refp(vulkan_swapchain_t, swapchain, 0)->image_available_semaphore = vk_get_semaphore(renderer->vk_device);
	refp(vulkan_swapchain_t, swapchain, 0)->render_finished_semaphore = vk_get_semaphore(renderer->vk_device);
	return refp(vulkan_swapchain_t, swapchain, 0);
}


void vulkan_swapchain_refresh(vulkan_swapchain_t* swapchain, renderer_t* renderer)
{
	destroy_swapchain(refp(vulkan_swapchain_t, swapchain, 0), renderer);
	VkSwapchainCreateInfoKHR create_info =
	{
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.minImageCount = refp(vulkan_swapchain_t, swapchain, 0)->image_count,
		.imageFormat = VK_FORMAT_B8G8R8A8_SRGB,
		.imageExtent = { refp(vulkan_swapchain_t, swapchain, 0)->window->width, refp(vulkan_swapchain_t, swapchain, 0)->window->height },
		.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
		.imageArrayLayers = 1,
		.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
		.presentMode = VK_PRESENT_MODE_MAILBOX_KHR,
		.clipped = VK_TRUE,
		.surface = refp(vulkan_swapchain_t, swapchain, 0)->surface,
		.oldSwapchain = VK_NULL_HANDLE,
		.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR
	};
	refp(vulkan_swapchain_t, swapchain, 0)->swapchain = vk_get_swapchain(renderer->vk_device, &create_info);
	vk_get_images_out(renderer->vk_device, refp(vulkan_swapchain_t, swapchain, 0)->swapchain, refp(vulkan_swapchain_t, swapchain, 0)->images, refp(vulkan_swapchain_t, swapchain, 0)->image_count);
	vk_get_image_views_out(renderer->vk_device, VK_FORMAT_B8G8R8A8_SRGB, refp(vulkan_swapchain_t, swapchain, 0)->image_count, refp(vulkan_swapchain_t, swapchain, 0)->images, refp(vulkan_swapchain_t, swapchain, 0)->image_views);
	vk_get_framebuffers_out(renderer->vk_device, refp(vulkan_swapchain_t, swapchain, 0)->image_count, renderer->vk_render_pass, (VkExtent2D) { renderer->window->width, renderer->window->height }, 1, refp(vulkan_swapchain_t, swapchain, 0)->image_views, refp(vulkan_swapchain_t, swapchain, 0)->framebuffers);
}

void vulkan_swapchain_destroy(vulkan_swapchain_t* swapchain, renderer_t* renderer)
{
	ASSERT(renderer->vk_device != VK_NULL_HANDLE, "renderer->vk_device == VK_NULL_HANDLE\n");
	destroy_semaphores(refp(vulkan_swapchain_t, swapchain, 0), renderer);
	destroy_swapchain(refp(vulkan_swapchain_t, swapchain, 0), renderer);
	vkDestroySurfaceKHR(renderer->vk_instance, refp(vulkan_swapchain_t, swapchain, 0)->surface, NULL);
}

u32 vulkan_swapchain_acquire_next_image(vulkan_swapchain_t* swapchain, renderer_t* renderer)
{
	ASSERT(renderer->vk_device != VK_NULL_HANDLE, "renderer->vk_device == VK_NULL_HANDLE\n");
	vkAcquireNextImageKHR(renderer->vk_device, refp(vulkan_swapchain_t, swapchain, 0)->swapchain, UINT64_MAX, refp(vulkan_swapchain_t, swapchain, 0)->image_available_semaphore, VK_NULL_HANDLE, &(refp(vulkan_swapchain_t, swapchain, 0)->current_image_index));
	return refp(vulkan_swapchain_t, swapchain, 0)->current_image_index;
}

void vulkan_swapchain_release_resources(vulkan_swapchain_t* swapchain)
{
	heap_free(swapchain->image_views);
	heap_free(swapchain->images);
	heap_free(swapchain->framebuffers);
	heap_free(swapchain);
}

#if GLOBAL_DEBUG
static void print_current_transform(VkSurfaceTransformFlagBitsKHR transform_bit)
{
	printf("Current Transform: ");
	bool found = false;
	if(transform_bit == 0x00000001)
		{ found = true; printf("VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR\n"); }
    if(transform_bit == 0x00000002)
    	{ found = true; printf("VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR\n"); }
    if(transform_bit == 0x00000004)
    	{ found = true; printf("VK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR\n"); }
    if(transform_bit == 0x00000008)
    	{ found = true; printf("VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR\n"); }
    if(transform_bit == 0x00000010)
    	{ found = true; printf("VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_BIT_KHR\n"); }
    if(transform_bit == 0x00000020)
    	{ found = true; printf("VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90_BIT_KHR\n"); }
    if(transform_bit == 0x00000040)
    	{ found = true; printf("VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180_BIT_KHR\n"); }
    if(transform_bit == 0x00000080)
    	{ found = true; printf("VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270_BIT_KHR\n"); }
    if(transform_bit == 0x00000100)
    	{ found = true; printf("VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR\n"); }
    if(transform_bit == 0x7FFFFFFF)
    	{ found = true; printf("VK_SURFACE_TRANSFORM_FLAG_BITS_MAX_ENUM_KHR\n");     }
    if(!found)
    	printf("Unknown\n");
}
static void print_supported_transforms(VkSurfaceTransformFlagsKHR transform_bits)
{
	printf("Supported Transforms: ");
	bool found = false;
	if(transform_bits & 0x00000001)
		{ found = true; printf("VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR |"); }
	if(transform_bits & 0x00000002)
	    { found = true; printf("VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR |"); }
	if(transform_bits & 0x00000004)
	    { found = true; printf("VK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR |"); }
	if(transform_bits & 0x00000008)
	    { found = true; printf("VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR |"); }
	if(transform_bits & 0x00000010)
	    { found = true; printf("VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_BIT_KHR |"); }
	if(transform_bits & 0x00000020)
	    { found = true; printf("VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90_BIT_KHR |"); }
	if(transform_bits & 0x00000040)
	    { found = true; printf("VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180_BIT_KHR |"); }
	if(transform_bits & 0x00000080)
	    { found = true; printf("VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270_BIT_KHR |"); }
	if(transform_bits & 0x00000100)
	    { found = true; printf("VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR |"); }
	if(transform_bits & 0x7FFFFFFF)
		{ found = true; printf("VK_SURFACE_TRANSFORM_FLAG_BITS_MAX_ENUM_KHR |"); }
	if(!found)
		printf("Unknown");
	puts("");
}
#endif

static void create_swapchain(vulkan_swapchain_t* swapchain, renderer_t* renderer)
{
	ASSERT(swapchain->surface != VK_NULL_HANDLE, "swapchain->surface == VK_NULL_HANDLE\n");
	ASSERT(swapchain->window != NULL, "swapchain->window == NULL\n");
	ASSERT(renderer->vk_render_pass != VK_NULL_HANDLE, "renderer->vk_render_pass == VK_NULL_HANDLE\n");
	VkBool32 is_supported;
	vkCall(vkGetPhysicalDeviceSurfaceSupportKHR(renderer->vk_physical_device, renderer->vk_graphics_queue_index, refp(vulkan_swapchain_t, swapchain, 0)->surface, &is_supported));
	ASSERT(is_supported == VK_TRUE, "surface not supported by the choosen physical device (GPU)\n");

	VkSurfaceCapabilitiesKHR surface_capabilities;
	vkCall(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(renderer->vk_physical_device, swapchain->surface, &surface_capabilities));

#if GLOBAL_DEBUG
	print_current_transform(surface_capabilities.currentTransform);
	print_supported_transforms(surface_capabilities.supportedTransforms);
#endif

	VkSwapchainCreateInfoKHR create_info =
	{
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.minImageCount = refp(vulkan_swapchain_t, swapchain, 0)->image_count,
		.imageFormat = VK_FORMAT_B8G8R8A8_SRGB,
		.imageExtent = { refp(vulkan_swapchain_t, swapchain, 0)->window->width, refp(vulkan_swapchain_t, swapchain, 0)->window->height },
		.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
		.imageArrayLayers = 1,
		.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
		.presentMode = VK_PRESENT_MODE_MAILBOX_KHR,
		.clipped = VK_TRUE,
		.surface = refp(vulkan_swapchain_t, swapchain, 0)->surface,
		.oldSwapchain = VK_NULL_HANDLE,
		.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR
	};

	refp(vulkan_swapchain_t, swapchain, 0)->swapchain = vk_get_swapchain(renderer->vk_device, &create_info);
	refp(vulkan_swapchain_t, swapchain, 0)->images = vk_get_images(renderer->vk_device, refp(vulkan_swapchain_t, swapchain, 0)->swapchain).value2;
	refp(vulkan_swapchain_t, swapchain, 0)->image_views = vk_get_image_views(renderer->vk_device, VK_FORMAT_B8G8R8A8_SRGB, refp(vulkan_swapchain_t, swapchain, 0)->image_count, refp(vulkan_swapchain_t, swapchain, 0)->images).value2;
	refp(vulkan_swapchain_t, swapchain, 0)->framebuffers = vk_get_framebuffers(renderer->vk_device, refp(vulkan_swapchain_t, swapchain, 0)->image_count, renderer->vk_render_pass, (VkExtent2D) { renderer->window->width, renderer->window->height }, 1, refp(vulkan_swapchain_t, swapchain, 0)->image_views).value2;
	refp(vulkan_swapchain_t, swapchain, 0)->current_image_index = 0;
}


static void destroy_semaphores(vulkan_swapchain_t* swapchain, renderer_t* renderer)
{
	vkDestroySemaphore(renderer->vk_device, refp(vulkan_swapchain_t, swapchain, 0)->image_available_semaphore, NULL);
	vkDestroySemaphore(renderer->vk_device, refp(vulkan_swapchain_t, swapchain, 0)->render_finished_semaphore, NULL);
}

static void destroy_swapchain(vulkan_swapchain_t* swapchain, renderer_t* renderer)
{
	vkDestroySwapchainKHR(renderer->vk_device, refp(vulkan_swapchain_t, swapchain, 0)->swapchain, NULL);
	for(u32 i = 0; i < refp(vulkan_swapchain_t, swapchain, 0)->image_count; i++)
		vkDestroyFramebuffer(renderer->vk_device, refp(vulkan_swapchain_t, swapchain, 0)->framebuffers[i], NULL);
	for(u32 i = 0; i < refp(vulkan_swapchain_t, swapchain, 0)->image_count; i++)
		vkDestroyImageView(renderer->vk_device, refp(vulkan_swapchain_t, swapchain, 0)->image_views[i], NULL);
}
