
#pragma once

#include <vulkan/vulkan.h> 	// for vulkan

#include <stdint.h> 		// standard int types
#include <stdio.h> 			// printf and puts
#include <stdbool.h>	 	// bool
#include <stdlib.h> 		// malloc
#include <string.h> 		// memset
#include <stdarg.h> 		// va_list, va_start, va_end, va_arg
#include <assert.h> 		// assert()

/* Memory functions */
#define new(type) (type*)__new(sizeof(type))
#define newv(type, count) (type*)__new(sizeof(type) * count)
static void* __new(size_t size) { void* block = malloc(size); memset(block, 0, size); return block; }
#define delete(ptr) __delete((char**)&(ptr))
static void __delete(char** ptr) 
{ 
	if(*ptr == NULL) {  puts("[Warning] You are trying to free an invalid memory block, ptr = NULL"); return;  }
	free((void*)(*ptr)); 
	*ptr = NULL; 
}

/* Resulting checking */
#define PVK_CHECK(result) pvkCheckResult(result, __LINE__, __FUNCTION__, __FILE__)

static void pvkCheckResult(VkResult result, uint32_t line_no, const char* function_name, const char* source_name)
{
	if(result != VK_SUCCESS)
	{
		printf("Result != VK_SUCCESS, Result = %lld , at %lu, %s, %s\n", result, line_no, function_name, source_name);
		exit(0);
	}
}

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
	if(full_screen)
		window->handle = glfwCreateWindow(width, height, title, glfwGetPrimaryMonitor(), NULL);
	else
		window->handle = glfwCreateWindow(width, height, title, NULL, NULL);
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
	printf("Found Device: %s\n", properties.deviceName);
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
		if(values[i] < maxValue)
			maxValue = values[i];

	// look up table
	bool exists[maxValue + 1]; memset(exists, false, maxValue + 1);

	for(int i = 0; i < count; i++)
	{
		if(!exists[values[i]])
		{
			out_values[uniqueCount++] = values[i];
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
		.pEnabledFeatures = &features,
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
	VkSubmitInfo info =
	{
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &wait,
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
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &wait,
		.swapchainCount = 1,
		.pSwapchains = &swapchain,
		.pImageIndices = &index,
		.pResults = &result
	};
	assert(result == VK_SUCCESS);
	PVK_CHECK(vkQueuePresentKHR(queue, &info));
}
