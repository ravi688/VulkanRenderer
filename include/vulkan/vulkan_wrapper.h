#pragma once

#include <vulkan/vulkan.h>
#include <stdint.h>
#include <stdbool.h>
#include <tuple/header_config.h>
#include <tuple/tuple.h>
#include <exception/exception.h>
#include <calltrace/calltrace.h>

#ifdef DEBUG
#	define vkCall(call)\
{\
	VkResult result =  call;\
	EXCEPTION_BLOCK\
	(\
		if(result != VK_SUCCESS)\
			throw_exception(VULKAN_ABORTED);\
	)\
}

#else
#	define vkCall(call) call
#endif


declare_exception(VULKAN_EXTENSION_NOT_SUPPORTED);
declare_exception(VULKAN_LAYER_NOT_SUPPORTED);
declare_exception(VULKAN_ABORTED);
declare_exception(VULKAN_DEVICE_NOT_FOUND);
declare_exception(VULKAN_GRAPHICS_QUEUE_NOT_FOUND);
declare_exception(VULKAN_SURFACE_NOT_SUPPORTED);
declare_exception(VULKAN_PHYSICAL_DEVICE_EXTENSION_NOT_SUPPORTED);


typedef VkPhysicalDevice* pVkPhysicalDevice_t; 
instantiate_tuple_t(uint32_t, pVkPhysicalDevice_t);

typedef VkExtensionProperties* pVkExtensionProperties_t; 
instantiate_tuple_t(uint32_t, pVkExtensionProperties_t);

typedef VkLayerProperties* pVkLayerProperties_t;
instantiate_tuple_t(uint32_t, pVkLayerProperties_t);

typedef const char* const* ppVkChar_t;
instantiate_tuple_t(uint32_t, ppVkChar_t);

typedef VkQueueFamilyProperties* pVkQueueFamilyProperties_t; 
instantiate_tuple_t(uint32_t, pVkQueueFamilyProperties_t);

typedef VkSurfaceFormatKHR* pVkSurfaceFormatKHR_t;
instantiate_tuple_t(uint32_t, pVkSurfaceFormatKHR_t);

typedef VkPresentModeKHR* pVkPresentModeKHR_t; 
instantiate_tuple_t(uint32_t, pVkPresentModeKHR_t);

typedef enum
{
	VERTEX_SHADER,
	FRAGMENT_SHADER
} shader_type_t;

typedef VkCommandBuffer* pVkCommandBuffer_t;
instantiate_tuple_t(uint32_t, pVkCommandBuffer_t);

typedef VkFramebuffer* pVkFramebuffer_t;
instantiate_tuple_t(uint32_t, pVkFramebuffer_t);

typedef VkImage* pVkImage_t;
instantiate_tuple_t(uint32_t, pVkImage_t);

typedef VkImageView* pVkImageView_t;
instantiate_tuple_t(uint32_t, pVkImageView_t);

typedef VkVertexInputAttributeDescription* pVkVertexInputAttributeDescription_t; 
instantiate_tuple_t(uint32_t, pVkVertexInputAttributeDescription_t);

typedef VkVertexInputBindingDescription* pVkVertexInputBindingDescription_t;
instantiate_tuple_t(uint32_t, pVkVertexInputBindingDescription_t);


#define vk_create_instance() __vk_create_instance(2, string_array(2, "VK_KHR_surface", "VK_KHR_win32_surface"))
function_signature(VkInstance, __vk_create_instance, uint32_t enabledExtensionCount, const char* const* enabledExtensionNames);

