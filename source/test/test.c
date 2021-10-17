
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_wrapper.h>
#include <debug.h>
#include <string/string.h>
#include <garbage_collector/garbage_collector.h>
#include <array/header_config.h>
#include <array/array.h>
#include <utilities/file_utility.h>
#include <defines.h>

#define BIT(flag, bit) ((flag & bit) == bit)

instantiate_array_struct(VkDeviceQueueCreateInfo);
instantiate_declaration_array(VkDeviceQueueCreateInfo);
instantiate_implementation_array(VkDeviceQueueCreateInfo);

#define WINDOW_HEIGHT 600
#define WINDOW_WIDTH 600

__attribute__((destructor))
void destructor()
{
	glfwTerminate();
}

__attribute__((constructor))
void constructor()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}

const char* vk_extension_properties_to_string(VkExtensionProperties* properties)
{
	return string(VK_MAX_EXTENSION_NAME_SIZE + 33, "[%s, %u]", properties->extensionName, properties->specVersion);
}

const char* vk_surface_transform_bitmask_to_string(VkFlags flag)
{
	char* buffer = GC_ALLOC(2048); 		//2 KB
	memset(buffer, 0, 2048);
	if(BIT(flag, VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR))
		strcat(buffer, "VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR");
    if(BIT(flag, VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR))
    	strcat(buffer, "VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR");
    if(BIT(flag, VK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR))
    	strcat(buffer, "VK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR");
    if(BIT(flag, VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR))
    	strcat(buffer, "VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR");
    if(BIT(flag, VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_BIT_KHR))
    	strcat(buffer, "VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_BIT_KHR");
    if(BIT(flag, VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90_BIT_KHR))
    	strcat(buffer, "VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90_BIT_KHR");
    if(BIT(flag, VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180_BIT_KHR))
    	strcat(buffer, "VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180_BIT_KHR");
    if(BIT(flag, VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270_BIT_KHR))
    	strcat(buffer, "VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270_BIT_KHR");
    if(BIT(flag, VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR))
    	strcat(buffer, "VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR");
    return (const char*)buffer;
}

const char* vk_surface_transform_bit_to_string(VkSurfaceTransformFlagBitsKHR transform)
{
	switch(transform)
	{
		case VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR: return "VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR";
    	case VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR: return "VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR";
    	case VK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR: return "VK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR";
    	case VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR: return "VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR";
    	case VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_BIT_KHR: return "VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_BIT_KHR";
    	case VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90_BIT_KHR: return "VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90_BIT_KHR";
    	case VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180_BIT_KHR: return "VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180_BIT_KHR";
    	case VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270_BIT_KHR: return "VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270_BIT_KHR";
    	case VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR: return "VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR";
    	default: return "Unknown";
	}
}

const char* vk_composite_alpha_bitmask_to_string(VkFlags flags)
{
	char* buffer = GC_ALLOC(1024); 		//1KB
	memset(buffer, 0, 1024);
	if(BIT(flags, VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR)) strcat(buffer, "[VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR]");
    if(BIT(flags, VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR)) strcat(buffer, "[VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR]");
    if(BIT(flags, VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR)) strcat(buffer, "[VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR]");
    if(BIT(flags, VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR)) strcat(buffer, "[VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR]");
    return (const char*)buffer;
}

