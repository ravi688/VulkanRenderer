

#include <engine/renderer/renderer.h>
#include <exception/exception.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string/string.h>		//custom string library
#include <string.h>				//standard string library

#include <tuple/header_config.h>
#include <tuple/tuple.h>

#include <garbage_collector/garbage_collector.h>
#include <vulkan/vulkan_wrapper.h>

#include <exception/exception.h>
#include <debug.h>


typedef VkImage* pVkImage_t;
instantiate_tuple_t(uint32_t, pVkImage_t);

typedef VkImageView* pVkImageView_t;
instantiate_tuple_t(uint32_t, pVkImageView_t);

define_exception(VULKAN_GRAPHICS_QUEUE_NOT_FOUND); 
declare_exception(VULKAN_GRAPHICS_QUEUE_NOT_FOUND);

define_exception(VULKAN_SURFACE_NOT_SUPPORTED);
declare_exception(VULKAN_SURFACE_NOT_SUPPORTED);

define_exception(VULKAN_PHYSICAL_DEVICE_EXTENSION_NOT_SUPPORTED); 
declare_exception(VULKAN_PHYSICAL_DEVICE_EXTENSION_NOT_SUPPORTED);

static void vk_dump_physical_device_extensions(renderer_t* renderer);
static tuple_t(uint32_t, pVkImage_t) vk_get_images(renderer_t* renderer);
static tuple_t(uint32_t, pVkImageView_t) vk_get_image_views(renderer_t* renderer);

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
	VkFormat vk_format; 
	VkExtent2D vk_extent;
} renderer_t;

void* renderer_get_vulkan_instance(renderer_t* renderer) { return (void*)(&(renderer->vk_instance)); }
void* renderer_get_vulkan_device(renderer_t* renderer) { return (void*)(&(renderer->vk_device)); }
void* renderer_get_vulkan_surface(renderer_t* renderer) { return (void*)(&(renderer->vk_surface)); }
void* renderer_get_vulkan_swapchain(renderer_t* renderer) { return (void*)(&(renderer->vk_swapchain)); }
static VkSwapchainKHR vk_get_swapchain(renderer_t* renderer);


renderer_t* renderer_init()
{
	renderer_t* renderer = GC_NEW(renderer_t);
	renderer->vk_instance = vk_create_instance();

#ifdef ENABLE_VALIDATION_LAYERS
	vk_setup_validation_layers();
#endif

	//TODO: this should be something like this: 
	//		VkPhysicalDevice vulkan_device = vk_get_suitable_vulkan_device();
	tuple_t(uint32_t, pVkPhysicalDevice_t) physical_devices = vk_get_physical_devices(renderer->vk_instance); 
	renderer->vk_physical_device = vk_select_suitable_device(physical_devices);

	int32_t graphics_queue_index = -1;
	tuple_t(uint32_t, pVkQueueFamilyProperties_t) queue_families = vk_get_queue_family_properties(renderer->vk_physical_device); 
	for(uint32_t i = 0; i < queue_families.value1; i++)
	{
		VkQueueFamilyProperties properties = queue_families.value2[i];
		if(properties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			graphics_queue_index = i;
			renderer->vk_graphics_queue_index = (uint32_t)i;
			break;
		}
	}
EXCEPTION_BLOCK
(
	if(graphics_queue_index == -1)
		throw_exception(VULKAN_GRAPHICS_QUEUE_NOT_FOUND);
)

	VkDeviceQueueCreateInfo queueCreateInfo = { }; 
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = graphics_queue_index;
	queueCreateInfo.queueCount = 1;
	float queuePriorities = 1.0f;
	queueCreateInfo.pQueuePriorities = &queuePriorities;

	tuple_t(uint32_t, ppVkChar_t) requiredPhysicalDevicExtensions = (tuple_t(uint32_t, ppVkChar_t)) { 1, string_array(1, "VK_KHR_swapchain") };
EXCEPTION_BLOCK
(
	if(!vk_check_physical_device_extension_support(renderer->vk_physical_device, requiredPhysicalDevicExtensions))
		throw_exception(VULKAN_PHYSICAL_DEVICE_EXTENSION_NOT_SUPPORTED);
)

	VkPhysicalDeviceFeatures features =  { }; 
	VkDeviceCreateInfo logicalDeviceCreateInfo =  { }; 
	logicalDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO; 
	logicalDeviceCreateInfo.pQueueCreateInfos = &queueCreateInfo; 
	logicalDeviceCreateInfo.queueCreateInfoCount = 1;
	logicalDeviceCreateInfo.pEnabledFeatures = &features;
	logicalDeviceCreateInfo.enabledExtensionCount = 1; 
	logicalDeviceCreateInfo.ppEnabledExtensionNames = requiredPhysicalDevicExtensions.value2;
	logicalDeviceCreateInfo.enabledLayerCount = 0;

	VkDevice logicalDevice;
	vkCall(vkCreateDevice(renderer->vk_physical_device, &logicalDeviceCreateInfo, NULL, &logicalDevice));

	renderer->vk_device = logicalDevice;

	VkQueue graphicsQueue;
	vkGetDeviceQueue(renderer->vk_device, graphics_queue_index, 0, &graphicsQueue);
	renderer->vk_graphics_queue = graphicsQueue;

#ifdef DEBUG
	vk_dump_instance_extensions(); 
	vk_dump_physical_device_extensions(renderer);
	vk_dump_instance_layers();
	vk_dump_physical_devices(&physical_devices);
	vk_dump_queue_families(&queue_families);
#endif
	return renderer;
}