function_signature_void(void, vk_setup_validation_layers);
function_signature(tuple_t(uint32_t, pVkPhysicalDevice_t), vk_get_physical_devices, VkInstance instance);
function_signature_void(tuple_t(uint32_t, pVkExtensionProperties_t), vk_get_instance_extension_properties);
function_signature(tuple_t(uint32_t, pVkExtensionProperties_t), vk_get_physical_device_extension_properties, VkPhysicalDevice physical_device);
function_signature_void(tuple_t(uint32_t, pVkLayerProperties_t), vk_get_instance_layer_properties);
function_signature(tuple_t(uint32_t, pVkQueueFamilyProperties_t), vk_get_queue_family_properties, VkPhysicalDevice physical_device);
function_signature(tuple_t(uint32_t, pVkSurfaceFormatKHR_t), vk_get_physical_device_surface_formats, VkPhysicalDevice physical_device, VkSurfaceKHR surface);
function_signature(tuple_t(uint32_t, pVkPresentModeKHR_t), vk_get_physical_device_surface_present_modes, VkPhysicalDevice physical_device, VkSurfaceKHR surface);
function_signature(tuple_t(uint32_t, pVkImage_t), vk_get_images, VkDevice device, VkSwapchainKHR swapchain);
function_signature(tuple_t(uint32_t, pVkImageView_t), vk_get_image_views, VkDevice device, VkFormat format, uint32_t imageCount, VkImage* images);
function_signature(tuple_t(uint32_t, pVkFramebuffer_t), vk_get_framebuffers, VkDevice device, uint32_t count, VkRenderPass renderPasse, VkExtent2D extent, uint32_t layer, VkImageView* attachments);
function_signature(tuple_t(uint32_t, pVkCommandBuffer_t), vk_get_command_buffers, VkDevice device, VkCommandPool commandPool, uint32_t count);
function_signature(tuple_t(uint32_t, pVkVertexInputBindingDescription_t), vk_get_vertex_input_binding_descriptions, uint32_t stride, VkVertexInputRate vertexInputRate);
function_signature(tuple_t(uint32_t, pVkVertexInputAttributeDescription_t), vk_get_vertex_input_attribute_descriptions, uint32_t attributeCount, VkFormat* attributeFormats, uint32_t* attributeOffsets);

function_signature(bool, vk_check_layer_support, tuple_t(uint32_t, ppVkChar_t) layers);
function_signature(bool, vk_check_instance_extension_support, tuple_t(uint32_t, ppVkChar_t) extensions);
function_signature(bool, vk_check_physical_device_extension_support, VkPhysicalDevice device, tuple_t(uint32_t, ppVkChar_t) extensions);

function_signature(VkSurfaceCapabilitiesKHR, vk_get_physical_device_surface_capabilities, VkPhysicalDevice physical_device, VkSurfaceKHR surface);
function_signature(VkPhysicalDeviceProperties, vk_get_physical_device_properties, VkPhysicalDevice physical_device);
function_signature(VkPhysicalDeviceFeatures, vk_get_physical_device_features, VkPhysicalDevice physical_device);
function_signature(VkPhysicalDeviceMemoryProperties, vk_get_physical_device_memory_properties, VkPhysicalDevice physical_device);
function_signature(VkPhysicalDevice, vk_get_suitable_physical_device, tuple_t(uint32_t, pVkPhysicalDevice_t) physical_devices);
function_signature(VkDevice, vk_get_device, VkPhysicalDevice physicalDevice);

