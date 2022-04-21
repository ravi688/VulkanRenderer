
#pragma once

#include <vulkan/vulkan.h> 	// for vulkan

#include <stdint.h> 		// standard int types
#include <stdio.h> 			// printf, puts, fopen, fclose
#include <stdbool.h>	 	// bool
#include <stdlib.h> 		// malloc
#include <string.h> 		// memset
#include <stdarg.h> 		// va_list, va_start, va_end, va_arg
#include <assert.h> 		// assert()

/* Logging functions */
#define PVK_FETAL_ERROR(...) pvkFetalError(__VA_ARGS__)
#define PVK_WARNING(...) pvkLog("[Warning] ", __VA_ARGS__)
#define PVK_ERROR(...) pvkLog("[Error] ", __VA_ARGS__)
#define PVK_INFO(...) pvkLog("[Info] ", __VA_ARGS__)
#define PVK_LOG(description, ...) pvkLog(description, __VA_ARGS__)

static void pvkLog(const char* description, const char* format, ...)
{
	printf("%s", description);
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
}

static void pvkFetalError(const char* format, ...)
{
	printf("[Fetal Error] ");
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
	exit(0);
}

/* Memory functions */
#define new(type) (type*)__new(sizeof(type))
#define newv(type, count) (type*)__new(sizeof(type) * count)
static void* __new(size_t size) { void* block = malloc(size); memset(block, 0, size); return block; }
#define delete(ptr) __delete((char**)&(ptr))
static void __delete(char** ptr) 
{ 
	if(*ptr == NULL) {  PVK_WARNING("You are trying to free an invalid memory block, ptr = NULL"); return;  }
	free((void*)(*ptr)); 
	*ptr = NULL; 
}

/* Resulting checking */
#define PVK_CHECK(result) pvkCheckResult(result, __LINE__, __FUNCTION__, __FILE__)

static void pvkCheckResult(VkResult result, uint32_t line_no, const char* function_name, const char* source_name)
{
	if(result != VK_SUCCESS)
		PVK_FETAL_ERROR("Result != VK_SUCCESS, Result = %lld , at %lu, %s, %s\n", result, line_no, function_name, source_name);
}

/* Windowing system */
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

typedef struct PvkWindow
{
	void* handle;
	uint32_t width;
	uint32_t height;
} PvkWindow;


#if GLOBAL_DEBUG
static void glfwErrorCallback(int code, const char* description)
{
	PVK_ERROR("GLFW: %d, %s\n", code, description);
}
#endif

static PvkWindow* pvkWindowCreate(uint32_t width, uint32_t height, const char* title, bool full_screen)
{
	PvkWindow* window = new(PvkWindow);
	glfwInit();
#if GLOBAL_DEBUG
	glfwSetErrorCallback(glfwErrorCallback);
#endif
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window->handle = glfwCreateWindow(width, height, title, full_screen ? glfwGetPrimaryMonitor() : NULL, NULL);
	// glfwSetFramebufferSizeCallback(window->handle, glfwOnWindowResizeCallback);
	glfwSetWindowUserPointer(window->handle, window);
	window->width = width;
	window->height = height;
	return window;
}

static bool pvkWindowShouldClose(PvkWindow* window)
{
	return glfwWindowShouldClose(window->handle);
}

static void pvkWindowPollEvents(PvkWindow* window)
{
	glfwPollEvents();
}

static void pvkWindowDestroy(PvkWindow* window)
{
	glfwDestroyWindow(window->handle);
	glfwTerminate();
	delete(window);
}

static void pvkWindowGetFramebufferExtent(PvkWindow* window, uint32_t* out_width, uint32_t* out_height)
{
	glfwGetFramebufferSize(window->handle, out_width, out_height);
}

static VkSurfaceKHR pvkWindowCreateVulkanSurface(PvkWindow* window, VkInstance instance)
{
	VkSurfaceKHR surface;
	PVK_CHECK(glfwCreateWindowSurface(instance, window->handle, NULL, &surface));
	return surface;
}


/* Vulkan */

typedef struct PvkEnabledLayerAndExtensionInfo
{
	uint32_t layerCount;
	const char* const* layerNames;
	uint32_t extensionCount;
	const char* const* extensionNames;
} PvkEnabledLayerAndExtensionInfo;

static void __pvkCheckForInstanceExtensionSupport(uint32_t count, const char* const* extensions)
{
	uint32_t supportedCount;
	PVK_CHECK(vkEnumerateInstanceExtensionProperties(NULL, &supportedCount, NULL));
	VkExtensionProperties* supportedExtensions = newv(VkExtensionProperties, supportedCount);
	PVK_CHECK(vkEnumerateInstanceExtensionProperties(NULL, &supportedCount, supportedExtensions));

	for(uint32_t i = 0; i < count; i++)
	{
		bool found = false;
		for(uint32_t j = 0; j < supportedCount; j++)
		{
			if(strcmp(extensions[i], supportedExtensions[j].extensionName) == 0)
			{
				found = true;
				break;
			}
		}
		if(!found)
			PVK_FETAL_ERROR("Extension \"%s\" isn't supported by the vulkan implementation\n", extensions[i]);
	}
	delete(supportedExtensions);
}

static VkInstance __pvkCreateVulkanInstance(const char* appName, uint32_t appVersion, uint32_t apiVersion, PvkEnabledLayerAndExtensionInfo* enabledInfo)
{
	VkInstance instance;
	VkApplicationInfo appInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = appName,
		.applicationVersion = appVersion,
		.apiVersion = appVersion
	};

	if(enabledInfo->extensionCount != 0)
		__pvkCheckForInstanceExtensionSupport(enabledInfo->extensionCount, enabledInfo->extensionNames);
	
	VkInstanceCreateInfo icInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &appInfo,
		.enabledLayerCount = enabledInfo->layerCount,
		.ppEnabledLayerNames = enabledInfo->layerNames,
		.enabledExtensionCount = enabledInfo->extensionCount,
		.ppEnabledExtensionNames = enabledInfo->extensionNames
	};
	PVK_CHECK(vkCreateInstance(&icInfo, NULL, &instance));
	return instance;
}

