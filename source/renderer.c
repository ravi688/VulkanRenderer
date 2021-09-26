

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

#define vkCall(call)\
{\
	VkResult result =  call;\
	EXCEPTION_BLOCK\
	(\
		if(result != VK_SUCCESS)\
			throw_exception(VULKAN_ABORTED);\
	)\
}

#ifdef DEBUG
static const bool enable_validation_layers = true;
#else
static const bool enable_validation_layers = false;
#endif

renderer_t* renderer_init()
{
	renderer_t* renderer = GC_NEW(renderer_t);
	VkInstance instance = renderer->vk_instance = vk_create_instance();

#ifdef DEBUG
	vk_dump_instance_extensions(); 
	vk_dump_instance_layers();
#endif

	if(vk_check_layer_support((tuple_t(uint32_t, ppVkChar_t)){ 1, string_array(1, "VK_LAYER_KHRONOS_validation") }))
		puts("VK_LAYER_KHRONOS_validation is supported");
	else puts("VK_LAYER_KHRONOS_validation is not supported");

	if(vk_check_extension_support((tuple_t(uint32_t, ppVkChar_t)) { 1, string_array(1, "My Extension") } ))
		puts("My Extension is supported"); 
	else puts("My Extension is not supported");

	tuple_t(uint32_t, pVkPhysicalDevice_t) physical_devices = vk_get_physical_devices(instance); 
	for(uint32_t i = 0; i < physical_devices.value1; i++)
	{
		VkPhysicalDevice device = physical_devices.value2[i];
		VkPhysicalDeviceProperties properties = vk_get_physical_device_properties(device);
		VkPhysicalDeviceMemoryProperties memory_properties = vk_get_physical_device_memory_properties(device);
		puts("Physical Device Properties----------------------");
		puts(vk_physical_device_properties_to_string(&properties));
		puts("Physical Device Memory Properties---------------");
		puts(vk_physical_device_memory_properties_to_string(&memory_properties));
	}
	return renderer;
}

void renderer_terminate(renderer_t* renderer)
{
	vkDestroyInstance(renderer->vk_instance, NULL);
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