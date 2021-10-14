

#include <engine/renderer/renderer.h>
#include <exception/exception.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string/string.h>		//custom string library
#include <string.h>				//standard string library

#include <garbage_collector/garbage_collector.h>
#include <vulkan/vulkan_wrapper.h>

#include <exception/exception.h>
#include <debug.h>

#include <array/header_config.h>
#include <array/array.h>

typedef float __point_t[2];
typedef float __color_t[3];
typedef struct 
{
	__point_t position; 
	__color_t color;
} vertex_t;

typedef vertex_t* pvertex_t;
instantiate_tuple_t(uint32_t, pvertex_t);

typedef struct renderer_t
{
	VkInstance vk_instance;
	VkPhysicalDevice vk_physical_device;
	VkDevice vk_device;
	VkQueue vk_graphics_queue; 
	VkSurfaceKHR vk_surface;
	VkSwapchainKHR vk_swapchain;
	uint32_t vk_graphics_queue_index;
	tuple_t(uint32_t, pVkImage_t) vk_images;
	tuple_t(uint32_t, pVkImageView_t) vk_image_views;
	tuple_t(uint32_t, pVkFramebuffer_t) vk_framebuffers;
	tuple_t(uint32_t, pVkCommandBuffer_t) vk_command_buffers;
	VkFormat vk_format; 
	VkExtent2D vk_extent;
	VkPipelineLayout vk_pipeline_layout;
	VkRenderPass vk_render_pass;
	VkPipeline vk_pipeline;
	VkCommandPool vk_command_pool;
	VkSemaphore vk_image_available_semaphore;
	VkSemaphore vk_render_finished_semaphore;

	VkBuffer vk_vertex_buffer;
	VkDeviceMemory vk_vertex_memory;

	tuple_t(uint32_t, pvertex_t) vertices;
} renderer_t;


instantiate_array_struct(VkFormat); 
instantiate_declaration_array(VkFormat);
instantiate_implementation_array(VkFormat);

void* renderer_get_vulkan_instance(renderer_t* renderer) { return (void*)(&(renderer->vk_instance)); }
void* renderer_get_vulkan_device(renderer_t* renderer) { return (void*)(&(renderer->vk_device)); }
void* renderer_get_vulkan_surface(renderer_t* renderer) { return (void*)(&(renderer->vk_surface)); }
void* renderer_get_vulkan_swapchain(renderer_t* renderer) { return (void*)(&(renderer->vk_swapchain)); }

static VkSwapchainKHR vk_get_swapchain(renderer_t* renderer);
static void record_commands(renderer_t* renderer);

