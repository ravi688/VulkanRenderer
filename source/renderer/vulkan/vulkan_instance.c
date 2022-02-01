
#include <renderer/internal/vulkan/vulkan_instance.h>
#include <renderer/internal/vulkan/vulkan_result.h>
#include <renderer/internal/vulkan/vulkan_physical_device.h>
#include <renderer/assert.h>
#include <memory_allocator/memory_allocator.h> 	// heap_new, heap_newv, heap_free
#include <stdio.h> 		// sprintf
#include <string.h>		// strcmp

vulkan_instance_t* vulkan_instance_new()
{
	vulkan_instance_t* instance = heap_new(vulkan_instance_t);
	memset(instance, 0, sizeof(vulkan_instance_t));
	instance->handle = VK_NULL_HANDLE;			// invalid
	instance->extension_count = U32_MAX; 		// invalid
	instance->physical_device_count = U32_MAX; 	// invalid
	return instance;
}

vulkan_instance_t* vulkan_instance_create(const char* const* extensions, u32 extension_count)
{
	vulkan_instance_t* instance = vulkan_instance_new();
	const char* supported_extensions[extension_count];
	u32 supported_extension_count = 0;
	for(u32 i = 0; i < extension_count; i++)
	{
		if(!vulkan_instance_is_extension_supported(instance, extensions[i], NULL))
			LOG_WRN("Extension \"%s\" isn't supported, ignored\n", extensions[i]);
		else
		{
			supported_extensions[supported_extension_count] = extensions[i];
			supported_extension_count++;
		}
	}
	if(supported_extension_count == 0)
		memset(supported_extensions, 0, sizeof(char*) * extension_count);

	VkApplicationInfo app_info = 
	{
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = "Default application",
		.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
		.pEngineName = "Default Engine",
		.engineVersion = VK_MAKE_VERSION(1, 0, 0),
		.apiVersion = VK_API_VERSION_1_0
	};
	
	VkInstanceCreateInfo create_info = 
	{
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &app_info,
		.enabledLayerCount = 0, 		// for now let it be 0
		.ppEnabledLayerNames = NULL, 	// for now let it be NULL
		.enabledExtensionCount = supported_extension_count,
		.ppEnabledExtensionNames = supported_extensions
	};
	
	VkResult result = vkCreateInstance(&create_info, NULL, &instance->handle);
	vulkan_result_assert_success(result);
	log_msg("Vulkan instance created successfully\n");
	return instance;
}

void vulkan_instance_destroy(vulkan_instance_t* instance)
{
	assert(instance != NULL);
	if(instance->physical_devices != NULL)
		for(u32 i = 0; i < instance->physical_device_count; i++)
			vulkan_physical_device_destroy(instance->physical_devices[i]);
	if(instance->handle != VK_NULL_HANDLE)
		vkDestroyInstance(instance->handle, NULL);
	instance->handle = VK_NULL_HANDLE;
	log_msg("Vulkan instance destroyed successfully\n");
}

void vulkan_instance_release_resources(vulkan_instance_t* instance)
{
	assert(instance != NULL);
	if(instance->extension_properties != NULL)
		heap_free(instance->extension_properties);
	if(instance->physical_devices != NULL)
	{
		for(u32 i = 0; i < instance->physical_device_count; i++)
			vulkan_physical_device_release_resources(instance->physical_devices[i]);
		heap_free(instance->physical_devices);
	}
	heap_free(instance);
}

u32 vulkan_instance_get_physical_device_count(vulkan_instance_t* instance)
{
	assert(instance != NULL);
	if(instance->physical_device_count != U32_MAX)
		return instance->physical_device_count;
	VkResult result = vkEnumeratePhysicalDevices(instance->handle, &instance->physical_device_count, NULL);
	vulkan_result_assert_success(result);
	assert_wrn(instance->physical_device_count != 0);
	return instance->physical_device_count;
}

u32 vulkan_instance_get_extension_count(vulkan_instance_t* instance, const char* layer_name)
{
	assert(instance != NULL);
	assert(layer_name == NULL); 		// for now it should be null, since we are not enabling any layers
	if(instance->extension_count != U32_MAX)
		return instance->extension_count;
	VkResult result = vkEnumerateInstanceExtensionProperties(layer_name, &instance->extension_count, NULL);
	vulkan_result_assert_success(result);
	assert_wrn(instance->extension_count != 0);
	return instance->extension_count;
}

vulkan_physical_device_t** vulkan_instance_get_physical_devices(vulkan_instance_t* instance)
{
	assert(instance != NULL);
	if(instance->physical_devices != NULL)
		return instance->physical_devices;
	u32 physical_device_count = vulkan_instance_get_physical_device_count(instance);
	if(physical_device_count == 0) return NULL;
	instance->physical_devices = heap_newv(vulkan_physical_device_t*, physical_device_count);
	VkPhysicalDevice* vk_devices = heap_newv(VkPhysicalDevice, physical_device_count);
	VkResult result = vkEnumeratePhysicalDevices(instance->handle, &physical_device_count, vk_devices);
	vulkan_result_assert_complete(result);
	vulkan_result_assert_success(result);
	for(u32 i = 0; i < physical_device_count; i++)
		instance->physical_devices[i] = vulkan_physical_device_create(vk_devices[i]);
	heap_free(vk_devices);
	return instance->physical_devices;
}

VkExtensionProperties* vulkan_instance_get_extension_properties(vulkan_instance_t* instance, const char* layer_name)
{
	assert(instance != NULL);
	assert(layer_name == NULL); 		// for now it should be null, since we are not enabling any layers
	if(instance->extension_properties != NULL)
		return instance->extension_properties;
	u32 extension_count = vulkan_instance_get_extension_count(instance, layer_name);
	if(extension_count == 0) return NULL;
	instance->extension_properties = heap_newv(VkExtensionProperties, extension_count);
	VkResult result = vkEnumerateInstanceExtensionProperties(layer_name, &extension_count, instance->extension_properties);
	vulkan_result_assert_complete(result);
	vulkan_result_assert_success(result);
	return instance->extension_properties;
}

// returning bools
bool vulkan_instance_is_extension_supported(vulkan_instance_t* instance, const char* extension, const char* layer_name)
{
	assert(instance != NULL);
	assert(layer_name == NULL);
	u32 extension_count = vulkan_instance_get_extension_count(instance, layer_name);
	VkExtensionProperties* properties = vulkan_instance_get_extension_properties(instance, layer_name);
	for(u32 i = 0; i < extension_count; i++)
		if(strcmp(properties[i].extensionName, extension) == 0)
			return true;
	return false;
}

// to string (s)
void vulkan_instance_to_string(vulkan_instance_t* instance, BUFFER* string_buffer)
{
	assert(instance != NULL);
	char buffer[512];
	buf_printf(string_buffer, buffer, "Vulkan Instance | physical device count: %u, extension property count: %u\n",
		vulkan_instance_get_physical_device_count(instance),
		vulkan_instance_get_extension_count(instance, NULL));
}