static VkInstance pvkCreateVulkanInstanceWithExtensions(uint32_t count, ...)
{
	const char* extensions[count];
	uint32_t extensionCount = count;

	va_list args;
	va_start(args, count);

	while(count > 0)
	{
		--count;
		extensions[count] = (char*)va_arg(args, const char*);
	}

	va_end(args);

	PvkEnabledLayerAndExtensionInfo enabledInfo = 
	{
		.extensionCount = extensionCount,
		.extensionNames = extensions
	};
	return __pvkCreateVulkanInstance("Default Vulkan App", VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_0, &enabledInfo);
}

typedef enum PvkShaderType
{
	PVK_SHADER_TYPE_VERTEX = 1UL << 0,
	PVK_SHADER_TYPE_TESSELLATION = 1UL << 1,
	PVK_SHADER_TYPE_GEOMETRY = 1UL << 2,
	PVK_SHADER_TYPE_FRAGMENT = 1UL << 3
} PvkShaderType;

typedef PvkShaderType PvkShaderFlags;

typedef struct PvkPhysicalDeviceRequirements
{
	VkPhysicalDeviceType type;		// device type
	VkFormat format; 				// surface format
	VkColorSpaceKHR colorSpace; 	// color space
	VkPresentModeKHR presentMode; 	// present mode
	PvkShaderFlags shaders; 		// required shaders
	uint32_t imageCount; 			// image count in the swapchain
	uint32_t imageWidth;			// image width in the swapchain
	uint32_t imageHeight;			// image height in the swapchain
} PvkPhysicalDeviceRequirements;

static bool __pvkIsPresentModeSupported(VkPhysicalDevice device, VkSurfaceKHR surface, VkPresentModeKHR mode)
{
	uint32_t presentModeCount;
	PVK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, NULL));
	VkPresentModeKHR* presentModes = newv(VkPresentModeKHR, presentModeCount);
	PVK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, presentModes));

	bool isPresentModeSupported = false;
	for(int i = 0; i < presentModeCount; i++)
		if(presentModes[i] == mode)
		{
			isPresentModeSupported = true;
			break;
		}
	delete(presentModes);
	if(!isPresentModeSupported)
		PVK_WARNING("Requested present mode is not supported\n");
	return isPresentModeSupported;
}

static bool __pvkIsSurfaceFormatSupported(VkPhysicalDevice device, VkSurfaceKHR surface, VkSurfaceFormatKHR surfaceFormat)
{
	uint32_t formatCount;
	PVK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, NULL));
	VkSurfaceFormatKHR* formats = newv(VkSurfaceFormatKHR, formatCount);
	PVK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, formats));

	bool isFormatSupported = false;
	for(int i = 0; i < formatCount; i++)
		if(memcmp(&formats[i], &surfaceFormat, sizeof(VkSurfaceFormatKHR)) == 0)
		{
			isFormatSupported = true;
			break;
		}
	delete(formats);
	if(!isFormatSupported)
		PVK_WARNING("Requested surface format is not supported\n");
	return isFormatSupported;
}

static bool __pvkIsDeviceTypeSupported(VkPhysicalDevice device, VkPhysicalDeviceType type)
{
	VkPhysicalDeviceProperties properties;
	vkGetPhysicalDeviceProperties(device, &properties);
	PVK_INFO("Found Device: %s\n", properties.deviceName);
	return properties.deviceType == type;
}

static bool __pvkIsDeviceFeaturesSupported(VkPhysicalDevice device, VkPhysicalDeviceFeatures* requiredFeatures)
{
	VkPhysicalDeviceFeatures features;
	vkGetPhysicalDeviceFeatures(device, &features);

	bool isSupported = true;
	isSupported &= requiredFeatures->geometryShader? (features.geometryShader && requiredFeatures->geometryShader) : true;
	isSupported &= requiredFeatures->tessellationShader? (features.tessellationShader && requiredFeatures->tessellationShader) : true;
	isSupported &= requiredFeatures->samplerAnisotropy? (features.samplerAnisotropy == requiredFeatures->samplerAnisotropy) : true;
	isSupported &= requiredFeatures->textureCompressionETC2? (features.textureCompressionETC2 == requiredFeatures->textureCompressionETC2) : true;
	if(!isSupported)
		PVK_WARNING("Requested GPU features are not supported\n");
	return isSupported;
}

static bool __pvkIsPhysicalDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface, PvkPhysicalDeviceRequirements* requirements)
{

	VkSurfaceCapabilitiesKHR capabilities;
	PVK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &capabilities));

	bool isImageSupported = true;
	isImageSupported &= (requirements->imageCount >= capabilities.minImageCount) && (requirements->imageCount <= capabilities.maxImageCount);
	isImageSupported &= (requirements->imageWidth >= capabilities.minImageExtent.width) && (requirements->imageWidth <= capabilities.maxImageExtent.width);
	isImageSupported &= (requirements->imageHeight >= capabilities.minImageExtent.height) && (requirements->imageHeight <= capabilities.maxImageExtent.height);

	VkPhysicalDeviceFeatures requiredFeatures = 
	{
		.geometryShader = (requirements->shaders & PVK_SHADER_TYPE_GEOMETRY) ? VK_TRUE : VK_FALSE,
		.tessellationShader = (requirements->shaders & PVK_SHADER_TYPE_TESSELLATION) ? VK_TRUE : VK_FALSE
	};

	return isImageSupported 
			&& __pvkIsDeviceTypeSupported(device, requirements->type)
			&& __pvkIsPresentModeSupported(device, surface, requirements->presentMode)
			&& __pvkIsSurfaceFormatSupported(device, surface, (VkSurfaceFormatKHR) { requirements->format, requirements->colorSpace })
			&& __pvkIsDeviceFeaturesSupported(device, &requiredFeatures);
}