function_signature(uint32_t, vk_get_graphics_queue_family_index, VkPhysicalDevice physicalDevice);
function_signature(VkQueue, vk_get_device_queue, VkDevice device, uint32_t queueFamilyIndex, uint32_t queueIndex);
function_signature(VkSwapchainKHR, vk_get_swapchain, VkDevice device, VkSwapchainCreateInfoKHR* createInfo);
function_signature(VkShaderModule, vk_get_shader_module, VkDevice device, const char* file_name);
function_signature(VkPipelineShaderStageCreateInfo, vk_get_pipeline_shader_stage_create_info, VkShaderModule shader_module, shader_type_t shader_type, const char* entry_point);
function_signature(VkPipelineVertexInputStateCreateInfo, vk_get_pipeline_vertex_input_state_create_info, uint32_t attributeCount, uint32_t stride, VkVertexInputRate vertexInputRate, VkFormat* attributeFormats, uint32_t* attributeOffsets);
function_signature_void(VkPipelineInputAssemblyStateCreateInfo, vk_get_pipeline_input_assembly_state_create_info); 
function_signature(VkPipelineViewportStateCreateInfo, vk_get_pipeline_viewport_state_create_info, uint32_t viewportWidth, uint32_t viewportHeight);
function_signature_void(VkPipelineRasterizationStateCreateInfo, vk_get_pipeline_rasterization_state_create_info);
function_signature_void(VkPipelineMultisampleStateCreateInfo, vk_get_pipeline_multisample_state_create_info);
function_signature_void(VkPipelineColorBlendStateCreateInfo, vk_get_pipeline_color_blend_state_create_info);
function_signature_void(VkPipelineColorBlendAttachmentState, vk_get_pipeline_color_blend_attachment_state);
function_signature_void(VkPipelineDynamicStateCreateInfo, vk_get_pipeline_dynamic_state_create_info);
function_signature(VkPipeline, vk_get_graphics_pipeline, VkDevice device, VkPipelineLayout pipelineLayout, VkRenderPass renderPass,
											VkPipelineShaderStageCreateInfo* shaderStages,
											VkPipelineVertexInputStateCreateInfo* vertexInputState, 
											VkPipelineInputAssemblyStateCreateInfo* inputAssemblyState, 
											VkPipelineViewportStateCreateInfo* viewportState, 
											VkPipelineRasterizationStateCreateInfo* rasterizationState, 
											VkPipelineMultisampleStateCreateInfo* multisampleState, 
											VkPipelineColorBlendStateCreateInfo* colorBlendState);

function_signature(VkCommandPool, vk_get_command_pool, VkDevice device, uint32_t queueFamilyIndex);
function_signature_void(VkAttachmentReference, vk_get_attachment_reference);
function_signature_void(VkSubpassDependency, vk_get_subpass_dependency);
function_signature(VkSemaphore, vk_get_semaphore, VkDevice device);
function_signature(VkAttachmentDescription, vk_get_attachment_description, VkFormat image_format);
function_signature(VkSubpassDescription, vk_get_subpass_description, VkAttachmentReference attachment_reference);
function_signature(VkRenderPass, vk_get_render_pass, VkDevice device, VkFormat format);
function_signature(VkPipelineLayout, vk_get_pipeline_layout, VkDevice device);
function_signature(VkViewport, vk_get_viewport, uint32_t width, uint32_t height);
function_signature(VkBuffer, vk_get_buffer, VkDevice device, VkDeviceSize size, VkBufferUsageFlags usageFlags, VkSharingMode sharingMode);
function_signature(VkDeviceMemory, vk_get_device_memory_for_buffer, VkDevice device, VkPhysicalDevice physicalDevice, VkBuffer buffer, uint64_t size, uint32_t memoryProperties);

function_signature(const char*, vk_physical_device_memory_properties_to_string, VkPhysicalDeviceMemoryProperties *memory_properties);
function_signature(const char*, vk_physical_device_properties_to_string, VkPhysicalDeviceProperties* properties);
function_signature(const char*, vk_physical_device_type_to_string, VkPhysicalDeviceType* deviceType);
function_signature(const char*, vk_physical_device_features_to_string, VkPhysicalDeviceFeatures* device_features);
function_signature(const char*, vk_physical_device_queue_family_to_string, VkQueueFamilyProperties properties);

function_signature_void(void, vk_dump_instance_extensions); 
function_signature_void(void, vk_dump_instance_layers);
function_signature(void, vk_dump_physical_devices, tuple_t(uint32_t, pVkPhysicalDevice_t)* physical_devices);
function_signature(void, vk_dump_queue_families, tuple_t(uint32_t, pVkQueueFamilyProperties_t)* queue_families);
function_signature(void, vk_dump_physical_device_extensions, VkPhysicalDevice* physicalDevice);

