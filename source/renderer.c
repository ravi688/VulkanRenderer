

#include <engine/renderer/renderer.h>
#include <exception/exception.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string/string.h>		//custom string library
#include <string.h>				//standard string library
#include <garbage_collector/garbage_collector.h>

declare_exception(VULKAN_ABORTED);
define_exception(VULKAN_ABORTED);
declare_exception(VULKAN_LAYER_NOT_SUPPORTED);
define_exception(VULKAN_LAYER_NOT_SUPPORTED);
declare_exception(VULKAN_EXTENSION_NOT_SUPPORTED);
define_exception(VULKAN_EXTENSION_NOT_SUPPORTED);
define_exception(VULKAN_DEVICE_NOT_FOUND);
declare_exception(VULKAN_DEVICE_NOT_FOUND);

#define vkCall(call)\
{\
	VkResult result =  call;\
	EXCEPTION_BLOCK\
	(\
		if(result != VK_SUCCESS)\
			throw_exception(VULKAN_ABORTED);\
	)\
}

static void vk_setup_validation_layers();

renderer_t* renderer_init()
{
	renderer_t* renderer = GC_NEW(renderer_t);
	VkInstance instance = renderer->vk_instance = vk_create_instance();

#ifdef ENABLE_VALIDATION_LAYERS
	vk_setup_validation_layers();
#endif

	//TODO: this should be something like this: 
	//		VkPhysicalDevice vulkan_device = vk_get_suitable_vulkan_device();
	tuple_t(uint32_t, pVkPhysicalDevice_t) physical_devices = vk_get_physical_devices(instance); 
	VkPhysicalDevice vulkan_device = vk_select_suitable_device(physical_devices);

	int32_t graphics_queue_index = -1;
	tuple_t(uint32_t, pVkQueueFamilyProperties_t) queue_families = vk_get_queue_family_properties(vulkan_device); 
	for(uint32_t i = 0; i < queue_families.value1; i++)
	{
		VkQueueFamilyProperties properties = queue_families.value2[i];
		if(properties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			graphics_queue_index = i;
			break;
		}
	}

	VkDeviceQueueCreateInfo queueCreateInfo = { }; 
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = graphics_queue_index;
	queueCreateInfo.queueCount = 1;
	float queuePriorities = 1.0f;
	queueCreateInfo.pQueuePriorities = &queuePriorities;

	VkPhysicalDeviceFeatures features =  { }; 
	VkDeviceCreateInfo logicalDeviceCreateInfo =  { }; 
	logicalDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO; 
	logicalDeviceCreateInfo.pQueueCreateInfos = &queueCreateInfo; 
	logicalDeviceCreateInfo.queueCreateInfoCount = 1;
	logicalDeviceCreateInfo.pEnabledFeatures = &features;
	logicalDeviceCreateInfo.enabledExtensionCount = 0; 
	logicalDeviceCreateInfo.enabledLayerCount = 0;

	VkDevice logicalDevice;
	vkCall(vkCreateDevice(vulkan_device, &logicalDeviceCreateInfo, NULL, &logicalDevice));

	renderer->vk_device = logicalDevice;

	VkQueue graphicsQueue;
	vkGetDeviceQueue(logicalDevice, graphics_queue_index, 0, &graphicsQueue);



#ifdef DEBUG
	vk_dump_instance_extensions(); 
	vk_dump_instance_layers();
	vk_dump_physical_devices(&physical_devices);
	vk_dump_queue_families(&queue_families);
#endif
	return renderer;
}

void renderer_terminate(renderer_t* renderer)
{
	vkDestroyDevice(renderer->vk_device, NULL);
	vkDestroyInstance(renderer->vk_instance, NULL);
}


VkPhysicalDevice vk_select_suitable_device(tuple_t(uint32_t, pVkPhysicalDevice_t) physical_devices)
{
	EXCEPTION_BLOCK(
		if(physical_devices.value1 <= 0)
			throw_exception(VULKAN_DEVICE_NOT_FOUND);
	);
	return physical_devices.value2[0];
}