static VkPhysicalDevice pvkGetPhysicalDevice(VkInstance instance, VkSurfaceKHR surface, 
											VkPhysicalDeviceType gpuType, 
											VkFormat format, 
											VkColorSpaceKHR colorSpace, 
											VkPresentModeKHR presentMode)
{
	uint32_t deviceCount;
	PVK_CHECK(vkEnumeratePhysicalDevices(instance, &deviceCount, NULL));
	VkPhysicalDevice* devices = newv(VkPhysicalDevice, deviceCount);
	PVK_CHECK(vkEnumeratePhysicalDevices(instance, &deviceCount, devices));

	VkPhysicalDevice device = VK_NULL_HANDLE, fallbackDevice = VK_NULL_HANDLE;

	char selectedGPUName[VK_MAX_PHYSICAL_DEVICE_NAME_SIZE];
	for(int i = 0; i < deviceCount; i++)
	{
		PvkPhysicalDeviceRequirements requirements =
		{
			.type = gpuType,
			.format = format,
			.colorSpace = colorSpace,
			.presentMode = presentMode,
			.shaders = PVK_SHADER_TYPE_GEOMETRY | PVK_SHADER_TYPE_TESSELLATION,
			.imageCount = 3,
			.imageWidth = 800,
			.imageHeight = 800
		};
		if(__pvkIsPhysicalDeviceSuitable(devices[i], surface, &requirements))
		{
			device = devices[i];
			VkPhysicalDeviceProperties properties;
			vkGetPhysicalDeviceProperties(devices[i], &properties);
			strcpy(selectedGPUName, properties.deviceName);
			break;
		}
		else
			fallbackDevice = devices[i];
	}

	if(device == VK_NULL_HANDLE)
	{
		PVK_WARNING("Unable to find the requested physical device\n");
		device = fallbackDevice;
	}
	
	VkPhysicalDeviceProperties properties;
	vkGetPhysicalDeviceProperties(device, &properties);
	strcpy(selectedGPUName, properties.deviceName);
	PVK_INFO("Selected physical device: %s\n", selectedGPUName);

	delete(devices);
	return device;
}


static uint32_t pvkFindQueueFamilyIndex(VkPhysicalDevice device, VkQueueFlags queueFlags)
{
	uint32_t count;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &count, NULL);
	VkQueueFamilyProperties* properties = newv(VkQueueFamilyProperties, count);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &count, properties);

	uint32_t index = UINT32_MAX;
	for(int i = 0; i < count; i++)
		if((properties[i].queueFlags & queueFlags) == queueFlags)
		{
			index = i;
			break;
		}
	delete(properties);

	if(index == UINT32_MAX)
		PVK_WARNING("Unable to find Queue Family with the requested QueueFlags\n");
	return index;
}

static uint32_t pvkFindQueueFamilyIndexWithPresentSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
{
	uint32_t count;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &count, NULL);

	for(int i = 0; i < count; i++)
	{
		VkBool32 supported;
		PVK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &supported));
		if(supported)
			return i;
	}
	return UINT32_MAX;
}

static void __pvkUnionUInt32(uint32_t count, const uint32_t* values, uint32_t* out_count, uint32_t* out_values)
{
	uint32_t uniqueCount = 0;

	// calculate the max value
	uint32_t maxValue = values[0];
	for(int i = 1; i < count; i++)
		if(values[i] > maxValue)
			maxValue = values[i];

	// look up table
	assert(sizeof(bool) == 1);
	bool exists[maxValue + 1]; memset(exists, false, maxValue + 1);

	for(int i = 0; i < count; i++)
	{
		if(!exists[values[i]])
		{
			out_values[uniqueCount] = values[i];
			uniqueCount++;
			exists[values[i]] = true;
		}
	}
	*out_count = uniqueCount;
}

static VkDevice pvkCreateLogicalDeviceWithExtensions(VkInstance instance, VkPhysicalDevice physicalDevice, 
													uint32_t queueFamilyCount, uint32_t* queueFamilyIndices,
													uint32_t extensionCount, ...)
{
	// union operation
	uint32_t uniqueQueueFamilyCount;
	uint32_t uniqueQueueFamilyIndices[queueFamilyCount];
	__pvkUnionUInt32(queueFamilyCount, queueFamilyIndices, &uniqueQueueFamilyCount, uniqueQueueFamilyIndices);

	float queuePriority = 1.0f;
	VkDeviceQueueCreateInfo* queueCreateInfos = newv(VkDeviceQueueCreateInfo, uniqueQueueFamilyCount);
	
	for(int i = 0; i < uniqueQueueFamilyCount; i++)
	{
		queueCreateInfos[i] = (VkDeviceQueueCreateInfo)
		{
			.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.queueFamilyIndex = uniqueQueueFamilyIndices[i],
			.queueCount = 1,
			.pQueuePriorities = &queuePriority
		};
	}

	// create extensions array from the variable arguments list
	const char* extensions[extensionCount];
	va_list args;
	va_start(args, extensionCount);
	for(int i = 0; i < extensionCount; i++)
		extensions[i] = va_arg(args, const char*);
	va_end(args);

	// check for device extension support
	uint32_t supportedExtensionCount;
	PVK_CHECK(vkEnumerateDeviceExtensionProperties(physicalDevice, NULL, &supportedExtensionCount, NULL));
	VkExtensionProperties* supportedExtensions = newv(VkExtensionProperties, supportedExtensionCount);
	PVK_CHECK(vkEnumerateDeviceExtensionProperties(physicalDevice, NULL, &supportedExtensionCount, supportedExtensions));

	for(uint32_t i = 0; i < extensionCount; i++)
	{
		bool found = false;
		for(uint32_t j = 0; j < supportedExtensionCount; j++)
		{
			if(strcmp(extensions[i], supportedExtensions[j].extensionName) == 0)
			{
				found = true;
				break;
			}
		}
		if(!found)
			PVK_FETAL_ERROR("Extension \"%s\" isn't supported by the vulkan physical device\n", extensions[i]);
	}
	delete(supportedExtensions);

	// TODO: make features configurable
	VkPhysicalDeviceFeatures features = { };

	VkDeviceCreateInfo dcInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.queueCreateInfoCount = uniqueQueueFamilyCount,
		.pQueueCreateInfos = queueCreateInfos,
		.enabledExtensionCount = extensionCount,
		.ppEnabledExtensionNames = extensions,
		.pEnabledFeatures = &features
	};
	VkDevice device;
	PVK_CHECK(vkCreateDevice(physicalDevice, &dcInfo, NULL, &device));
	delete(queueCreateInfos);
	return device;
}