#define __vk_create_instance(...) define_alias_function_macro(__vk_create_instance, __VA_ARGS__)

#define vk_setup_validation_layers(...) define_alias_function_void_macro(vk_setup_validation_layers)
#define vk_get_physical_devices(...) define_alias_function_macro(vk_get_physical_devices, __VA_ARGS__)
#define vk_get_instance_extension_properties(...) define_alias_function_void_macro(vk_get_instance_extension_properties)
#define vk_get_physical_device_extension_properties(...) define_alias_function_macro(vk_get_physical_device_extension_properties, __VA_ARGS__)
#define vk_get_instance_layer_properties(...) define_alias_function_void_macro(vk_get_instance_layer_properties)
#define vk_get_queue_family_properties(...) define_alias_function_macro(vk_get_queue_family_properties, __VA_ARGS__)
#define vk_get_physical_device_surface_formats(...) define_alias_function_macro(vk_get_physical_device_surface_formats, __VA_ARGS__)
#define vk_get_physical_device_surface_present_modes(...) define_alias_function_macro(vk_get_physical_device_surface_present_modes, __VA_ARGS__)
#define vk_get_images(...) define_alias_function_macro(vk_get_images, __VA_ARGS__)
#define vk_get_image_views(...) define_alias_function_macro(vk_get_image_views, __VA_ARGS__)
#define vk_get_framebuffers(...) define_alias_function_macro(vk_get_framebuffers, __VA_ARGS__)
#define vk_get_command_buffers(...) define_alias_function_macro(vk_get_command_buffers, __VA_ARGS__)
#define vk_get_vertex_input_binding_descriptions(...) define_alias_function_macro(vk_get_vertex_input_binding_descriptions, __VA_ARGS__)
#define vk_get_vertex_input_attribute_descriptions(...) define_alias_function_macro(vk_get_vertex_input_attribute_descriptions, __VA_ARGS__)

#define vk_check_layer_support(...) define_alias_function_macro(vk_check_layer_support, __VA_ARGS__)
#define vk_check_instance_extension_support(...) define_alias_function_macro(vk_check_instance_extension_support, __VA_ARGS__)
#define vk_check_physical_device_extension_support(...) define_alias_function_macro(vk_check_physical_device_extension_support, __VA_ARGS__)

#define vk_get_physical_device_surface_capabilities(...) define_alias_function_macro(vk_get_physical_device_surface_capabilities, __VA_ARGS__)
#define vk_get_physical_device_properties(...) define_alias_function_macro(vk_get_physical_device_properties, __VA_ARGS__)
#define vk_get_physical_device_features(...) define_alias_function_macro(vk_get_physical_device_features, __VA_ARGS__)
#define vk_get_physical_device_memory_properties(...) define_alias_function_macro(vk_get_physical_device_memory_properties, __VA_ARGS__)
#define vk_get_suitable_physical_device(...) define_alias_function_macro(vk_get_suitable_physical_device, __VA_ARGS__)
#define vk_get_device(...) define_alias_function_macro(vk_get_device, __VA_ARGS__)