bool vk_check_layer_support(tuple_t(uint32_t, ppVkChar_t) layers)
{
	tuple_t(uint32_t, pVkLayerProperties_t) layer_properties = vk_get_instance_layer_properties(); 
	for(uint32_t i = 0; i < layers.value1; i++)
	{
		bool contains = false;
		for(uint32_t j = 0; j < layer_properties.value1; j++)
		{
			if(strcmp(layers.value2[i], layer_properties.value2[j].layerName) == 0)
			{
				contains = true;
				break;
			}
		}
		if(!contains)
			return false;
	}
	return true;
}


bool vk_check_extension_support(tuple_t(uint32_t, ppVkChar_t) extensions)
{
	tuple_t(uint32_t, pVkExtensionProperties_t) extension_properties = vk_get_instance_extension_properties();
	for(uint32_t i = 0; i < extensions.value1; i++)
	{
		bool contains = false; 
		for(uint32_t j = 0; j < extension_properties.value1; j++)
		{
			if(strcmp(extensions.value2[i], extension_properties.value2[j].extensionName) == 0)
			{
				contains = true; 
				break;
			}
		}
		if(!contains)
			return false;
	}
	return true;
}

VkInstance vk_create_instance()
{
	VkInstanceCreateInfo instance_create_info =  { };
	instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO; 
	instance_create_info.pNext = NULL; 
	instance_create_info.flags = 0; 
	instance_create_info.pApplicationInfo = NULL; 
	instance_create_info.enabledLayerCount = 0;
	instance_create_info.ppEnabledLayerNames = NULL; 
	instance_create_info.enabledExtensionCount = 0; 
	instance_create_info.ppEnabledExtensionNames = NULL; 
	VkInstance instance; 
	vkCall(vkCreateInstance(&instance_create_info, NULL, &instance));
	return instance;
}

tuple_t(uint32_t, pVkPhysicalDevice_t) vk_get_physical_devices(VkInstance instance)
{
	tuple_t(uint32_t, pVkPhysicalDevice_t) pair;
	vkCall(vkEnumeratePhysicalDevices(instance, &(pair.value1), NULL)); 
	pair.value2 = (pVkPhysicalDevice_t)GC_ALLOC(sizeof(VkPhysicalDevice) * pair.value1);
	vkCall(vkEnumeratePhysicalDevices(instance, &(pair.value1), pair.value2));
	return pair;
}

tuple_t(uint32_t, pVkExtensionProperties_t) vk_get_instance_extension_properties()
{
	tuple_t(uint32_t, pVkExtensionProperties_t) pair;
	vkCall(vkEnumerateInstanceExtensionProperties(NULL, &(pair.value1), NULL));
	pair.value2 = GC_NEWV(VkExtensionProperties, pair.value1);
	vkCall(vkEnumerateInstanceExtensionProperties(NULL, &(pair.value1), pair.value2));
	return pair;
}

tuple_t(uint32_t, pVkLayerProperties_t) vk_get_instance_layer_properties()
{
	tuple_t(uint32_t, pVkLayerProperties_t) pair; 
	vkCall(vkEnumerateInstanceLayerProperties(&(pair.value1), NULL)); 
	pair.value2 = GC_NEWV(VkLayerProperties, pair.value1);
	vkCall(vkEnumerateInstanceLayerProperties(&(pair.value1), pair.value2));
	return pair;
}

VkPhysicalDeviceProperties vk_get_physical_device_properties(VkPhysicalDevice physical_device)
{
	VkPhysicalDeviceProperties properties; 
	vkGetPhysicalDeviceProperties(physical_device, &properties);
	return properties;
}

VkPhysicalDeviceFeatures vk_get_physical_device_features(VkPhysicalDevice physical_device)
{
	VkPhysicalDeviceFeatures features;
	vkGetPhysicalDeviceFeatures(physical_device, &features); 
	return features;
}

VkPhysicalDeviceMemoryProperties vk_get_physical_device_memory_properties(VkPhysicalDevice physical_device)
{
	VkPhysicalDeviceMemoryProperties memory_properties;
	vkGetPhysicalDeviceMemoryProperties(physical_device, &memory_properties);
	return memory_properties;
}