//TODO: Wrapp this physical device selection & creation of logical device into a single function
renderer_t* renderer_init()
{
	renderer_t* renderer = GC_NEW(renderer_t);
	renderer->vk_instance = vk_create_instance();

#ifdef ENABLE_VALIDATION_LAYERS
	vk_setup_validation_layers();
#endif

	//TODO: this should be something like this: 
	//		VkPhysicalDevice vulkan_device = vk_get_suitable_vulkan_device();
	tuple_t(uint32_t, pVkPhysicalDevice_t) physical_devices = vk_get_physical_devices(renderer->vk_instance); 
	renderer->vk_physical_device = vk_select_suitable_device(physical_devices);

	int32_t graphics_queue_index = -1;
	tuple_t(uint32_t, pVkQueueFamilyProperties_t) queue_families = vk_get_queue_family_properties(renderer->vk_physical_device); 
	for(uint32_t i = 0; i < queue_families.value1; i++)
	{
		VkQueueFamilyProperties properties = queue_families.value2[i];
		if(properties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			graphics_queue_index = i;
			renderer->vk_graphics_queue_index = (uint32_t)i;
			break;
		}
	}
EXCEPTION_BLOCK
(
	if(graphics_queue_index == -1)
		throw_exception(VULKAN_GRAPHICS_QUEUE_NOT_FOUND);
)

	VkDeviceQueueCreateInfo queueCreateInfo = { }; 
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = graphics_queue_index;
	queueCreateInfo.queueCount = 1;
	float queuePriorities = 1.0f;
	queueCreateInfo.pQueuePriorities = &queuePriorities;

	tuple_t(uint32_t, ppVkChar_t) requiredPhysicalDevicExtensions = (tuple_t(uint32_t, ppVkChar_t)) { 1, string_array(1, "VK_KHR_swapchain") };
EXCEPTION_BLOCK
(
	if(!vk_check_physical_device_extension_support(renderer->vk_physical_device, requiredPhysicalDevicExtensions))
		throw_exception(VULKAN_PHYSICAL_DEVICE_EXTENSION_NOT_SUPPORTED);
)

	VkPhysicalDeviceFeatures features =  { }; 
	VkDeviceCreateInfo logicalDeviceCreateInfo =  { }; 
	logicalDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO; 
	logicalDeviceCreateInfo.pQueueCreateInfos = &queueCreateInfo; 
	logicalDeviceCreateInfo.queueCreateInfoCount = 1;
	logicalDeviceCreateInfo.pEnabledFeatures = &features;
	logicalDeviceCreateInfo.enabledExtensionCount = 1; 
	logicalDeviceCreateInfo.ppEnabledExtensionNames = requiredPhysicalDevicExtensions.value2;
	logicalDeviceCreateInfo.enabledLayerCount = 0;

	VkDevice logicalDevice;
	vkCall(vkCreateDevice(renderer->vk_physical_device, &logicalDeviceCreateInfo, NULL, &logicalDevice));

	renderer->vk_device = logicalDevice;

	VkQueue graphicsQueue;
	vkGetDeviceQueue(renderer->vk_device, graphics_queue_index, 0, &graphicsQueue);
	renderer->vk_graphics_queue = graphicsQueue;

#ifdef DEBUG
	vk_dump_instance_extensions(); 
	vk_dump_physical_device_extensions(&(renderer->vk_physical_device));
	vk_dump_instance_layers();
	vk_dump_physical_devices(&physical_devices);
	vk_dump_queue_families(&queue_families);
#endif
	return renderer;
}

