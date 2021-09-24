

#include <engine/renderer/renderer.h>
#include <exception/exception.h>

#include <stdlib.h>

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

VkInstance vk_create_instance(VkInstanceCreateInfo* create_info, VkAllocationCallbacks* host_memory_allocator)
{
	VkInstance instance; 
	vkCall(vkCreateInstance(create_info, host_memory_allocator, &instance));
	return instance;
}

tuple_t(uint32_t, pVkPhysicalDevice_t) vk_get_physical_devices(VkInstance instance)
{
	tuple_t(uint32_t, pVkPhysicalDevice_t) pair;
	vkCall(vkEnumeratePhysicalDevices(instance, &(pair.value1), NULL)); 
	pair.value2 = (pVkPhysicalDevice_t)malloc(sizeof(VkPhysicalDevice) * pair.value1);
	vkCall(vkEnumeratePhysicalDevices(instance, &(pair.value1), pair.value2));
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