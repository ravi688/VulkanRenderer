#include <renderer/internal/vulkan/vulkan_defines.h>
#include <renderer/internal/vulkan/vulkan_instance.h>
#include <renderer/internal/vulkan/vulkan_result.h>
#include <renderer/internal/vulkan/vulkan_physical_device.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_allocator.h>
#include <renderer/assert.h>
#include <renderer/alloc.h> 	// heap_new, heap_newv, heap_free
#include <renderer/memory_allocator.h>
#include <stdio.h> 		// sprintf
#include <string.h>		// strcmp

RENDERER_API vulkan_instance_t* vulkan_instance_new(memory_allocator_t* allocator)
{
	vulkan_instance_t* instance = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_INSTANCE, vulkan_instance_t);
	memzero(instance, vulkan_instance_t);
	instance->handle = VK_NULL_HANDLE;			// invalid
	instance->extension_count = U32_MAX; 		// invalid
	instance->physical_device_count = U32_MAX; 	// invalid
	return instance;
}

RENDERER_API vulkan_instance_t* vulkan_instance_create(vulkan_renderer_t* renderer, const char* const* extensions, u32 extension_count)
{
	vulkan_instance_t* instance = vulkan_instance_new(renderer->allocator);
	instance->renderer = renderer;
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
		safe_memzerov((void*)supported_extensions, char*, extension_count);

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
	
	vkCall(vkCreateInstance(&create_info, VULKAN_ALLOCATION_CALLBACKS(renderer), &instance->handle));
	log_msg("Vulkan instance created successfully\n");
	return instance;
}

RENDERER_API void vulkan_instance_destroy(vulkan_instance_t* instance)
{
	if(instance->physical_devices != NULL)
		for(u32 i = 0; i < instance->physical_device_count; i++)
			vulkan_physical_device_destroy(&instance->physical_devices[i]);
	if(instance->handle != VK_NULL_HANDLE)
		vkDestroyInstance(instance->handle, VULKAN_ALLOCATION_CALLBACKS(instance->renderer));
	instance->handle = VK_NULL_HANDLE;
	log_msg("Vulkan instance destroyed successfully\n");
}

RENDERER_API void vulkan_instance_release_resources(vulkan_instance_t* instance)
{
	_debug_assert__(instance != NULL);
	if(instance->extension_properties != NULL)
		memory_allocator_dealloc(instance->renderer->allocator, instance->extension_properties);
	if(instance->physical_devices != NULL)
	{
		for(u32 i = 0; i < instance->physical_device_count; i++)
			vulkan_physical_device_release_resources(&instance->physical_devices[i]);
		memory_allocator_dealloc(instance->renderer->allocator, instance->physical_devices);
	}
	memory_allocator_dealloc(instance->renderer->allocator, instance);
}

RENDERER_API u32 vulkan_instance_get_physical_device_count(vulkan_instance_t* instance)
{
	_debug_assert__(instance != NULL);
	if(instance->physical_device_count != U32_MAX)
		return instance->physical_device_count;
	vkCall(vkEnumeratePhysicalDevices(instance->handle, &instance->physical_device_count, NULL));
	_debug_assert_wrn__(instance->physical_device_count != 0);
	return instance->physical_device_count;
}

RENDERER_API u32 vulkan_instance_get_extension_count(vulkan_instance_t* instance, const char* layer_name)
{
	_debug_assert__(layer_name == NULL); 		// for now it should be null, since we are not enabling any layers
	if(instance->extension_count != U32_MAX)
		return instance->extension_count;
	vkCall(vkEnumerateInstanceExtensionProperties(layer_name, &instance->extension_count, NULL));
	_debug_assert_wrn__(instance->extension_count != 0);
	return instance->extension_count;
}

RENDERER_API vulkan_physical_device_t* vulkan_instance_get_physical_devices(vulkan_instance_t* instance)
{
	if(instance->physical_devices != NULL)
		return instance->physical_devices;
	u32 physical_device_count = vulkan_instance_get_physical_device_count(instance);
	if(physical_device_count == 0) return NULL;
	instance->physical_devices = memory_allocator_alloc_obj_array(instance->renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_PHYSICAL_DEVICE_ARRAY, vulkan_physical_device_t, physical_device_count);
	VkPhysicalDevice* vk_devices = memory_allocator_alloc_obj_array(instance->renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_PHYSICAL_DEVICE_ARRAY, VkPhysicalDevice, physical_device_count);
	vkCall(vkEnumeratePhysicalDevices(instance->handle, &physical_device_count, vk_devices));
	for(u32 i = 0; i < physical_device_count; i++)
		vulkan_physical_device_create_no_alloc(instance->renderer, vk_devices[i], &instance->physical_devices[i]);
	memory_allocator_dealloc(instance->renderer->allocator, vk_devices);
	return instance->physical_devices;
}

RENDERER_API VkExtensionProperties* vulkan_instance_get_extension_properties(vulkan_instance_t* instance, const char* layer_name)
{
	_debug_assert__(layer_name == NULL); 		// for now it should be null, since we are not enabling any layers
	if(instance->extension_properties != NULL)
		return instance->extension_properties;
	u32 extension_count = vulkan_instance_get_extension_count(instance, layer_name);
	if(extension_count == 0) return NULL;
	instance->extension_properties = memory_allocator_alloc_obj_array(instance->renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_EXTENSION_PROPERTIES_ARRAY, VkExtensionProperties, extension_count);
	vkCall(vkEnumerateInstanceExtensionProperties(layer_name, &extension_count, instance->extension_properties));
	return instance->extension_properties;
}

// returning bools
RENDERER_API bool vulkan_instance_is_extension_supported(vulkan_instance_t* instance, const char* extension, const char* layer_name)
{
	_debug_assert__(layer_name == NULL);
	u32 extension_count = vulkan_instance_get_extension_count(instance, layer_name);
	VkExtensionProperties* properties = vulkan_instance_get_extension_properties(instance, layer_name);
	for(u32 i = 0; i < extension_count; i++)
		if(strcmp(properties[i].extensionName, extension) == 0)
			return true;
	return false;
}

// to string (s)
RENDERER_API void vulkan_instance_to_string(vulkan_instance_t* instance, BUFFER* string_buffer)
{
	char buffer[512];
	buf_printf(string_buffer, buffer, "Vulkan Instance | physical device count: %u, extension property count: %u\n",
		vulkan_instance_get_physical_device_count(instance),
		vulkan_instance_get_extension_count(instance, NULL));
}