const char* vk_image_usage_bitmask_to_string(VkFlags flags)
{
	char* buffer = GC_ALLOC(1024);		//1KB
	memset(buffer, 0, 1024);
	if(BIT(flags, VK_IMAGE_USAGE_SAMPLED_BIT)) strcat(buffer, "[VK_IMAGE_USAGE_SAMPLED_BIT]");
	if(BIT(flags, VK_IMAGE_USAGE_STORAGE_BIT)) strcat(buffer, "[VK_IMAGE_USAGE_STORAGE_BIT]");
	if(BIT(flags, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)) strcat(buffer, "[VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT]");
	if(BIT(flags, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)) strcat(buffer, "[VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT]");
	if(BIT(flags, VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT)) strcat(buffer, "[VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT]");
	if(BIT(flags, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT)) strcat(buffer, "[VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT]");
	if(BIT(flags, VK_IMAGE_USAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR)) strcat(buffer, "[VK_IMAGE_USAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR]");
	if(BIT(flags, VK_IMAGE_USAGE_FRAGMENT_DENSITY_MAP_BIT_EXT)) strcat(buffer, "[VK_IMAGE_USAGE_FRAGMENT_DENSITY_MAP_BIT_EXT]");
	return buffer;
}

const char* vk_surface_capabilities_to_string(VkSurfaceCapabilitiesKHR* capabilities)
{
	return string(4096 * 2,
		"Surface Capabilities:\n"
		"Min Image Count: %u\n"
		"Max Image Count: %u\n"
		"Current Extent: (%u, %u)\n"
		"Min Image Extent: (%u, %u)\n"
		"Max Image Extent: (%u, %u)\n"
		"Max Image Array Layers: %u\n"
		"Supported Transforms: %s\n"
		"Current Transform: %s\n"
		"Supported Composite Alpha: %s\n"
		"Supported Image Usage: %s\n",
		capabilities->minImageCount, 
		capabilities->maxImageCount, 
		capabilities->currentExtent.width, capabilities->currentExtent.height,
		capabilities->minImageExtent.width, capabilities->minImageExtent.height,
		capabilities->maxImageExtent.width, capabilities->maxImageExtent.height,
		capabilities->maxImageArrayLayers,
		vk_surface_transform_bitmask_to_string(capabilities->supportedTransforms),
		vk_surface_transform_bit_to_string(capabilities->currentTransform),
		vk_composite_alpha_bitmask_to_string(capabilities->supportedCompositeAlpha),
		vk_image_usage_bitmask_to_string(capabilities->supportedUsageFlags)
		);
}