static VkSwapchainKHR pvkCreateSwapchain(VkDevice device, VkSurfaceKHR surface, 
											uint32_t width, uint32_t height, 
											VkFormat format, VkColorSpaceKHR colorSpace, VkPresentModeKHR presentMode,
											uint32_t queueFamilyCount, uint32_t* queueFamilyIndices)
{
	// union operation
	uint32_t uniqueQueueFamilyCount;
	uint32_t uniqueQueueFamilyIndices[queueFamilyCount];
	__pvkUnionUInt32(queueFamilyCount, queueFamilyIndices, &uniqueQueueFamilyCount, uniqueQueueFamilyIndices);

	VkSwapchainCreateInfoKHR scInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.surface = surface,
		.minImageCount = 3,
		.imageFormat = format,
		.imageColorSpace = colorSpace,
		.imageExtent = { width, height },
		.imageArrayLayers = 1,
		.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		.imageSharingMode = (uniqueQueueFamilyCount > 1) ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = uniqueQueueFamilyCount,
		.pQueueFamilyIndices = uniqueQueueFamilyIndices,
		.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
		.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		.presentMode = presentMode,
		.clipped = VK_TRUE,
		.oldSwapchain = VK_NULL_HANDLE
	};

	VkSwapchainKHR swapchain;
	PVK_CHECK(vkCreateSwapchainKHR(device, &scInfo, NULL, &swapchain));
	return swapchain;
}

static VkCommandPool pvkCreateCommandPool(VkDevice device, VkCommandPoolCreateFlags flags, uint32_t queueFamilyIndex)
{
	VkCommandPoolCreateInfo cInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.flags = flags,
		.queueFamilyIndex = queueFamilyIndex
	};
	VkCommandPool commandPool;
	PVK_CHECK(vkCreateCommandPool(device, &cInfo, NULL, &commandPool));
	return commandPool;
}

static VkCommandBuffer* __pvkAllocateCommandBuffers(VkDevice device, VkCommandPool pool, VkCommandBufferLevel level, uint32_t count)
{
	VkCommandBufferAllocateInfo info = 
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandPool = pool,
		.level = level,
		.commandBufferCount = count
	};

	VkCommandBuffer* commandBuffers = newv(VkCommandBuffer, count);
	PVK_CHECK(vkAllocateCommandBuffers(device, &info, commandBuffers));
	return commandBuffers;
}

static inline VkCommandBuffer* pvkAllocateCommandBuffers(VkDevice device, VkCommandPool pool, VkCommandBufferLevel level)
{
	return __pvkAllocateCommandBuffers(device, pool, level, 3);
}

static VkSemaphore pvkCreateSemaphore(VkDevice device)
{
	VkSemaphoreCreateInfo cInfo = { .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
	VkSemaphore semaphore;
	PVK_CHECK(vkCreateSemaphore(device, &cInfo, NULL, &semaphore));
	return semaphore;
}


static void pvkSubmit(VkCommandBuffer commandBuffer, VkQueue queue, VkSemaphore wait, VkSemaphore signal)
{
	VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	VkSubmitInfo info =
	{
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &wait,
		.pWaitDstStageMask = &waitStage,
		.signalSemaphoreCount = 1,
		.pSignalSemaphores = &signal,
		.commandBufferCount = 1,
		.pCommandBuffers = &commandBuffer
	};
	PVK_CHECK(vkQueueSubmit(queue, 1, &info, VK_NULL_HANDLE));
}

static void pvkPresent(uint32_t index, VkSwapchainKHR  swapchain, VkQueue queue, VkSemaphore wait)
{
	VkResult result;
	VkPresentInfoKHR info = 
	{
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &wait,
		.swapchainCount = 1,
		.pSwapchains = &swapchain,
		.pImageIndices = &index,
		.pResults = &result
	};
	// assert(result == VK_SUCCESS);
	PVK_CHECK(vkQueuePresentKHR(queue, &info));
}

static void pvkBeginCommandBuffer(VkCommandBuffer commandBuffer)
{
	VkCommandBufferBeginInfo beginInfo = { .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
	PVK_CHECK(vkBeginCommandBuffer(commandBuffer, &beginInfo));
}

static inline void pvkEndCommandBuffer(VkCommandBuffer commandBuffer)
{
	PVK_CHECK(vkEndCommandBuffer(commandBuffer));
}

static void pvkBeginRenderPass(VkCommandBuffer commandBuffer, VkRenderPass renderPass, VkFramebuffer framebuffer, uint32_t width, uint32_t height)
{
	VkClearValue clearValue = { .color = { .float32 = { 0.01f, 0.02f, 0.3f, 1.0f } }  };
	VkRenderPassBeginInfo beginInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.renderPass = renderPass,
		.framebuffer = framebuffer,
		.renderArea = { .offset = { 0, 0 }, .extent = { width, height } },
		.clearValueCount = 1,
		.pClearValues = &clearValue
	};
	vkCmdBeginRenderPass(commandBuffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

static inline void pvkEndRenderPass(VkCommandBuffer commandBuffer)
{
	vkCmdEndRenderPass(commandBuffer);
}

static VkRenderPass pvkCreateRenderPass(VkDevice device)
{
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
		.attachment = 0,
		.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};

	VkSubpassDescription subpass = 
	{
		.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
		.colorAttachmentCount = 1,
		.pColorAttachments = &colorAttachmentReference,
	};

	VkRenderPassCreateInfo rInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.attachmentCount = 1, 
		.pAttachments = &colorAttachmentDescription,
		.subpassCount = 1,
		.pSubpasses = &subpass
	};
	VkRenderPass renderPass;
	PVK_CHECK(vkCreateRenderPass(device, &rInfo, NULL, &renderPass));
	return renderPass;
}

/* Vulkan Device Memory Allocation */
static VkDeviceMemory pvkAllocateMemory(VkDevice device, VkDeviceSize size, uint32_t memoryTypeIndex)
{
	VkMemoryAllocateInfo aInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.allocationSize = size,
		.memoryTypeIndex = memoryTypeIndex
	};

	VkDeviceMemory memory;
	PVK_CHECK(vkAllocateMemory(device, &aInfo, NULL, &memory));
	return memory;
}