#define vk_get_graphics_queue_family_index(...) define_alias_function_macro(vk_get_graphics_queue_family_index, __VA_ARGS__)
#define vk_get_device_queue(...) define_alias_function_macro(vk_get_device_queue, __VA_ARGS__)
#define vk_get_swapchain(...) define_alias_function_macro(vk_get_swapchain, __VA_ARGS__)
#define vk_get_shader_module(...) define_alias_function_macro(vk_get_shader_module, __VA_ARGS__)
#define vk_get_pipeline_shader_stage_create_info(...) define_alias_function_macro(vk_get_pipeline_shader_stage_create_info, __VA_ARGS__)
#define vk_get_pipeline_vertex_input_state_create_info(...) define_alias_function_macro(vk_get_pipeline_vertex_input_state_create_info, __VA_ARGS__)
#define vk_get_pipeline_input_assembly_state_create_info(...) define_alias_function_void_macro(vk_get_pipeline_input_assembly_state_create_info)
#define vk_get_pipeline_viewport_state_create_info(...) define_alias_function_macro(vk_get_pipeline_viewport_state_create_info, __VA_ARGS__)
#define vk_get_pipeline_rasterization_state_create_info(...) define_alias_function_void_macro(vk_get_pipeline_rasterization_state_create_info)
#define vk_get_pipeline_multisample_state_create_info(...) define_alias_function_void_macro(vk_get_pipeline_multisample_state_create_info)
#define vk_get_pipeline_color_blend_state_create_info(...) define_alias_function_void_macro(vk_get_pipeline_color_blend_state_create_info)
#define vk_get_pipeline_color_blend_attachment_state(...) define_alias_function_void_macro(vk_get_pipeline_color_blend_attachment_state)
#define vk_get_pipeline_dynamic_state_create_info(...) define_alias_function_void_macro(vk_get_pipeline_dynamic_state_create_info)
#define vk_get_graphics_pipeline(...) define_alias_function_macro(vk_get_graphics_pipeline, __VA_ARGS__)

#define vk_get_command_pool(...) define_alias_function_macro(vk_get_command_pool, __VA_ARGS__)
#define vk_get_attachment_reference(...) define_alias_function_void_macro(vk_get_attachment_reference)
#define vk_get_subpass_dependency(...) define_alias_function_macro(vk_get_subpass_dependency, __VA_ARGS__)
#define vk_get_semaphore(...) define_alias_function_macro(vk_get_semaphore, __VA_ARGS__)
#define vk_get_attachment_description(...) define_alias_function_macro(vk_get_attachment_description, __VA_ARGS__)
#define vk_get_subpass_description(...) define_alias_function_macro(vk_get_subpass_description, __VA_ARGS__)
#define vk_get_render_pass(...) define_alias_function_macro(vk_get_render_pass, __VA_ARGS__)
#define vk_get_pipeline_layout(...) define_alias_function_macro(vk_get_pipeline_layout, __VA_ARGS__)
#define vk_get_viewport(...) define_alias_function_macro(vk_get_viewport, __VA_ARGS__)
#define vk_get_buffer(...) define_alias_function_macro(vk_get_buffer, __VA_ARGS__)
#define vk_get_device_memory_for_buffer(...) define_alias_function_macro(vk_get_device_memory_for_buffer, __VA_ARGS__)

#define vk_physical_device_memory_properties_to_string(...) define_alias_function_macro(vk_physical_device_memory_properties_to_string, __VA_ARGS__)
#define vk_physical_device_properties_to_string(...) define_alias_function_macro(vk_physical_device_properties_to_string, __VA_ARGS__)
#define vk_physical_device_type_to_string(...) define_alias_function_macro(vk_physical_device_type_to_string, __VA_ARGS__)
#define vk_physical_device_features_to_string(...) define_alias_function_macro(vk_physical_device_features_to_string, __VA_ARGS__)
#define vk_physical_device_queue_family_to_string(...) define_alias_function_macro(vk_physical_device_queue_family_to_string, __VA_ARGS__)

#define vk_physical_device_memory_properties_to_string(...) define_alias_function_macro(vk_physical_device_memory_properties_to_string, __VA_ARGS__)
#define vk_physical_device_properties_to_string(...) define_alias_function_macro(vk_physical_device_properties_to_string, __VA_ARGS__)
#define vk_physical_device_type_to_string(...) define_alias_function_macro(vk_physical_device_type_to_string, __VA_ARGS__)
#define vk_physical_device_features_to_string(...) define_alias_function_macro(vk_physical_device_features_to_string, __VA_ARGS__)
#define vk_physical_device_queue_family_to_string(...) define_alias_function_macro(vk_physical_device_queue_family_to_string, __VA_ARGS__)