int main(int argc, char** argv)
{
	u32 instanceExtensionCount;
	vkCall(vkEnumerateInstanceExtensionProperties(NULL, &instanceExtensionCount, NULL));
	VkExtensionProperties* instanceExtensions = GC_NEWV(VkExtensionProperties, instanceExtensionCount); 
	vkCall(vkEnumerateInstanceExtensionProperties(NULL, &instanceExtensionCount, instanceExtensions));

	log_msg("SUPPORTED INSTANCE EXTENSIONS");
	for(u32 i = 0; i < instanceExtensionCount; i++)
	{
		log_msg("%u. %s", i + 1, vk_extension_properties_to_string(instanceExtensions + i));
	}

	u32 glfwRequiredExtensionCount;
	const char* * glfwRequiredExtensions = glfwGetRequiredInstanceExtensions(&glfwRequiredExtensionCount);

	log_msg("GLFW REQUIRED INSTANCE EXTENSIONS");
	for(u32 i = 0; i < glfwRequiredExtensionCount; i++)
	{
		log_msg("%u. %s", i + 1, glfwRequiredExtensions[i]);
	}

	for(u32 i = 0; i < glfwRequiredExtensionCount; i++)
	{
		bool found = false;
		for(u32 j = 0; j < instanceExtensionCount; j++)
		{
			if(!strcmp(glfwRequiredExtensions[i], instanceExtensions[j].extensionName))
			{	
				found = true;
				break;
			}
		}
		if(!found)
		{
			log_fetal_err("Glfw required extension %s isn't supported", glfwRequiredExtensions[i]);
		}
	}


	VkInstanceCreateInfo instanceCreateInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.enabledLayerCount = 0,
		.ppEnabledLayerNames = NULL, 
		.enabledExtensionCount = glfwRequiredExtensionCount,
		.ppEnabledExtensionNames = glfwRequiredExtensions,
		.pApplicationInfo = NULL
	};
	VkInstance instance;
	vkCall(vkCreateInstance(&instanceCreateInfo, NULL, &instance));

	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Vulkan Renderer Test", NULL, NULL);
	if(window == NULL)
	{
		log_fetal_err("Failed to create GLFW window");
	}

	VkSurfaceKHR surface;
	vkCall(glfwCreateWindowSurface(instance, window, NULL, &surface));


	u32 gpuCount;
	VkPhysicalDevice* gpus;
	vkCall(vkEnumeratePhysicalDevices(instance, &gpuCount, NULL));
	gpus = GC_NEWV(VkPhysicalDevice, gpuCount); 
	vkCall(vkEnumeratePhysicalDevices(instance, &gpuCount, gpus)); 

	log_msg("GPU INFORMATION");
	log_msg("GPU count: %d", gpuCount);
	for(u32 i = 0; i < gpuCount; i++)
	{
		VkPhysicalDeviceProperties gpuProperties;
		vkGetPhysicalDeviceProperties(gpus[i], &gpuProperties);
		log_msg("%u. %s", i + 1, vk_physical_device_properties_to_string(&gpuProperties));
	}

	VkPhysicalDevice gpu = gpus[0];


	u32 queueFamilyCount; 
	VkQueueFamilyProperties* queueFamilyProperties;
	vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queueFamilyCount, NULL); 
	queueFamilyProperties = GC_NEWV(VkQueueFamilyProperties, queueFamilyCount); 
	vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queueFamilyCount, queueFamilyProperties); 

	s32 graphicsQueueIndex = -1;
	s32 presentQueueIndex = -1;
	for(u32 i = 0; i < queueFamilyCount; i++)
	{
		if(graphicsQueueIndex == -1)
		{
			if(queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
				graphicsQueueIndex = i;
		}

		if(presentQueueIndex == -1)
		{
			VkBool32 supportsPresentation;
			vkCall(vkGetPhysicalDeviceSurfaceSupportKHR(gpu, i, surface, &supportsPresentation));
			if(supportsPresentation)
				presentQueueIndex = i;
		}
	}

	if(graphicsQueueIndex == -1)
	{
		log_fetal_err("Failed to find queue with VK_QUEUE_GRAPHICS_BIT bit set");
	}

	if(presentQueueIndex == -1)
	{
		log_fetal_err("Failed to find queue with presentation support");
	}

	bool sameQueue = (graphicsQueueIndex == presentQueueIndex);
	if(!sameQueue)
	{
		log_msg("Graphics and Present Queues are different");
	}
	VkDeviceQueueCreateInfo graphicsQueueCreateInfo = 
	{
		.queueFamilyIndex = graphicsQueueIndex,
		.queueCount = 1,
		.pQueuePriorities = array(float)(1, 1.0f).data
	}; 

	VkDeviceQueueCreateInfo presentQueueCreateInfo = 
	{
		.queueFamilyIndex = presentQueueIndex,
		.queueCount = 1,
		.pQueuePriorities = array(float)(1, 1.0f).data
	};

	VkPhysicalDeviceFeatures enabledFeatures =  { };
	VkDeviceCreateInfo logicalDeviceCreateInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.enabledExtensionCount = 1,
		.ppEnabledExtensionNames = string_array(1, "VK_KHR_swapchain"),
		.pEnabledFeatures = &enabledFeatures,
		.enabledLayerCount = 0,
		.ppEnabledLayerNames = NULL, 
		.queueCreateInfoCount = sameQueue ? 1 : 2,
		.pQueueCreateInfos = sameQueue ? &graphicsQueueCreateInfo : array(VkDeviceQueueCreateInfo)(2, graphicsQueueCreateInfo, presentQueueCreateInfo).data
	};

	VkDevice device;
	vkCall(vkCreateDevice(gpu, &logicalDeviceCreateInfo, NULL, &device));


	//Queues
	VkQueue graphicsQueue, presentQueue; 
	vkGetDeviceQueue(device, graphicsQueueIndex, 0, &graphicsQueue); 
	vkGetDeviceQueue(device, presentQueueIndex, 0, &presentQueue); 



	//Swapchain
	VkSurfaceCapabilitiesKHR capabilities; 
	vkCall(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, surface, &capabilities));
	log_msg(vk_surface_capabilities_to_string(&capabilities));



	VkSwapchainCreateInfoKHR swapchainCreateInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.surface = surface,
		.minImageCount = 3,
		.imageFormat = VK_FORMAT_B8G8R8A8_SRGB,
		.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
		.imageExtent = { WINDOW_WIDTH, WINDOW_HEIGHT },
		.imageArrayLayers = 1,
		.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = NULL,
		.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
		.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		.presentMode = VK_PRESENT_MODE_MAILBOX_KHR,
		.clipped = VK_TRUE,
		.oldSwapchain = VK_NULL_HANDLE
	};

	VkSwapchainKHR swapchain;
	vkCall(vkCreateSwapchainKHR(device, &swapchainCreateInfo, NULL, &swapchain));


	//Graphics Pipeline


	//Shaders
	tuple_t(uint64_t, pchar_t) vertexShaderBytes = load_binary_from_file("shaders/test/vertexShader.vert");
	tuple_t(uint64_t, pchar_t) fragmentShaderBytes = load_binary_from_file("shaders/test/fragmentShader.frag");
	VkShaderModuleCreateInfo vertexShaderCreateInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.pCode = (const uint32_t*)vertexShaderBytes.value2,
		.codeSize = vertexShaderBytes.value1

	};
	VkShaderModule vertexShaderModule;
	vkCall(vkCreateShaderModule(device, &vertexShaderCreateInfo, NULL, &vertexShaderModule));
	VkShaderModuleCreateInfo fragmentShaderCreateInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.pCode = (const uint32_t*)fragmentShaderBytes.value2,
		.codeSize = fragmentShaderBytes.value1
	};
	VkShaderModule fragmentShaderModule;
	vkCall(vkCreateShaderModule(device, &fragmentShaderCreateInfo, NULL, &fragmentShaderModule)); 



	//Semphores
	VkSemaphoreCreateInfo semaphoreCreateInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,	
	}; 
	VkSemaphore imageAvailableSemphore;
	vkCreateSemaphore(device, &semaphoreCreateInfo, NULL, &imageAvailableSemphore);
	VkSemaphore renderingCompleteSemaphore; 
	vkCreateSemaphore(device, &semaphoreCreateInfo, NULL, &renderingCompleteSemaphore);

	//Swapchain Images
	u32 imageCount;
	vkCall(vkGetSwapchainImagesKHR(device, swapchain, &imageCount, NULL));
	log_msg("Requested swapchain image count: %u, Created swapchain image count: %u", capabilities.minImageCount + 1, imageCount);
	VkImage* images = GC_NEWV(VkImage, imageCount); 
	vkCall(vkGetSwapchainImagesKHR(device, swapchain, &imageCount, images));


	//Render Pass
	VkAttachmentDescription colorAttachmentDescription = 
	{
		.format = VK_FORMAT_B8G8R8A8_SRGB,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR, 
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE, 
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
	};

	VkAttachmentReference colorAttachmentReference = 
	{
		.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		.attachment = 0
	};

	VkSubpassDescription subpass = 
	{
		.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
		.colorAttachmentCount = 1, 
		.pColorAttachments = &colorAttachmentReference
	}; 

	VkSubpassDependency dependency =
	{

	};

	VkRenderPassCreateInfo renderPassCreateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.attachmentCount = 1,
		.pAttachments = &colorAttachmentDescription,
		.subpassCount = 1, 
		.pSubpasses = &subpass,
	};

	VkRenderPass renderPass;
	vkCall(vkCreateRenderPass(device, &renderPassCreateInfo, NULL, &renderPass));


	//Swapchain Image Views
	VkImageView* imageViews = GC_NEWV(VkImageView, imageCount); 

	for(u32 i = 0; i < imageCount; i++)
	{
		VkImageViewCreateInfo createInfo = 
		{
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.image = images[i],
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = VK_FORMAT_B8G8R8A8_SRGB,
			.components.r = VK_COMPONENT_SWIZZLE_IDENTITY,
			.components.g = VK_COMPONENT_SWIZZLE_IDENTITY,
			.components.b = VK_COMPONENT_SWIZZLE_IDENTITY,
			.components.a = VK_COMPONENT_SWIZZLE_IDENTITY,
			.subresourceRange =
			{
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1
			}
		};

		vkCall(vkCreateImageView(device, &createInfo, NULL, &imageViews[i]));	
	}


	//Framebuffers

	VkFramebuffer* framebuffers = GC_NEWV(VkFramebuffer, imageCount);
	for(u32 i = 0; i < imageCount; i++)
	{
		VkFramebufferCreateInfo createInfo = 
		{
			.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			.attachmentCount = 1,
			.pAttachments = &imageViews[i],
			.renderPass = renderPass,
			.width = WINDOW_WIDTH,
			.height = WINDOW_HEIGHT,
			.layers = 1
		}; 
		vkCall(vkCreateFramebuffer(device, &createInfo, NULL, &framebuffers[i]));
	}


	//Command Buffers
	VkCommandPoolCreateInfo commandPoolCreateInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.queueFamilyIndex = graphicsQueueIndex
	};
	VkCommandPool commandPool; 
	vkCall(vkCreateCommandPool(device, &commandPoolCreateInfo, NULL, &commandPool));

	VkCommandBuffer commandBuffers[capabilities.minImageCount + 1];
	VkCommandBufferAllocateInfo commandBufferAllocateInfo = 
	{
		.commandPool = commandPool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = capabilities.minImageCount + 1
	};

	vkCall(vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, commandBuffers));

	u64 count = 0;
	while(!glfwWindowShouldClose(window))
	{		
		u32 imageIndex;
		VkResult err = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, imageAvailableSemphore, VK_NULL_HANDLE, &imageIndex);
		switch(err)
		{
			case VK_SUCCESS:
			case VK_SUBOPTIMAL_KHR: 
					break;
			case VK_ERROR_OUT_OF_DATE_KHR: 
				log_msg("Window is resized"); 
				break;
			default:
				log_err("Error occured with Swap chain image acquisition, Count %d", count);
		}

		u32 waitStages[1] =  {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
		VkSubmitInfo submitInfo = 
		{
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.commandBufferCount = 1,
			.pCommandBuffers = &commandBuffers[imageIndex], 
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &imageAvailableSemphore,
			.pWaitDstStageMask = waitStages,
			.signalSemaphoreCount = 1,
			.pSignalSemaphores = &renderingCompleteSemaphore,

		}; 

		vkCall(vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE));

		VkPresentInfoKHR presentInfo = 
		{
			.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &renderingCompleteSemaphore,
			.swapchainCount = 1,
			.pSwapchains = &swapchain,
			.pImageIndices = &imageIndex
		};

		vkCall(vkQueuePresentKHR(presentQueue, &presentInfo));

		glfwPollEvents();
//		glfwSwapBuffers(window);
		count++;
	}


	vkDestroyCommandPool(device, commandPool, NULL);
	vkDestroySemaphore(device, imageAvailableSemphore, NULL); 
	vkDestroySemaphore(device, renderingCompleteSemaphore, NULL);
	vkDestroyShaderModule(device, vertexShaderModule, NULL); 
	vkDestroyShaderModule(device, fragmentShaderModule, NULL);
	vkDestroySwapchainKHR(device, swapchain, NULL);
	vkDestroyDevice(device, NULL);
	vkDestroyInstance(instance, NULL);
	return 0;
}