static uint32_t __pvkGetMemoryTypeIndexFromMemoryProperty(VkPhysicalDevice device, VkMemoryPropertyFlags propertyFlags)
{
	VkPhysicalDeviceMemoryProperties properties;
	vkGetPhysicalDeviceMemoryProperties(device, &properties);
	uint32_t index = UINT32_MAX;
	for(int i = 0; i < properties.memoryTypeCount; i++)
	{
		VkMemoryPropertyFlags supportedFlags = properties.memoryTypes[i].propertyFlags;
		if((supportedFlags & propertyFlags) == propertyFlags)
			return i;
	}
	PVK_WARNING("Unable to find memory type with requested memory property flags\n");
	return index;
}

static void __pvkCheckForMemoryTypesSupport(VkPhysicalDevice device, uint32_t bits)
{
	/* TODO */
}

/* Vulkan Image & ImageView */
static VkImage __pvkCreateImage(VkDevice device, VkFormat format, uint32_t width, uint32_t height, VkImageUsageFlags usageFlags, uint32_t queueFamilyIndexCount, uint32_t* queueFamilyIndices)
{
	// union operation
	uint32_t uniqueQueueFamilyCount;
	uint32_t uniqueQueueFamilyIndices[queueFamilyIndexCount];
	__pvkUnionUInt32(queueFamilyIndexCount, queueFamilyIndices, &uniqueQueueFamilyCount, uniqueQueueFamilyIndices);
	
	VkImageCreateInfo cInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.imageType = VK_IMAGE_TYPE_2D,
		.format = format,
		.extent = { .width = width, .height = height, .depth = 1 },
		.mipLevels = 1,
		.arrayLayers = 1,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.tiling = VK_IMAGE_TILING_OPTIMAL,
		.usage = usageFlags,
		.sharingMode = (uniqueQueueFamilyCount > 1) ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = uniqueQueueFamilyCount,
		.pQueueFamilyIndices = uniqueQueueFamilyIndices
	};
	VkImage image;
	PVK_CHECK(vkCreateImage(device, &cInfo, NULL, &image));
	return image;
}

typedef struct PvkImage
{
	VkImage handle;
	VkDeviceMemory memory;
} PvkImage;

static PvkImage pvkCreateImage(VkPhysicalDevice physicalDevice, VkDevice device, VkMemoryPropertyFlags mflags, VkFormat format, uint32_t width, uint32_t height, VkImageUsageFlags usageFlags, uint32_t queueFamilyIndexCount, uint32_t* queueFamilyIndices)
{
	VkImage image = __pvkCreateImage(device, format, width, height, usageFlags, queueFamilyIndexCount, queueFamilyIndices);
	VkMemoryRequirements imageMemoryRequirements;
	vkGetImageMemoryRequirements(device, image, &imageMemoryRequirements);
	__pvkCheckForMemoryTypesSupport(physicalDevice, imageMemoryRequirements.memoryTypeBits);
	VkDeviceMemory memory = pvkAllocateMemory(device, imageMemoryRequirements.size, __pvkGetMemoryTypeIndexFromMemoryProperty(physicalDevice, mflags));
	PVK_CHECK(vkBindImageMemory(device, image, memory, 0));
	return (PvkImage) { image, memory };
}

static void pvkDestroyImage(VkDevice device, PvkImage image)
{
	vkDestroyImage(device, image.handle, NULL);
	vkFreeMemory(device, image.memory, NULL);
}

static VkImageView pvkCreateImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlagBits aspectMask)
{
	VkImageViewCreateInfo cInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.image = image,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format = format,
		.components = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY },
		.subresourceRange = 
		{
			.aspectMask = aspectMask,
			.levelCount = 1,
			.layerCount = 1
		}
	};
	VkImageView imageView;
	PVK_CHECK(vkCreateImageView(device, &cInfo, NULL, &imageView));
	return imageView;
}

static VkImageView* pvkCreateSwapchainImageViews(VkDevice device, VkSwapchainKHR swapchain, VkFormat format)
{
	// get the swapchain images
	uint32_t imageCount;
	PVK_CHECK(vkGetSwapchainImagesKHR(device, swapchain, &imageCount, NULL));
	assert(imageCount == 3);
	VkImage* images = newv(VkImage, imageCount);
	PVK_CHECK(vkGetSwapchainImagesKHR(device, swapchain, &imageCount, images));

	VkImageView* imageViews = newv(VkImageView, imageCount);
	for(int i = 0; i < imageCount; i++)
		imageViews[i] = pvkCreateImageView(device, images[i], format, VK_IMAGE_ASPECT_COLOR_BIT);

	delete(images);
	return imageViews;
}

static void pvkDestroySwapchainImageViews(VkDevice device, VkSwapchainKHR swapchain, VkImageView* imageViews)
{
	uint32_t imageCount;
	PVK_CHECK(vkGetSwapchainImagesKHR(device, swapchain, &imageCount, NULL));
	for(int i = 0; i < imageCount; i++)
		vkDestroyImageView(device, imageViews[i], NULL);
	delete(imageViews);
}