void renderer_init_surface(renderer_t* renderer, void* surface)
{
	renderer->vk_surface = *((VkSurfaceKHR*)surface);

EXCEPTION_BLOCK
(
	VkBool32 isSupported;
	vkCall(vkGetPhysicalDeviceSurfaceSupportKHR(renderer->vk_physical_device, renderer->vk_graphics_queue_index, renderer->vk_surface, &isSupported)); 
	if(!isSupported)
		throw_exception(VULKAN_SURFACE_NOT_SUPPORTED);
)
	renderer->vk_swapchain = vk_get_swapchain(renderer);
	renderer->vk_images = vk_get_images(renderer); 
	renderer->vk_image_views = vk_get_image_views(renderer);
}

static tuple_t(uint32_t, pVkImageView_t) vk_get_image_views(renderer_t* renderer)
{
	tuple_t(uint32_t, pVkImageView_t) imageViews  =  { renderer->vk_images.value1, GC_NEWV(VkImageView, renderer->vk_images.value1) }; 
	for(uint32_t i = 0; i < imageViews.value1; i++)
	{
		VkImageViewCreateInfo createInfo =  { };
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO; 
		createInfo.image = renderer->vk_images.value2[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = renderer->vk_format;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY; 
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT; 
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;
		vkCall(vkCreateImageView(renderer->vk_device, &createInfo, NULL, &(imageViews.value2[i])));
	}
	return imageViews;
}

static tuple_t(uint32_t, pVkImage_t) vk_get_images(renderer_t* renderer)
{
	tuple_t(uint32_t, pVkImage_t) pair; 
	vkCall(vkGetSwapchainImagesKHR(renderer->vk_device, renderer->vk_swapchain, &(pair.value1), NULL)); 
	pair.value2 = GC_NEWV(VkImage, pair.value1); 
	vkCall(vkGetSwapchainImagesKHR(renderer->vk_device, renderer->vk_swapchain, &(pair.value1), pair.value2)); 
	return pair;
}

static VkSwapchainKHR vk_get_swapchain(renderer_t* renderer)
{
	VkSurfaceCapabilitiesKHR surfaceCapabilities = vk_get_physical_device_surface_capabilities(renderer->vk_physical_device, renderer->vk_surface);
 	tuple_t(uint32_t, pVkSurfaceFormatKHR_t) formats = vk_get_physical_device_surface_formats(renderer->vk_physical_device, renderer->vk_surface);
 	tuple_t(uint32_t, pVkPresentModeKHR_t) presentModes = vk_get_physical_device_surface_present_modes(renderer->vk_physical_device, renderer->vk_surface);

EXCEPTION_BLOCK
(
	if(formats.value1 == 0)
		throw_exception(VULKAN_ABORTED);
	if(presentModes.value1 == 0)
		throw_exception(VULKAN_ABORTED);
)

	VkSurfaceFormatKHR format; bool isFormatFound = false;
	for(uint32_t i = 0; i < formats.value1; i++)
		if((formats.value2[i].format == VK_FORMAT_B8G8R8A8_SRGB) && (formats.value2[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR))
		{	format = formats.value2[i]; isFormatFound = true; break; }
	if(!isFormatFound)
		format = formats.value2[0];


	VkPresentModeKHR presentMode; bool isModeFound = false; 
	for(uint32_t i = 0; i < presentModes.value1; i++)
		if(presentModes.value2[i] == VK_PRESENT_MODE_MAILBOX_KHR)
		{	presentMode = presentModes.value2[i]; isModeFound = true; break; }
	if(!isModeFound)
		presentMode = VK_PRESENT_MODE_FIFO_KHR;

	uint32_t imageCount; 
	imageCount = surfaceCapabilities.minImageCount + 1; 
	if(imageCount > surfaceCapabilities.maxImageCount) imageCount = surfaceCapabilities.maxImageCount;

	VkExtent2D extent = surfaceCapabilities.currentExtent; 
	log_msg("Extent = (%u, %u)", extent.width, extent.height);
	log_msg("ImageCount: %u", imageCount);

	renderer->vk_extent = extent; 
	renderer->vk_format = format.format;

	VkSwapchainCreateInfoKHR createInfo = { }; 
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.minImageCount = imageCount; 
	createInfo.imageFormat = format.format;
	createInfo.imageColorSpace = format.colorSpace;
	createInfo.imageExtent = extent; 
	createInfo.imageArrayLayers = 1; 
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; 
	createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.queueFamilyIndexCount = 0; 
	createInfo.pQueueFamilyIndices = NULL; 
	createInfo.preTransform = surfaceCapabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;
	createInfo.surface = renderer->vk_surface;

	VkSwapchainKHR swapchain; 
	vkCall(vkCreateSwapchainKHR(renderer->vk_device, &createInfo, NULL, &swapchain)); 
	return swapchain;
}

void renderer_terminate(renderer_t* renderer)
{
	for(uint32_t i = 0; i < renderer->vk_image_views.value1; i++)
		vkDestroyImageView(renderer->vk_device, renderer->vk_image_views.value2[i], NULL);
	vkDestroySwapchainKHR(renderer->vk_device, renderer->vk_swapchain, NULL);
	vkDestroySurfaceKHR(renderer->vk_instance, renderer->vk_surface, NULL);
	vkDestroyDevice(renderer->vk_device, NULL);
	vkDestroyInstance(renderer->vk_instance, NULL);
}

static void vk_dump_physical_device_extensions(renderer_t* renderer)
{
	log_msg("Physical Device Extensions-----------------------");
	tuple_t(uint32_t, pVkExtensionProperties_t) extensions = vk_get_physical_device_extension_properties(renderer->vk_physical_device);
	for(uint32_t i = 0; i < extensions.value1; i++)
		log_msg(extensions.value2[i].extensionName);
}