#pragma once

#include <vulkan/vulkan.h>
#include <stdint.h>
#include <stdbool.h>
#include <tuple/header_config.h>
#include <tuple/tuple.h>
#include <exception/exception.h>

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


void vk_setup_validation_layers();
VkInstance vk_create_instance(); 
tuple_t(uint32_t, pVkPhysicalDevice_t) vk_get_physical_devices(VkInstance instance);
tuple_t(uint32_t, pVkExtensionProperties_t) vk_get_instance_extension_properties();
tuple_t(uint32_t, pVkExtensionProperties_t) vk_get_physical_device_extension_properties(VkPhysicalDevice physical_device);
tuple_t(uint32_t, pVkLayerProperties_t) vk_get_instance_layer_properties();
tuple_t(uint32_t, pVkQueueFamilyProperties_t) vk_get_queue_family_properties(VkPhysicalDevice physical_device);
tuple_t(uint32_t, pVkSurfaceFormatKHR_t) vk_get_physical_device_surface_formats(VkPhysicalDevice physical_device, VkSurfaceKHR surface);
tuple_t(uint32_t, pVkPresentModeKHR_t) vk_get_physical_device_surface_present_modes(VkPhysicalDevice physical_device, VkSurfaceKHR surface);

VkSurfaceCapabilitiesKHR vk_get_physical_device_surface_capabilities(VkPhysicalDevice physical_device, VkSurfaceKHR surface);
VkPhysicalDeviceProperties vk_get_physical_device_properties(VkPhysicalDevice physical_device);
VkPhysicalDeviceFeatures vk_get_physical_device_features(VkPhysicalDevice physical_device);
VkPhysicalDeviceMemoryProperties vk_get_physical_device_memory_properties(VkPhysicalDevice physical_device);
bool vk_check_layer_support(tuple_t(uint32_t, ppVkChar_t) layers);
bool vk_check_instance_extension_support(tuple_t(uint32_t, ppVkChar_t) extensions);
bool vk_check_physical_device_extension_support(VkPhysicalDevice device, tuple_t(uint32_t, ppVkChar_t) extensions);
VkPhysicalDevice vk_select_suitable_device(tuple_t(uint32_t, pVkPhysicalDevice_t) physical_devices);

tuple_t(uint32_t, pVkImage_t) vk_get_images(VkDevice device, VkSwapchainKHR swapchain);
tuple_t(uint32_t, pVkImageView_t) vk_get_image_views(VkDevice device, VkFormat format, uint32_t imageCount, VkImage* images);
VkShaderModule vk_get_shader_module(VkDevice device, const char* file_name);
VkPipelineShaderStageCreateInfo vk_get_pipeline_shader_stage_create_info(VkShaderModule shader_module, shader_type_t shader_type, const char* entry_point);
VkPipelineVertexInputStateCreateInfo vk_get_pipeline_vertex_input_state_create_info(uint32_t attributeCount, uint32_t stride, VkVertexInputRate vertexInputRate, VkFormat* attributeFormats, uint32_t* attributeOffsets);
VkPipelineInputAssemblyStateCreateInfo vk_get_pipeline_input_assembly_state_create_info(void); 
VkPipelineViewportStateCreateInfo vk_get_pipeline_viewport_state_create_info(uint32_t viewportWidth, uint32_t viewportHeight);
VkPipelineRasterizationStateCreateInfo vk_get_pipeline_rasterization_state_create_info(void);
VkPipelineMultisampleStateCreateInfo vk_get_pipeline_multisample_state_create_info(void);
VkPipelineColorBlendStateCreateInfo vk_get_pipeline_color_blend_state_create_info(void);
VkPipelineColorBlendAttachmentState vk_get_pipeline_color_blend_attachment_state(void);
VkPipelineDynamicStateCreateInfo vk_get_pipeline_dynamic_state_create_info(void);
VkPipeline vk_get_graphics_pipeline(VkDevice device, VkPipelineLayout pipelineLayout, VkRenderPass renderPass,
											VkPipelineShaderStageCreateInfo* shaderStages,
											VkPipelineVertexInputStateCreateInfo* vertexInputState, 
											VkPipelineInputAssemblyStateCreateInfo* inputAssemblyState, 
											VkPipelineViewportStateCreateInfo* viewportState, 
											VkPipelineRasterizationStateCreateInfo* rasterizationState, 
											VkPipelineMultisampleStateCreateInfo* multisampleState, 
											VkPipelineColorBlendStateCreateInfo* colorBlendState);
tuple_t(uint32_t, pVkFramebuffer_t) vk_get_framebuffers(VkDevice device, uint32_t count, VkRenderPass* renderPasses, VkExtent2D* extents, uint32_t* layers, tuple_t(uint32_t, pVkImageView_t)* attachments);
tuple_t(uint32_t, pVkCommandBuffer_t) vk_get_command_buffers(VkDevice device, VkCommandPool commandPool, uint32_t count);
VkCommandPool vk_get_command_pool(VkDevice device, uint32_t queueFamilyIndex);
VkAttachmentReference vk_get_attachment_reference(void);
VkSubpassDependency vk_get_subpass_dependency(void);
VkSemaphore vk_get_semaphore(VkDevice device);
VkAttachmentDescription vk_get_attachment_description(VkFormat image_format);
VkSubpassDescription vk_get_subpass_description(VkAttachmentReference attachment_reference);
VkRenderPass vk_get_render_pass(VkDevice device, VkAttachmentDescription* attachments, VkSubpassDescription* subpasses, VkSubpassDependency* subpassDependencies);
VkPipelineLayout vk_get_pipeline_layout(VkDevice device);
VkViewport vk_get_viewport(uint32_t width, uint32_t height);
VkBuffer vk_get_buffer(VkDevice device, VkDeviceSize size, VkBufferUsageFlags usageFlags, VkSharingMode sharingMode);
VkDeviceMemory vk_get_device_memory_for_buffer(VkDevice device, VkPhysicalDevice physicalDevice, VkBuffer buffer, uint64_t size, uint32_t memoryProperties);
tuple_t(uint32_t, pVkVertexInputBindingDescription_t) vk_get_vertex_input_binding_descriptions(uint32_t stride, VkVertexInputRate vertexInputRate);
tuple_t(uint32_t, pVkVertexInputAttributeDescription_t) vk_get_vertex_input_attribute_descriptions(uint32_t attributeCount, VkFormat* attributeFormats, uint32_t* attributeOffsets);


void vk_dump_instance_extensions(); 
void vk_dump_instance_layers();
void vk_dump_physical_devices(tuple_t(uint32_t, pVkPhysicalDevice_t)* physical_devices);
void vk_dump_queue_families(tuple_t(uint32_t, pVkQueueFamilyProperties_t)* queue_families);
void vk_dump_physical_device_extensions(VkPhysicalDevice* physicalDevice);

const char* vk_physical_device_memory_properties_to_string(VkPhysicalDeviceMemoryProperties *memory_properties);
const char* vk_physical_device_properties_to_string(VkPhysicalDeviceProperties* properties);
const char* vk_physical_device_type_to_string(VkPhysicalDeviceType* deviceType);
const char* vk_physical_device_features_to_string(VkPhysicalDeviceFeatures* device_features);
const char* vk_physical_device_queue_family_to_string(VkQueueFamilyProperties properties);