/* Vulkan Frambuffer */
static VkFramebuffer* pvkCreateFramebuffers(VkDevice device, VkRenderPass renderPass, uint32_t width, uint32_t height, uint32_t imageViewCount, VkImageView* imageViews)
{
	VkFramebuffer* framebuffers = newv(VkFramebuffer, 3);
	for(int i = 0; i < 3; i++)
	{
		VkFramebufferCreateInfo fInfo = 
		{
			.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			.renderPass = renderPass,
			.attachmentCount = imageViewCount,
			.pAttachments = &imageViews[imageViewCount * i],
			.width = width,
			.height = height,
			.layers = 1
		};
		PVK_CHECK(vkCreateFramebuffer(device, &fInfo, NULL, &framebuffers[i]));
	}
	return framebuffers;
}

static void pvkDestroyFramebuffers(VkDevice device, VkFramebuffer* framebuffers)
{
	for(int i = 0; i < 3; i++)
		vkDestroyFramebuffer(device, framebuffers[i], NULL);
}

/* Shaders & Graphics Pipeline */
static const char* __pvkLoadBinaryFile(const char* filePath, size_t* out_length)
{
	FILE* file = fopen(filePath, "rb");
	if(file == NULL)
		PVK_FETAL_ERROR("Unable to open the file at path \"%s\"\n", filePath);
	int result = fseek(file, 0, SEEK_END);
	assert(result == 0);
	size_t length = ftell(file);
	if(length == 0)
		PVK_WARNING("File at path \"%s\" is empty\n", filePath);
	rewind(file);
	char* data = newv(char, length);
	size_t readLength = fread(data, 1, length, file);
	assert(readLength == length);
	fclose(file);
	if(out_length != NULL)
		*out_length = length;
	return data;
}

static VkShaderModule pvkCreateShaderModule(VkDevice device, const char* filePath)
{
	size_t length;
	const char* bytes = __pvkLoadBinaryFile(filePath, &length);
	assert((length % 4) == 0);
	VkShaderModuleCreateInfo cInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.codeSize = (uint32_t)length,
		.pCode = (uint32_t*)bytes,
	};
	VkShaderModule shaderModule;
	PVK_CHECK(vkCreateShaderModule(device, &cInfo, NULL, &shaderModule));
	delete(bytes);
	return shaderModule;
}

typedef struct PvkShader
{
	VkShaderModule handle;
	PvkShaderType type;
} PvkShader;

static VkShaderStageFlagBits __pkvShaderTypeToVulkanShaderStage(PvkShaderType type)
{
	VkShaderStageFlagBits flags = 0;
	if(type & PVK_SHADER_TYPE_VERTEX)
		flags |= VK_SHADER_STAGE_VERTEX_BIT;
	if(type & PVK_SHADER_TYPE_TESSELLATION)
		flags |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
	if(type & PVK_SHADER_TYPE_GEOMETRY)
		flags |= VK_SHADER_STAGE_GEOMETRY_BIT;
	if(type & PVK_SHADER_TYPE_FRAGMENT)
		flags |= VK_SHADER_STAGE_FRAGMENT_BIT;
	return flags;
}

static VkPipelineShaderStageCreateInfo* __pvkCreatePipelineShaderStageCreateInfos(uint32_t count, const PvkShader* const shaders)
{
	VkPipelineShaderStageCreateInfo* infos = newv(VkPipelineShaderStageCreateInfo, count);
	for(int i = 0; i < count; i++)
	{
		infos[i] = (VkPipelineShaderStageCreateInfo)
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.module = shaders[i].handle,
			.pName = "main",
			.stage = __pkvShaderTypeToVulkanShaderStage(shaders[i].type)	
		};
	}
	return infos;
}


typedef struct PvkVertex
{
	// attribute at location = 0 : position of the vertex
	struct 
	{
		float x, y, z;
	} position;

	// attribute at location = 1 : normal of the vertex
	struct
	{
		float x, y, z;
	} normal;

	// attribute at location = 2 : texture coordinates of the vertex
	union 
	{
		struct 
		{
			float x, y;
		};
		struct 
		{
			float s, t;
		};
	} texcoord;

	// attribute at location = 3 : color of the vertex
	struct
	{
		float r, g, b, a;
	} color;

} PvkVertex;

#define PVK_VERTEX_SIZE sizeof(PvkVertex)
#define PVK_VERTEX_ATTRIBUTE_COUNT 4
#define PVK_VERTEX_POSITION_OFFSET offsetof(PvkVertex, position)
#define PVK_VERTEX_NORMAL_OFFSET offsetof(PvkVertex, normal)
#define PVK_VERTEX_TEXCOORD_OFFSET offsetof(PvkVertex, texcoord)
#define PVK_VERTEX_COLOR_OFFSET offsetof(PvkVertex, color)

static VkVertexInputAttributeDescription __pvkGetVertexInputAttributeDescription(uint32_t binding, uint32_t location, VkFormat format, uint32_t offset)
{
	return (VkVertexInputAttributeDescription)
	{
		.location = location,
		.binding = binding,
		.format = format,
		.offset = offset
	};
}