tuple_t(uint32_t, pVkQueueFamilyProperties_t) vk_get_queue_family_properties(VkPhysicalDevice physical_device)
{
	tuple_t(uint32_t, pVkQueueFamilyProperties_t) pair; 
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &(pair.value1), NULL);
	pair.value2 = GC_NEWV(VkQueueFamilyProperties, pair.value1); 
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &(pair.value1), pair.value2); 
	return pair;
}

static void vk_setup_validation_layers()
{
	EXCEPTION_BLOCK
	(
		if(!vk_check_layer_support((tuple_t(uint32_t, ppVkChar_t)){ 1, string_array(1, "VK_LAYER_KHRONOS_validation") }))
			throw_exception(VULKAN_LAYER_NOT_SUPPORTED);

		if(!vk_check_extension_support((tuple_t(uint32_t, ppVkChar_t)) { 1, string_array(1, "My Extension") } ))
			throw_exception(VULKAN_EXTENSION_NOT_SUPPORTED);
	);
}


void vk_dump_physical_devices(tuple_t(uint32_t, pVkPhysicalDevice_t)* physical_devices)
{
	for(uint32_t i = 0; i < physical_devices->value1; i++)
	{
		VkPhysicalDevice device = physical_devices->value2[i];
		VkPhysicalDeviceProperties properties = vk_get_physical_device_properties(device);
		VkPhysicalDeviceMemoryProperties memory_properties = vk_get_physical_device_memory_properties(device);
		VkPhysicalDeviceFeatures features = vk_get_physical_device_features(device); 

		puts("Physical Device Properties----------------------");
		puts(vk_physical_device_properties_to_string(&properties));
		puts("Physical Device Memory Properties---------------");
		puts(vk_physical_device_memory_properties_to_string(&memory_properties));
		puts("Physical Device Features------------------------"); 
		puts(vk_physical_device_features_to_string(&features));
	}
}

void vk_dump_queue_families(tuple_t(uint32_t, pVkQueueFamilyProperties_t)* queue_families)
{
	puts("Physical Device Queue Family::QueueFlags---------");
	for(uint32_t i = 0; i < queue_families->value1; i++)
		puts(vk_physical_device_queue_family_to_string(queue_families->value2[i]));
}

void vk_dump_instance_layers()
{
	tuple_t(uint32_t, pVkLayerProperties_t) layer_properties = vk_get_instance_layer_properties();
	puts("Instance Layer Properties----------------------"); 
	for(uint32_t i = 0; i < layer_properties.value1; i++)
		puts(layer_properties.value2[i].layerName);
}

void vk_dump_instance_extensions()
{
	tuple_t(uint32_t, pVkExtensionProperties_t) extension_properties = vk_get_instance_extension_properties();
	puts("Instance Extension Properties----------------------");
	for(uint32_t i = 0; i < extension_properties.value1; i++)
		puts(extension_properties.value2[i].extensionName);
}