void renderer_init_surface(renderer_t* renderer, void* surface)
{
	renderer->vk_surface = *((VkSurfaceKHR*)surface);

EXCEPTION_BLOCK
(
	VkBool32 isSupported;
	vkCall(vkGetPhysicalDeviceSurfaceSupportKHR(renderer->vk_physical_device, renderer->vk_graphics_queue_index, renderer->vk_surface, &isSupported)); 
	if(!isSupported)
		throw_exception(VULKAN_SURFACE_NOT_SUPPORTED);
)
	renderer->vk_swapchain = vk_get_swapchain(renderer);
	renderer->vk_images = vk_get_images(renderer->vk_device, renderer->vk_swapchain); 
	renderer->vk_image_views = vk_get_image_views(renderer->vk_device, renderer->vk_format, renderer->vk_images.value1, renderer->vk_images.value2);
 
	VkPipelineShaderStageCreateInfo shaderStages[] =
	{ 
		vk_get_pipeline_shader_stage_create_info(vk_get_shader_module(renderer->vk_device, "shaders/vertexShader.spv"), VERTEX_SHADER, "main"), 
		vk_get_pipeline_shader_stage_create_info(vk_get_shader_module(renderer->vk_device, "shaders/fragmentShader.spv"), FRAGMENT_SHADER, "main")
	};

	VkPipelineVertexInputStateCreateInfo vertexInputInfo = vk_get_pipeline_vertex_input_state_create_info(
															2, 
															sizeof(vertex_t), 
															VK_VERTEX_INPUT_RATE_VERTEX, 
															array(VkFormat)(2, VK_FORMAT_R32G32_SFLOAT, VK_FORMAT_R32G32B32_SFLOAT).data, 
															array(uint32_t)(2, offsetof(vertex_t, position), offsetof(vertex_t, color)).data);

	//Fixed functions configuration
	VkPipelineInputAssemblyStateCreateInfo inputAssembly = vk_get_pipeline_input_assembly_state_create_info();
	VkPipelineViewportStateCreateInfo viewportState = vk_get_pipeline_viewport_state_create_info(renderer->vk_extent.width, renderer->vk_extent.height);
	VkPipelineRasterizationStateCreateInfo rasterizer = vk_get_pipeline_rasterization_state_create_info();
	VkPipelineMultisampleStateCreateInfo multisampling = vk_get_pipeline_multisample_state_create_info();
	VkPipelineColorBlendStateCreateInfo colorBlending = vk_get_pipeline_color_blend_state_create_info();
	VkPipelineDynamicStateCreateInfo dynamicState = vk_get_pipeline_dynamic_state_create_info();

	renderer->vk_pipeline_layout = vk_get_pipeline_layout(renderer->vk_device);

	VkAttachmentDescription colorAttachment = vk_get_attachment_description(renderer->vk_format);
	VkAttachmentReference colorAttachmentRef = vk_get_attachment_reference();
	VkSubpassDescription subpass = vk_get_subpass_description(colorAttachmentRef);
	VkSubpassDependency dependency = vk_get_subpass_dependency();

	//TODO: why calling vk_get_render_pass isn't working? fix that
	// renderer->vk_render_pass = vk_get_render_pass(renderer, &colorAttachment, &subpass, &dependency);
 	VkRenderPassCreateInfo renderPassCreateInfo = {};
	renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.attachmentCount = 1;
	renderPassCreateInfo.pAttachments = &colorAttachment;
	renderPassCreateInfo.subpassCount = 1;
	renderPassCreateInfo.pSubpasses = &subpass;
	renderPassCreateInfo.dependencyCount = 1;
	renderPassCreateInfo.pDependencies = &dependency;
	vkCall(vkCreateRenderPass(renderer->vk_device, &renderPassCreateInfo, NULL, &(renderer->vk_render_pass)));

	renderer->vk_pipeline = vk_get_graphics_pipeline(renderer->vk_device, renderer->vk_pipeline_layout, renderer->vk_render_pass,
													shaderStages,
													&vertexInputInfo, 
													&inputAssembly, 
													&viewportState, 
													&rasterizer, 
													&multisampling,
													&colorBlending);

	uint32_t framebufferCount = renderer->vk_image_views.value1;
	VkRenderPass* renderPasses = GC_NEWV(VkRenderPass, framebufferCount);
	VkExtent2D* extents = GC_NEWV(VkExtent2D, framebufferCount);
	tuple_t(uint32_t, pVkImageView_t)* attachments = GC_NEWV(tuple_t(uint32_t, pVkImageView_t), framebufferCount);
	uint32_t* layers = GC_NEWV(uint32_t, framebufferCount);
	for(uint32_t i = 0; i < framebufferCount; i++)
	{
		renderPasses[i] = renderer->vk_render_pass;
		extents[i] = renderer->vk_extent;
		attachments[i] = (tuple_t(uint32_t, pVkImageView_t)) { 1, &(renderer->vk_image_views.value2[i]) };
		layers[i] = 1;
	}
    renderer->vk_framebuffers = vk_get_framebuffers(renderer->vk_device, framebufferCount, renderPasses, extents, layers, attachments);

    vertex_t vertices[3] =
    {
    {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
    };

    // vertex_t vertices[] = 
    // {
    // 	{ { 0.5f, -0.5f},  { 0.0f, 0.0f, 0.0f } },
    // 	{ { 0.0f,  0.5f},  { 0.0f, 0.0f, 0.0f } },
    // 	{ {-0.5f, -0.5f},  { 0.0f, 0.0f, 0.0f } }
    // };
    
    renderer->vertices = (tuple_t(uint32_t, pvertex_t)) { sizeof(vertices) / sizeof(vertex_t), (vertex_t*)vertices };


    renderer->vk_vertex_buffer = vk_get_buffer(renderer->vk_device, sizeof(vertices), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE);
    renderer->vk_vertex_memory  = vk_get_device_memory_for_buffer(renderer->vk_device, renderer->vk_physical_device, renderer->vk_vertex_buffer, sizeof(vertices), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	void* data;
	vkMapMemory(renderer->vk_device, renderer->vk_vertex_memory, 0, sizeof(vertices), 0, &data);
    memcpy(data, vertices, (size_t) sizeof(vertices));
	vkUnmapMemory(renderer->vk_device, renderer->vk_vertex_memory);

	renderer->vk_command_pool = vk_get_command_pool(renderer->vk_device, renderer->vk_graphics_queue_index);
	renderer->vk_command_buffers = vk_get_command_buffers(renderer->vk_device, renderer->vk_command_pool, renderer->vk_images.value1);
	renderer->vk_image_available_semaphore = vk_get_semaphore(renderer->vk_device);
    renderer->vk_render_finished_semaphore = vk_get_semaphore(renderer->vk_device);

	record_commands(renderer);
}

void renderer_update(renderer_t* renderer)
{
	uint32_t imageIndex;
    vkAcquireNextImageKHR(renderer->vk_device, renderer->vk_swapchain, UINT64_MAX, renderer->vk_image_available_semaphore, VK_NULL_HANDLE, &imageIndex);


    VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { renderer->vk_image_available_semaphore };
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &(renderer->vk_command_buffers.value2[imageIndex]);

	VkSemaphore signalSemaphores[] = { renderer->vk_render_finished_semaphore };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	vkCall(vkQueueSubmit(renderer->vk_graphics_queue, 1, &submitInfo, VK_NULL_HANDLE));

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { renderer->vk_swapchain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = NULL; // Optional

	vkQueuePresentKHR(renderer->vk_graphics_queue, &presentInfo);
}

void renderer_terminate(renderer_t* renderer)
{
	vkFreeMemory(renderer->vk_device, renderer->vk_vertex_memory, NULL);
	vkDestroyBuffer(renderer->vk_device, renderer->vk_vertex_buffer, NULL);
    vkDestroySemaphore(renderer->vk_device, renderer->vk_image_available_semaphore, NULL);
	vkDestroySemaphore(renderer->vk_device, renderer->vk_render_finished_semaphore, NULL);
	vkDestroyCommandPool(renderer->vk_device, renderer->vk_command_pool, NULL);
	for(uint32_t i = 0; i < renderer->vk_framebuffers.value1; i++)
		vkDestroyFramebuffer(renderer->vk_device, renderer->vk_framebuffers.value2[i], NULL);
	vkDestroyPipeline(renderer->vk_device, renderer->vk_pipeline, NULL);
	vkDestroyRenderPass(renderer->vk_device, renderer->vk_render_pass, NULL);
	vkDestroyPipelineLayout(renderer->vk_device, renderer->vk_pipeline_layout, NULL);
	for(uint32_t i = 0; i < renderer->vk_image_views.value1; i++)
		vkDestroyImageView(renderer->vk_device, renderer->vk_image_views.value2[i], NULL);
	vkDestroySwapchainKHR(renderer->vk_device, renderer->vk_swapchain, NULL);
	vkDestroySurfaceKHR(renderer->vk_instance, renderer->vk_surface, NULL);
	vkDestroyDevice(renderer->vk_device, NULL);
	vkDestroyInstance(renderer->vk_instance, NULL);
}

static void record_commands(renderer_t* renderer)
{
	for (uint32_t i = 0; i < renderer->vk_command_buffers.value1; i++)
	{
    	VkCommandBufferBeginInfo beginInfo = {};
    	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    	beginInfo.flags = 0; // Optional
    	beginInfo.pInheritanceInfo = NULL; // Optional
    	vkCall(vkBeginCommandBuffer(renderer->vk_command_buffers.value2[i], &beginInfo));

    	VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass =  renderer->vk_render_pass;
		renderPassInfo.framebuffer = renderer->vk_framebuffers.value2[i];
		renderPassInfo.renderArea.offset = (VkOffset2D) {0, 0};
		renderPassInfo.renderArea.extent = renderer->vk_extent;

		VkClearValue clearColor = {{{0.1f, 0.05f, 0.1f, 1.0f}}};
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(renderer->vk_command_buffers.value2[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(renderer->vk_command_buffers.value2[i], VK_PIPELINE_BIND_POINT_GRAPHICS, renderer->vk_pipeline);
		VkBuffer vertexBuffers[] = { renderer->vk_vertex_buffer };
		VkDeviceSize offsets[] = {0};
		vkCmdBindVertexBuffers(renderer->vk_command_buffers.value2[i], 0, 1, vertexBuffers, offsets);
		vkCmdDraw(renderer->vk_command_buffers.value2[i], renderer->vertices.value1, 1, 0, 0);
		vkCmdEndRenderPass(renderer->vk_command_buffers.value2[i]);
		vkCall(vkEndCommandBuffer(renderer->vk_command_buffers.value2[i]));
    }
}


static VkSwapchainKHR vk_get_swapchain(renderer_t* renderer)
{
	VkSurfaceCapabilitiesKHR surfaceCapabilities = vk_get_physical_device_surface_capabilities(renderer->vk_physical_device, renderer->vk_surface);
 	tuple_t(uint32_t, pVkSurfaceFormatKHR_t) formats = vk_get_physical_device_surface_formats(renderer->vk_physical_device, renderer->vk_surface);
 	tuple_t(uint32_t, pVkPresentModeKHR_t) presentModes = vk_get_physical_device_surface_present_modes(renderer->vk_physical_device, renderer->vk_surface);

EXCEPTION_BLOCK
(
	if(formats.value1 == 0)
		throw_exception(VULKAN_ABORTED);
	if(presentModes.value1 == 0)
		throw_exception(VULKAN_ABORTED);
)

	VkSurfaceFormatKHR format; bool isFormatFound = false;
	for(uint32_t i = 0; i < formats.value1; i++)
		if((formats.value2[i].format == VK_FORMAT_B8G8R8A8_SRGB) && (formats.value2[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR))
		{	format = formats.value2[i]; isFormatFound = true; break; }
	if(!isFormatFound)
		format = formats.value2[0];


	VkPresentModeKHR presentMode; bool isModeFound = false; 
	for(uint32_t i = 0; i < presentModes.value1; i++)
		if(presentModes.value2[i] == VK_PRESENT_MODE_MAILBOX_KHR)
		{	presentMode = presentModes.value2[i]; isModeFound = true; break; }
	if(!isModeFound)
		presentMode = VK_PRESENT_MODE_FIFO_KHR;

	uint32_t imageCount; 
	imageCount = surfaceCapabilities.minImageCount + 1; 
	if(imageCount > surfaceCapabilities.maxImageCount) imageCount = surfaceCapabilities.maxImageCount;

	VkExtent2D extent = surfaceCapabilities.currentExtent; 
	log_msg("Extent = (%u, %u)", extent.width, extent.height);
	log_msg("ImageCount: %u", imageCount);

	renderer->vk_extent = extent; 
	renderer->vk_format = format.format;

	VkSwapchainCreateInfoKHR createInfo = { }; 
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.minImageCount = imageCount; 
	createInfo.imageFormat = format.format;
	createInfo.imageColorSpace = format.colorSpace;
	createInfo.imageExtent = extent; 
	createInfo.imageArrayLayers = 1; 
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; 
	createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.queueFamilyIndexCount = 0; 
	createInfo.pQueueFamilyIndices = NULL; 
	createInfo.preTransform = surfaceCapabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;
	createInfo.surface = renderer->vk_surface;

	VkSwapchainKHR swapchain; 
	vkCall(vkCreateSwapchainKHR(renderer->vk_device, &createInfo, NULL, &swapchain)); 
	return swapchain;
}