static VkPipeline pvkCreateGraphicsPipeline(VkDevice device, VkPipelineLayout layout, VkRenderPass renderPass, uint32_t subpassIndex, uint32_t width, uint32_t height, uint32_t count, ...)
{
	/* Shader modules */
	PvkShader shaderModules[count];
	va_list args;
	va_start(args, count);
	for(int i = 0; i < count; i++)
		shaderModules[i] = va_arg(args, PvkShader);
	va_end(args);
	VkPipelineShaderStageCreateInfo* stageCInfos = __pvkCreatePipelineShaderStageCreateInfos(count, shaderModules);

	/* Vertex buffer layouts and their description */
	VkVertexInputBindingDescription vertexBindingDescription = 
	{
		.binding = 0,
		.stride = PVK_VERTEX_SIZE,
		.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
	};
	VkVertexInputAttributeDescription* vertexAttributeDescriptions = newv(VkVertexInputAttributeDescription, PVK_VERTEX_ATTRIBUTE_COUNT);
	vertexAttributeDescriptions[0] = __pvkGetVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, PVK_VERTEX_POSITION_OFFSET);
	vertexAttributeDescriptions[1] = __pvkGetVertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32B32A32_SFLOAT, PVK_VERTEX_NORMAL_OFFSET);
	vertexAttributeDescriptions[2] = __pvkGetVertexInputAttributeDescription(0, 2, VK_FORMAT_R32G32_SFLOAT, PVK_VERTEX_TEXCOORD_OFFSET);
	vertexAttributeDescriptions[3] = __pvkGetVertexInputAttributeDescription(0, 3, VK_FORMAT_R32G32B32A32_SFLOAT, PVK_VERTEX_COLOR_OFFSET);

	VkPipelineVertexInputStateCreateInfo vertexInputStateCInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		.vertexBindingDescriptionCount = 1,
		.pVertexBindingDescriptions = &vertexBindingDescription,
		.vertexAttributeDescriptionCount = PVK_VERTEX_ATTRIBUTE_COUNT,
		.pVertexAttributeDescriptions = vertexAttributeDescriptions
	};

	/* Primitive assembly */
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		.primitiveRestartEnable = VK_FALSE
	};

	/* Viewport configuration */
	VkViewport viewport = 
	{
		.x = 0,
		.y = 0,
		.width = width,
		.height = height,
		.minDepth = 0,
		.maxDepth = 1.0f
	};
	VkRect2D scissor = 
	{
		.offset = { 0, 0 },
		.extent = { width, height }
	};

	VkPipelineViewportStateCreateInfo viewportStateCInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.viewportCount = 1,
		.pViewports = &viewport,
		.scissorCount = 1,
		.pScissors = &scissor
	};

	/* Rasterization configuration */
	VkPipelineRasterizationStateCreateInfo rasterizationStateCInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		.depthClampEnable = VK_FALSE,
		.rasterizerDiscardEnable = VK_FALSE,
		.polygonMode = VK_POLYGON_MODE_FILL,
		.cullMode = VK_CULL_MODE_BACK_BIT,
		.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
		.depthBiasEnable = VK_FALSE
	};

	/* Multisampling */
	VkPipelineMultisampleStateCreateInfo multisamplingStateCInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
		.sampleShadingEnable = VK_FALSE
	};

	/* Depth stencil configuration */
	VkPipelineDepthStencilStateCreateInfo dephtStencilStateCInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
		.depthTestEnable = VK_FALSE,
		.depthWriteEnable = VK_FALSE,
		.stencilTestEnable = VK_FALSE
	};

	/* Color attachment configuration */
	VkPipelineColorBlendAttachmentState colorAttachment = 
	{
		.blendEnable = VK_FALSE,
		.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT	
	};

	VkPipelineColorBlendStateCreateInfo colorBlendStateCInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.logicOpEnable = VK_FALSE,
		.attachmentCount = 1,
		.pAttachments = &colorAttachment
	};

	VkGraphicsPipelineCreateInfo pipelineCInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.stageCount = count,
		.pStages = stageCInfos,
		.pVertexInputState = &vertexInputStateCInfo,
		.pInputAssemblyState = &inputAssemblyStateCInfo,
		.pViewportState = &viewportStateCInfo,
		.pRasterizationState = &rasterizationStateCInfo,
		.pDepthStencilState = &dephtStencilStateCInfo,
		.pColorBlendState = &colorBlendStateCInfo,
		.layout = layout,
		.renderPass = renderPass,
		.subpass = subpassIndex
	};

	VkPipeline pipeline;
	PVK_CHECK(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineCInfo, NULL, &pipeline));

	delete(stageCInfos);
	delete(vertexAttributeDescriptions);
	return pipeline;
}


static VkPipelineLayout pvkCreatePipelineLayout(VkDevice device, uint32_t setLayoutCount, VkDescriptorSetLayout* setLayouts)
{
	VkPipelineLayoutCreateInfo cInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.setLayoutCount = setLayoutCount,
		.pSetLayouts = setLayouts
	};
	VkPipelineLayout layout;
	PVK_CHECK(vkCreatePipelineLayout(device, &cInfo, NULL, &layout));
	return layout;
}

/* Vulkan Buffer */
static VkBuffer __pvkCreateBuffer(VkDevice device, VkBufferUsageFlags usageFlags, VkDeviceSize size, uint32_t queueFamilyCount, uint32_t* queueFamilyIndices)
{
	// union operation
	uint32_t uniqueQueueFamilyCount;
	uint32_t uniqueQueueFamilyIndices[queueFamilyCount];
	__pvkUnionUInt32(queueFamilyCount, queueFamilyIndices, &uniqueQueueFamilyCount, uniqueQueueFamilyIndices);

	VkBufferCreateInfo cInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.usage = usageFlags,
		.size = size,
		.queueFamilyIndexCount = uniqueQueueFamilyCount,
		.pQueueFamilyIndices = uniqueQueueFamilyIndices,
		.sharingMode = (uniqueQueueFamilyCount > 1) ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE
	};

	VkBuffer buffer;
	PVK_CHECK(vkCreateBuffer(device, &cInfo, NULL, &buffer));
	return buffer;
}

typedef struct PvkBuffer
{
	VkBuffer handle;
	VkDeviceMemory memory;
} PvkBuffer;


