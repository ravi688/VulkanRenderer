
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_wrapper.h>
#include <renderer/debug.h>
#include <string/string.h>
#include <garbage_collector/garbage_collector.h>

#include <memory_allocator/memory_allocator.h>
#include <array/header_config.h>
#include <array/array.h>
#include <renderer/defines.h>
#include <renderer/assert.h>

#define BIT(flag, bit) ((flag & bit) == bit)

instantiate_array_struct(VkDeviceQueueCreateInfo);
instantiate_declaration_array(VkDeviceQueueCreateInfo);
instantiate_implementation_array(VkDeviceQueueCreateInfo);

instantiate_array_struct(VkFormat); 
instantiate_declaration_array(VkFormat);

#define WINDOW_HEIGHT 600
#define WINDOW_WIDTH 600


typedef struct { float position[2]; float color[3]; } vertex_t;

__attribute__((destructor))
void destructor()
{
	glfwTerminate();
}

__attribute__((constructor))
void constructor()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}

const char* vk_extension_properties_to_string(VkExtensionProperties* properties)
{
	return string(VK_MAX_EXTENSION_NAME_SIZE + 33, "[%s, %u]", properties->extensionName, properties->specVersion);
}

const char* vk_surface_transform_bitmask_to_string(VkFlags flag)
{
	char* buffer = GC_ALLOC(2048); 		//2 KB
	memset(buffer, 0, 2048);
	if(BIT(flag, VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR))
		strcat(buffer, "VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR");
    if(BIT(flag, VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR))
    	strcat(buffer, "VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR");
    if(BIT(flag, VK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR))
    	strcat(buffer, "VK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR");
    if(BIT(flag, VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR))
    	strcat(buffer, "VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR");
    if(BIT(flag, VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_BIT_KHR))
    	strcat(buffer, "VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_BIT_KHR");
    if(BIT(flag, VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90_BIT_KHR))
    	strcat(buffer, "VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90_BIT_KHR");
    if(BIT(flag, VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180_BIT_KHR))
    	strcat(buffer, "VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180_BIT_KHR");
    if(BIT(flag, VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270_BIT_KHR))
    	strcat(buffer, "VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270_BIT_KHR");
    if(BIT(flag, VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR))
    	strcat(buffer, "VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR");
    return (const char*)buffer;
}

const char* vk_surface_transform_bit_to_string(VkSurfaceTransformFlagBitsKHR transform)
{
	switch(transform)
	{
		case VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR: return "VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR";
    	case VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR: return "VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR";
    	case VK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR: return "VK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR";
    	case VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR: return "VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR";
    	case VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_BIT_KHR: return "VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_BIT_KHR";
    	case VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90_BIT_KHR: return "VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90_BIT_KHR";
    	case VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180_BIT_KHR: return "VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180_BIT_KHR";
    	case VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270_BIT_KHR: return "VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270_BIT_KHR";
    	case VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR: return "VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR";
    	default: return "Unknown";
	}
}

const char* vk_composite_alpha_bitmask_to_string(VkFlags flags)
{
	char* buffer = GC_ALLOC(1024); 		//1KB
	memset(buffer, 0, 1024);
	if(BIT(flags, VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR)) strcat(buffer, "[VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR]");
    if(BIT(flags, VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR)) strcat(buffer, "[VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR]");
    if(BIT(flags, VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR)) strcat(buffer, "[VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR]");
    if(BIT(flags, VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR)) strcat(buffer, "[VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR]");
    return (const char*)buffer;
}

const char* vk_image_usage_bitmask_to_string(VkFlags flags)
{
	char* buffer = GC_ALLOC(1024);		//1KB
	memset(buffer, 0, 1024);
	if(BIT(flags, VK_IMAGE_USAGE_SAMPLED_BIT)) strcat(buffer, "[VK_IMAGE_USAGE_SAMPLED_BIT]");
	if(BIT(flags, VK_IMAGE_USAGE_STORAGE_BIT)) strcat(buffer, "[VK_IMAGE_USAGE_STORAGE_BIT]");
	if(BIT(flags, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)) strcat(buffer, "[VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT]");
	if(BIT(flags, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)) strcat(buffer, "[VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT]");
	if(BIT(flags, VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT)) strcat(buffer, "[VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT]");
	if(BIT(flags, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT)) strcat(buffer, "[VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT]");
	if(BIT(flags, VK_IMAGE_USAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR)) strcat(buffer, "[VK_IMAGE_USAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR]");
	if(BIT(flags, VK_IMAGE_USAGE_FRAGMENT_DENSITY_MAP_BIT_EXT)) strcat(buffer, "[VK_IMAGE_USAGE_FRAGMENT_DENSITY_MAP_BIT_EXT]");
	return buffer;
}