const char* vk_physical_device_queue_family_to_string(VkQueueFamilyProperties properties)
{
	char* buffer = GC_NEWV(char, 1024);		//1KB
	sprintf(buffer, "Queue Count: %u, ", properties.queueCount);
	strcat(buffer, "Queue Flags: ");
	if(properties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		strcat(buffer, "[VK_QUEUE_GRAPHICS_BIT]"); 
	if(properties.queueFlags & VK_QUEUE_COMPUTE_BIT)
		strcat(buffer, "[VK_QUEUE_COMPUTE_BIT]"); 
	if(properties.queueFlags & VK_QUEUE_TRANSFER_BIT)
		strcat(buffer, "[VK_QUEUE_TRANSFER_BIT]");
	strcat(buffer, ", ");
	VkExtent3D extents = properties.minImageTransferGranularity;
	sprintf(buffer + strlen(buffer), "minImageTransferGranularity: (%u, %u, %u)", extents.width, extents.height, extents.depth);
	return buffer;
}

const char* vk_physical_device_memory_properties_to_string(VkPhysicalDeviceMemoryProperties* memory_properties)
{
	return string(128, 
		"Memory Type Count: %u\n", 
		memory_properties->memoryTypeCount
		);
}

const char* vk_physical_device_limits_to_string(VkPhysicalDeviceLimits* device_limits)
{
	return string(512, 
		"Max Image Dimension 1D [width]: %u\n"
		"Max Image Dimension 2D max(width, height): %u\n"
		"Max Image Dimension 3D max(width, height, depth): %u\n"
		"Max Image Dimension Cube max(width, height): %u\n"
		"Max Image Layers: %u\n"
		"Max Texel Buffer Elements: %u\n"
		,
		device_limits->maxImageDimension1D,
		device_limits->maxImageDimension2D, 
		device_limits->maxImageDimension3D, 
		device_limits->maxImageDimensionCube, 
		device_limits->maxImageArrayLayers,
		device_limits->maxTexelBufferElements
		// device_limits->maxUniformBufferRange, 
		// device_limits->maxStorageBufferRange, 
		// device_limits->maxPushConstantsSize,
		// device_limits->maxMemoryAllocationCount, 
		// device_limits->maxSamplerAllocationCount, 
		// device_limits->bufferImageGranularity,
		// device_limits->sparseAddressSpaceSize, 
		// device_limits->maxBoundDescriptorSets,
		// device_limits->maxPerStageDescriptorSampler,
		);
}

const char* vk_physical_device_features_to_string(VkPhysicalDeviceFeatures* device_features)
{
	//2KB of memory
	return string(2048,
	"robustBufferAccess = %s\n"
    "fullDrawIndexUint32 = %s\n"
    "imageCubeArray = %s\n"
    "independentBlend = %s\n"
    "geometryShader = %s\n"
    "tessellationShader = %s\n"
    "sampleRateShading = %s\n"
    "dualSrcBlend = %s\n"
    "logicOp = %s\n"
    "multiDrawIndirect = %s\n"
    "drawIndirectFirstInstance = %s\n"
    "depthClamp = %s\n"
    "depthBiasClamp = %s\n"
    "fillModeNonSolid = %s\n"
    "depthBounds = %s\n"
    "wideLines = %s\n"
    "largePoints = %s\n"
    "alphaToOne = %s\n"
    "multiViewport = %s\n"
    "samplerAnisotropy = %s\n"
    "textureCompressionETC2 = %s\n"
    "textureCompressionASTC_LDR = %s\n"
    "textureCompressionBC = %s\n"
    "occlusionQueryPrecise = %s\n"
    "pipelineStatisticsQuery = %s\n"
    "vertexPipelineStoresAndAtomics = %s\n"
    "fragmentStoresAndAtomics = %s\n"
    "shaderTessellationAndGeometryPointSize = %s\n"
    "shaderImageGatherExtended = %s\n"
    "shaderStorageImageExtendedFormats = %s\n"
    "shaderStorageImageMultisample = %s\n"
    "shaderStorageImageReadWithoutFormat = %s\n"
    "shaderStorageImageWriteWithoutFormat = %s\n"
    "shaderUniformBufferArrayDynamicIndexing = %s\n"
    "shaderSampledImageArrayDynamicIndexing = %s\n"
    "shaderStorageBufferArrayDynamicIndexing = %s\n"
    "shaderStorageImageArrayDynamicIndexing = %s\n"
    "shaderClipDistance = %s\n"
    "shaderCullDistance = %s\n"
    "shaderFloat64 = %s\n"
    "shaderInt64 = %s\n"
    "shaderInt16 = %s\n"
    "shaderResourceResidency = %s\n"
    "shaderResourceMinLod = %s\n"
    "sparseBinding = %s\n"
    "sparseResidencyBuffer = %s\n"
    "sparseResidencyImage2D = %s\n"
    "sparseResidencyImage3D = %s\n"
    "sparseResidency2Samples = %s\n"
    "sparseResidency4Samples = %s\n"
    "sparseResidency8Samples = %s\n"
    "sparseResidency16Samples = %s\n"
    "sparseResidencyAliased = %s\n"
    "variableMultisampleRate = %s\n"
    "inheritedQueries = %s\n"
	,string_bool(device_features->robustBufferAccess)
    ,string_bool(device_features->fullDrawIndexUint32)
    ,string_bool(device_features->imageCubeArray)
    ,string_bool(device_features->independentBlend)
    ,string_bool(device_features->geometryShader)
    ,string_bool(device_features->tessellationShader)
    ,string_bool(device_features->sampleRateShading)
    ,string_bool(device_features->dualSrcBlend)
    ,string_bool(device_features->logicOp)
    ,string_bool(device_features->multiDrawIndirect)
    ,string_bool(device_features->drawIndirectFirstInstance)
    ,string_bool(device_features->depthClamp)
    ,string_bool(device_features->depthBiasClamp)
    ,string_bool(device_features->fillModeNonSolid)
    ,string_bool(device_features->depthBounds)
    ,string_bool(device_features->wideLines)
    ,string_bool(device_features->largePoints)
    ,string_bool(device_features->alphaToOne)
    ,string_bool(device_features->multiViewport)
    ,string_bool(device_features->samplerAnisotropy)
    ,string_bool(device_features->textureCompressionETC2)
    ,string_bool(device_features->textureCompressionASTC_LDR)
    ,string_bool(device_features->textureCompressionBC)
    ,string_bool(device_features->occlusionQueryPrecise)
    ,string_bool(device_features->pipelineStatisticsQuery)
    ,string_bool(device_features->vertexPipelineStoresAndAtomics)
    ,string_bool(device_features->fragmentStoresAndAtomics)
    ,string_bool(device_features->shaderTessellationAndGeometryPointSize)
    ,string_bool(device_features->shaderImageGatherExtended)
    ,string_bool(device_features->shaderStorageImageExtendedFormats)
    ,string_bool(device_features->shaderStorageImageMultisample)
    ,string_bool(device_features->shaderStorageImageReadWithoutFormat)
    ,string_bool(device_features->shaderStorageImageWriteWithoutFormat)
    ,string_bool(device_features->shaderUniformBufferArrayDynamicIndexing)
    ,string_bool(device_features->shaderSampledImageArrayDynamicIndexing)
    ,string_bool(device_features->shaderStorageBufferArrayDynamicIndexing)
    ,string_bool(device_features->shaderStorageImageArrayDynamicIndexing)
    ,string_bool(device_features->shaderClipDistance)
    ,string_bool(device_features->shaderCullDistance)
    ,string_bool(device_features->shaderFloat64)
    ,string_bool(device_features->shaderInt64)
    ,string_bool(device_features->shaderInt16)
    ,string_bool(device_features->shaderResourceResidency)
    ,string_bool(device_features->shaderResourceMinLod)
    ,string_bool(device_features->sparseBinding)
    ,string_bool(device_features->sparseResidencyBuffer)
    ,string_bool(device_features->sparseResidencyImage2D)
    ,string_bool(device_features->sparseResidencyImage3D)
    ,string_bool(device_features->sparseResidency2Samples)
    ,string_bool(device_features->sparseResidency4Samples)
    ,string_bool(device_features->sparseResidency8Samples)
    ,string_bool(device_features->sparseResidency16Samples)
    ,string_bool(device_features->sparseResidencyAliased)
    ,string_bool(device_features->variableMultisampleRate)
    ,string_bool(device_features->inheritedQueries));
}

const char* vk_physical_device_type_to_string(VkPhysicalDeviceType *device_type)
{
	switch(*device_type)
	{
		case VK_PHYSICAL_DEVICE_TYPE_OTHER: 
			return "VK_PHYSICAL_DEVICE_TYPE_OTHER";
		case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
			return "VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU";
		case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU: 
			return "VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU";
		case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
			return "VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU";
		case VK_PHYSICAL_DEVICE_TYPE_CPU:
			return "VK_PHYSICAL_DEVICE_TYPE_CPU";
		default: 
			return "(Unknown)";
	}
}

const char* vk_physical_device_properties_to_string(VkPhysicalDeviceProperties* properties)
{
	return string(1024,
			"API Version: %d\n"
			"Driver Version: %d\n"
			"Vendor ID: %d\n"
			"Device ID: %d\n"
			"Device Name: %s\n"
			"Device Type: %s\n"
			"Pipeline Cache UUID: %d\n"
			"Device Limits: \n"
			"%s",
			properties->apiVersion,
			properties->driverVersion,
			properties->vendorID,
			properties->deviceID,
			properties->deviceName,
			vk_physical_device_type_to_string(&(properties->deviceType)),
			properties->pipelineCacheUUID,
			vk_physical_device_limits_to_string(&(properties->limits)));
}