static PvkBuffer pvkCreateBuffer(VkPhysicalDevice physicalDevice, VkDevice device, VkMemoryPropertyFlags mflags, VkBufferUsageFlags flags, VkDeviceSize size, uint32_t queueFamilyCount, uint32_t* queueFamilyIndices)
{
	VkBuffer buffer = __pvkCreateBuffer(device, flags, size, queueFamilyCount, queueFamilyIndices);
	VkMemoryRequirements bufferMemoryRequirements;
	vkGetBufferMemoryRequirements(device, buffer, &bufferMemoryRequirements);
	__pvkCheckForMemoryTypesSupport(physicalDevice, bufferMemoryRequirements.memoryTypeBits);
	VkDeviceMemory memory = pvkAllocateMemory(device, bufferMemoryRequirements.size, 
												__pvkGetMemoryTypeIndexFromMemoryProperty(physicalDevice, mflags));	
	PVK_CHECK(vkBindBufferMemory(device, buffer, memory, 0));
	return (PvkBuffer) { buffer, memory };
}

static void pvkDestroyBuffer(VkDevice device, PvkBuffer buffer)
{
	vkDestroyBuffer(device, buffer.handle, NULL);
	vkFreeMemory(device, buffer.memory, NULL);
}

static void pvkUploadToBuffer(VkDevice device, PvkBuffer buffer, void* data, size_t size)
{
	void* dst;
	PVK_CHECK(vkMapMemory(device, buffer.memory, 0, size, 0, &dst));
	memcpy(dst, data, size);
	vkUnmapMemory(device, buffer.memory);
}

/* Vulkan Descriptor sets */

static VkDescriptorSet* pvkAllocateDescriptorSets(VkDevice device, VkDescriptorPool pool, uint32_t setCount, VkDescriptorSetLayout* setLayouts)
{
	VkDescriptorSetAllocateInfo allocInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.descriptorPool = pool,
		.descriptorSetCount = setCount,
		.pSetLayouts = setLayouts
	};

	VkDescriptorSet* sets = newv(VkDescriptorSet, setCount);
	PVK_CHECK(vkAllocateDescriptorSets(device, &allocInfo, sets));
	return sets;
}

static void pvkWriteInputAttachmentToDescriptor(VkDevice device, VkDescriptorSet set, uint32_t binding, VkImageView imageView)
{
	VkDescriptorImageInfo imageInfo = 
	{
		.imageView = imageView,
		.sampler = VK_NULL_HANDLE,
		.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
	};

	VkWriteDescriptorSet writeInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
		.dstSet = set,
		.dstBinding = binding,
		.dstArrayElement = 0,
		.descriptorCount = 1,
		.descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
		.pImageInfo = &imageInfo
	};

	vkUpdateDescriptorSets(device, 1, &writeInfo, 0, NULL);
}


/* Geometry */
typedef uint16_t PvkIndex;

typedef struct PvkGeometryData
{
	PvkVertex* vertices;
	PvkIndex* indices;
	uint32_t vertexCount;
	uint32_t indexCount;
} PvkGeometryData;

typedef struct PvkGeometry
{
	PvkBuffer vertexBuffer;
	PvkBuffer indexBuffer;
	uint16_t indexCount;
} PvkGeometry;

PvkGeometry* __pvkCreateGeometry(VkPhysicalDevice physicalDevice, VkDevice device, uint16_t queueFamilyIndexCount, uint32_t* queueFamilyIndices, PvkGeometryData* data)
{
	uint64_t vertexBufferSize = sizeof(PvkVertex) * data->vertexCount;
	uint64_t indexBufferSize = sizeof(PvkIndex) * data->indexCount;
	PvkBuffer vertexBuffer = pvkCreateBuffer(physicalDevice, device, 
												VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
												VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vertexBufferSize, queueFamilyIndexCount, queueFamilyIndices);
	PvkBuffer indexBuffer = pvkCreateBuffer(physicalDevice, device, 
												VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
												VK_BUFFER_USAGE_INDEX_BUFFER_BIT, indexBufferSize, queueFamilyIndexCount, queueFamilyIndices);
	pvkUploadToBuffer(device, vertexBuffer, data->vertices, vertexBufferSize);
	pvkUploadToBuffer(device, indexBuffer, data->indices, indexBufferSize);
	PvkGeometry* geometry = new(PvkGeometry);
	geometry->vertexBuffer = vertexBuffer;
	geometry->indexBuffer = indexBuffer;
	geometry->indexCount = 36;
	return geometry;
}

PvkGeometry* pvkCreateBoxGeometry(VkPhysicalDevice physicalDevice, VkDevice device, uint32_t queueFamilyIndexCount, uint32_t* queueFamilyIndices, float size)
{
	PvkVertex vertices[4] = 
	{
		{ { -0.5f * size, -0.5f * size },  { }, { }, { 0, 1, 1, 1 } },
		{ { 0.5f * size, -0.5f * size },   { }, { }, { 1, 1, 1, 1 } },
		{ { 0.5f * size, 0.5f * size },  { }, { }, { 1, 0, 1, 1 } },
		{ { -0.5f * size, 0.5f * size }, { }, { }, { 1, 1, 0, 1 } }
	};

	PvkIndex indices[6] = 
	{
		2, 1, 0,
		0, 3, 2
	};
	PvkGeometryData geometryData = { .vertices = vertices, .vertexCount = 4, .indices = indices, .indexCount = 6 };
	return __pvkCreateGeometry(physicalDevice, device, queueFamilyIndexCount, queueFamilyIndices, &geometryData);
}

void pvkDrawGeometry(VkCommandBuffer cb, PvkGeometry* geometry)
{
	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(cb, 0, 1, &geometry->vertexBuffer.handle, &offset);
	vkCmdBindIndexBuffer(cb, geometry->indexBuffer.handle, 0, VK_INDEX_TYPE_UINT16);
	vkCmdDrawIndexed(cb, geometry->indexCount, 1, 0, 0, 0);
}

void pvkDestroyGeometry(VkDevice device, PvkGeometry* geometry)
{
	pvkDestroyBuffer(device, geometry->vertexBuffer);
	pvkDestroyBuffer(device, geometry->indexBuffer);
	delete(geometry);
}