const char* vk_surface_capabilities_to_string(VkSurfaceCapabilitiesKHR* capabilities)
{
	return string(4096 * 2,
		"Surface Capabilities:\n"
		"Min Image Count: %u\n"
		"Max Image Count: %u\n"
		"Current Extent: (%u, %u)\n"
		"Min Image Extent: (%u, %u)\n"
		"Max Image Extent: (%u, %u)\n"
		"Max Image Array Layers: %u\n"
		"Supported Transforms: %s\n"
		"Current Transform: %s\n"
		"Supported Composite Alpha: %s\n"
		"Supported Image Usage: %s\n",
		capabilities->minImageCount, 
		capabilities->maxImageCount, 
		capabilities->currentExtent.width, capabilities->currentExtent.height,
		capabilities->minImageExtent.width, capabilities->minImageExtent.height,
		capabilities->maxImageExtent.width, capabilities->maxImageExtent.height,
		capabilities->maxImageArrayLayers,
		vk_surface_transform_bitmask_to_string(capabilities->supportedTransforms),
		vk_surface_transform_bit_to_string(capabilities->currentTransform),
		vk_composite_alpha_bitmask_to_string(capabilities->supportedCompositeAlpha),
		vk_image_usage_bitmask_to_string(capabilities->supportedUsageFlags)
		);
}

u32 vk_get_suitable_memory(VkPhysicalDevice physicalDevice, u32 memoryTypeBits, u32 properties)
{
	VkPhysicalDeviceMemoryProperties memProperties; 
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties); 
	for(u32 i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if((memoryTypeBits & (1 << i)) && ((memProperties.memoryTypes[i].propertyFlags & properties) == properties))
			return i;
	}
	log_fetal_err("Failed to find suitable memory!");
}

int main(int argc, char** argv)
{
	u32 instanceExtensionCount;
	vkCall(vkEnumerateInstanceExtensionProperties(NULL, &instanceExtensionCount, NULL));
	VkExtensionProperties* instanceExtensions = GC_NEWV(VkExtensionProperties, instanceExtensionCount); 
	vkCall(vkEnumerateInstanceExtensionProperties(NULL, &instanceExtensionCount, instanceExtensions));

	log_msg("SUPPORTED INSTANCE EXTENSIONS");
	for(u32 i = 0; i < instanceExtensionCount; i++)
	{
		log_msg("%u. %s", i + 1, vk_extension_properties_to_string(instanceExtensions + i));
	}

	u32 glfwRequiredExtensionCount;
	const char* * glfwRequiredExtensions = glfwGetRequiredInstanceExtensions(&glfwRequiredExtensionCount);

	log_msg("GLFW REQUIRED INSTANCE EXTENSIONS");
	for(u32 i = 0; i < glfwRequiredExtensionCount; i++)
	{
		log_msg("%u. %s", i + 1, glfwRequiredExtensions[i]);
	}

	for(u32 i = 0; i < glfwRequiredExtensionCount; i++)
	{
		bool found = false;
		for(u32 j = 0; j < instanceExtensionCount; j++)
		{
			if(!strcmp(glfwRequiredExtensions[i], instanceExtensions[j].extensionName))
			{	
				found = true;
				break;
			}
		}
		if(!found)
		{
			log_fetal_err("Glfw required extension %s isn't supported", glfwRequiredExtensions[i]);
		}
	}


	VkInstanceCreateInfo instanceCreateInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.enabledLayerCount = 0,
		.ppEnabledLayerNames = NULL, 
		.enabledExtensionCount = glfwRequiredExtensionCount,
		.ppEnabledExtensionNames = glfwRequiredExtensions,
		.pApplicationInfo = NULL
	};
	VkInstance instance = vk_create_instance();
	//vkCall(vkCreateInstance(&instanceCreateInfo, NULL, &instance));

	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Vulkan Renderer Test", NULL, NULL);
	if(window == NULL)
	{
		log_fetal_err("Failed to create GLFW window");
	}

	VkSurfaceKHR surface;
	vkCall(glfwCreateWindowSurface(instance, window, NULL, &surface));


	u32 gpuCount;
	VkPhysicalDevice* gpus;
	vkCall(vkEnumeratePhysicalDevices(instance, &gpuCount, NULL));
	gpus = GC_NEWV(VkPhysicalDevice, gpuCount); 
	vkCall(vkEnumeratePhysicalDevices(instance, &gpuCount, gpus)); 

	log_msg("GPU INFORMATION");
	log_msg("GPU count: %d", gpuCount);
	for(u32 i = 0; i < gpuCount; i++)
	{
		VkPhysicalDeviceProperties gpuProperties;
		vkGetPhysicalDeviceProperties(gpus[i], &gpuProperties);
		log_msg("%u. %s", i + 1, vk_physical_device_properties_to_string(&gpuProperties));
	}

	VkPhysicalDevice gpu = gpus[0];


	u32 queueFamilyCount; 
	VkQueueFamilyProperties* queueFamilyProperties;
	vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queueFamilyCount, NULL); 
	queueFamilyProperties = GC_NEWV(VkQueueFamilyProperties, queueFamilyCount); 
	vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queueFamilyCount, queueFamilyProperties); 

	s32 graphicsQueueIndex = -1;
	s32 presentQueueIndex = -1;
	for(u32 i = 0; i < queueFamilyCount; i++)
	{
		if(graphicsQueueIndex == -1)
		{
			if(queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
				graphicsQueueIndex = i;
		}

		if(presentQueueIndex == -1)
		{
			VkBool32 supportsPresentation;
			vkCall(vkGetPhysicalDeviceSurfaceSupportKHR(gpu, i, surface, &supportsPresentation));
			if(supportsPresentation)
				presentQueueIndex = i;
		}
	}

	if(graphicsQueueIndex == -1)
	{
		log_fetal_err("Failed to find queue with VK_QUEUE_GRAPHICS_BIT bit set");
	}

	if(presentQueueIndex == -1)
	{
		log_fetal_err("Failed to find queue with presentation support");
	}

	bool sameQueue = (graphicsQueueIndex == presentQueueIndex);
	if(!sameQueue)
	{
		log_wrn("Graphics and Present Queues are different");
	}
	VkDeviceQueueCreateInfo graphicsQueueCreateInfo = 
	{
		.queueFamilyIndex = graphicsQueueIndex,
		.queueCount = 1,
		.pQueuePriorities = array(float)(1, 1.0f).data
	}; 

	VkDeviceQueueCreateInfo presentQueueCreateInfo = 
	{
		.queueFamilyIndex = presentQueueIndex,
		.queueCount = 1,
		.pQueuePriorities = array(float)(1, 1.0f).data
	};

	VkPhysicalDeviceFeatures enabledFeatures =  { };
	VkDeviceCreateInfo logicalDeviceCreateInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.enabledExtensionCount = 1,
		.ppEnabledExtensionNames = string_array(1, "VK_KHR_swapchain"),
		.pEnabledFeatures = &enabledFeatures,
		.enabledLayerCount = 0,
		.ppEnabledLayerNames = NULL, 
		.queueCreateInfoCount = sameQueue ? 1 : 2,
		.pQueueCreateInfos = sameQueue ? &graphicsQueueCreateInfo : array(VkDeviceQueueCreateInfo)(2, graphicsQueueCreateInfo, presentQueueCreateInfo).data
	};

	VkDevice device;
	vkCall(vkCreateDevice(gpu, &logicalDeviceCreateInfo, NULL, &device));


	//Queues
	VkQueue graphicsQueue = vk_get_device_queue(device, graphicsQueueIndex, 0);
	VkQueue presentQueue = vk_get_device_queue(device, presentQueueIndex, 0);


	//Swapchain
	VkSwapchainCreateInfoKHR swapchainCreateInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.surface = surface,
		.minImageCount = 3,
		.imageFormat = VK_FORMAT_B8G8R8A8_SRGB,
		.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
		.imageExtent = { WINDOW_WIDTH, WINDOW_HEIGHT },
		.imageArrayLayers = 1,
		.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = NULL,
		.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
		.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		.presentMode = VK_PRESENT_MODE_MAILBOX_KHR,
		.clipped = VK_TRUE,
		.oldSwapchain = VK_NULL_HANDLE
	};

	VkSwapchainKHR swapchain = vk_get_swapchain(device, &swapchainCreateInfo);

	//Semphores
	VkSemaphore imageAvailableSemphore = vk_get_semaphore(device);
	VkSemaphore renderingCompleteSemaphore = vk_get_semaphore(device);

	//Swapchain Images
	uint32_t imageCount;
	tuple_t(uint32_t, pVkImage_t) _images = vk_get_images(device, swapchain);
	imageCount = _images.value1;
	VkImage* images = _images.value2;

	//Render Pass
	VkRenderPass renderPass = vk_get_render_pass(device, VK_FORMAT_B8G8R8A8_SRGB);


	//Swapchain Image Views
	VkImageView* imageViews  = vk_get_image_views(device, VK_FORMAT_B8G8R8A8_SRGB, imageCount, images).value2;

	//Framebuffers
	VkFramebuffer* framebuffers = vk_get_framebuffers(device, imageCount, renderPass, (VkExtent2D) { WINDOW_WIDTH, WINDOW_HEIGHT }, 1, imageViews).value2;


	//Command Buffers
	VkCommandPool commandPool = vk_get_command_pool(device, graphicsQueueIndex);
	VkCommandBuffer* commandBuffers = vk_get_command_buffers(device, commandPool, imageCount).value2;

	//Graphics Pipeline
	VkPipelineLayout pipelineLayout = vk_get_pipeline_layout(device, 0, NULL, 0, NULL);

	VkPipelineShaderStageCreateInfo shaderStages[2] = 
	{
		vk_get_pipeline_shader_stage_create_info(vk_get_shader_module(device, "shaders/test/vertexShader.vert"), VULKAN_SHADER_TYPE_VERTEX, "main"),
		vk_get_pipeline_shader_stage_create_info(vk_get_shader_module(device, "shaders/test/fragmentShader.frag"), VULKAN_SHADER_TYPE_FRAGMENT, "main"),
	};
	VkFormat formats[2] = { VK_FORMAT_R32G32_SFLOAT, VK_FORMAT_R32G32B32_SFLOAT };
	u32 offsets[2] = { offsetof(vertex_t, position), offsetof(vertex_t, color) };
	vertex_attribute_binding_info_t* vertex_attribute_info = stack_new(vertex_attribute_binding_info_t);
	vertex_attribute_info->attribute_count = 2;
	vertex_attribute_info->attribute_formats = formats;
	vertex_attribute_info->attribute_offsets = offsets;
	uint32_t* strides = stack_new(uint32_t);
	*strides = sizeof(vertex_t);
	VkPipelineVertexInputStateCreateInfo vertexInputInfo = vk_get_pipeline_vertex_input_state_create_info(1, strides, vertex_attribute_info);
	VkPipelineInputAssemblyStateCreateInfo inputAssembly = vk_get_pipeline_input_assembly_state_create_info();
	VkPipelineViewportStateCreateInfo viewportState = vk_get_pipeline_viewport_state_create_info(WINDOW_WIDTH, WINDOW_HEIGHT); 
	VkPipelineRasterizationStateCreateInfo rasterizer = vk_get_pipeline_rasterization_state_create_info();
	VkPipelineMultisampleStateCreateInfo multisampling = vk_get_pipeline_multisample_state_create_info();
	VkPipelineColorBlendStateCreateInfo colorBlending = vk_get_pipeline_color_blend_state_create_info();

	VkPipeline graphicsPipeline = vk_get_graphics_pipeline(device, pipelineLayout, renderPass,
													shaderStages,
													&vertexInputInfo, 
													&inputAssembly, 
													&viewportState, 
													&rasterizer, 
													&multisampling,
													&colorBlending);
	stack_free(strides);
	stack_free(vertex_attribute_info);
	log_msg("Graphics Pipeline Created!");


	//Vertex Buffer
	vertex_t vertices[3] = 
	{
		{ { 0.0f, 0.5f 	}, { 	0.0f, 0.0f, 1.0f 	} },
		{ { -0.5f, 0.0f }, {	0.0f, 1.0f, 0.0f 	} },
		{ { 0.5f, 0.0f 	}, { 	1.0f, 0.0f, 0.0f 	} }
	}; 

	VkBuffer vertexBuffer = vk_get_buffer(device, sizeof(vertices), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE);
	VkDeviceMemory deviceMemory = vk_get_device_memory_for_buffer(device, gpu, vertexBuffer, sizeof(vertices), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	void* data; 
	vkMapMemory(device, deviceMemory, 0, sizeof(vertices), 0, &data); 
	memcpy(data, vertices, sizeof(vertices));
	vkUnmapMemory(device, deviceMemory);


	//Command recording into command buffers
	for(u32 i = 0; i < imageCount; i++)
	{
		VkCommandBufferBeginInfo beginInfo =  { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO }; 
		vkCall(vkBeginCommandBuffer(commandBuffers[i], &beginInfo)); 

		VkClearValue clearValue; 
		clearValue.color.float32[0] = 0;
		clearValue.color.float32[1] = 0.1;
		clearValue.color.float32[2] = 0.3;
		clearValue.color.float32[3] = 1;

		VkRenderPassBeginInfo renderPassBeginInfo = 
		{
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO, 
			.renderPass = renderPass,
			.framebuffer = framebuffers[i],
			.renderArea.offset =  (VkOffset2D){ 0, 0 }, 
			.renderArea.extent =  (VkExtent2D){ WINDOW_WIDTH , WINDOW_HEIGHT },
			.clearValueCount = 1, 
			.pClearValues = &clearValue
		};
		vkCmdBeginRenderPass(commandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
		VkBuffer vertexBuffers[1] =  { vertexBuffer };
		VkDeviceSize offsets[1] = { 0 };
		vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);
		vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);
		vkCmdEndRenderPass(commandBuffers[i]);
		vkCall(vkEndCommandBuffer(commandBuffers[i]));
	}


	u64 count = 0;
	while(!glfwWindowShouldClose(window))
	{		
		u32 imageIndex;
		VkResult err = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, imageAvailableSemphore, VK_NULL_HANDLE, &imageIndex);
		switch(err)
		{
			case VK_SUCCESS:
			case VK_SUBOPTIMAL_KHR: 
					break;
			case VK_ERROR_OUT_OF_DATE_KHR: 
				log_msg("Window is resized"); 
				break;
			default:
				log_err("Error occured with Swap chain image acquisition, Count %d", count);
		}

		u32 waitStages[1] =  {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
		VkSubmitInfo submitInfo = 
		{
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.commandBufferCount = 1,
			.pCommandBuffers = &commandBuffers[imageIndex], 
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &imageAvailableSemphore,
			.pWaitDstStageMask = waitStages,
			.signalSemaphoreCount = 1,
			.pSignalSemaphores = &renderingCompleteSemaphore,

		}; 

		vkCall(vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE));

		VkPresentInfoKHR presentInfo = 
		{
			.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &renderingCompleteSemaphore,
			.swapchainCount = 1,
			.pSwapchains = &swapchain,
			.pImageIndices = &imageIndex
		};

		vkCall(vkQueuePresentKHR(presentQueue, &presentInfo));

		glfwPollEvents();
//		glfwSwapBuffers(window);
		count++;
	}

	vkDestroyBuffer(device, vertexBuffer, NULL);
	vkFreeMemory(device, deviceMemory, NULL);
	vkDestroyPipelineLayout(device, pipelineLayout, NULL); 
	vkDestroyPipeline(device, graphicsPipeline, NULL);
	vkDestroyCommandPool(device, commandPool, NULL);
	vkDestroySemaphore(device, imageAvailableSemphore, NULL); 
	vkDestroySemaphore(device, renderingCompleteSemaphore, NULL);
	//vkDestroyShaderModule(device, vertexShaderModule, NULL); 
	//vkDestroyShaderModule(device, fragmentShaderModule, NULL);
	vkDestroySwapchainKHR(device, swapchain, NULL);
	vkDestroyDevice(device, NULL);
	vkDestroyInstance(instance, NULL);
	return 0;
}