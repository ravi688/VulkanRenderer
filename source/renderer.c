

#include <engine/renderer/renderer.h>
#include <exception/exception.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string/string.h>		//custom string library
#include <string.h>				//standard string library

#include <tuple/header_config.h>
#include <tuple/tuple.h>

#include <garbage_collector/garbage_collector.h>
#include <vulkan/vulkan_wrapper.h>

#include <exception/exception.h>
#include <debug.h>

#include <utilities/file_utility.h>		//for load_binary_from_file()


typedef VkCommandBuffer* pVkCommandBuffer_t;
instantiate_tuple_t(uint32_t, pVkCommandBuffer_t);

typedef VkFramebuffer* pVkFramebuffer_t;
instantiate_tuple_t(uint32_t, pVkFramebuffer_t);

typedef VkImage* pVkImage_t;
instantiate_tuple_t(uint32_t, pVkImage_t);

typedef VkImageView* pVkImageView_t;
instantiate_tuple_t(uint32_t, pVkImageView_t);

define_exception(VULKAN_GRAPHICS_QUEUE_NOT_FOUND); 
declare_exception(VULKAN_GRAPHICS_QUEUE_NOT_FOUND);

define_exception(VULKAN_SURFACE_NOT_SUPPORTED);
declare_exception(VULKAN_SURFACE_NOT_SUPPORTED);

define_exception(VULKAN_PHYSICAL_DEVICE_EXTENSION_NOT_SUPPORTED); 
declare_exception(VULKAN_PHYSICAL_DEVICE_EXTENSION_NOT_SUPPORTED);


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
} renderer_t;

typedef enum
{
	VERTEX_SHADER,
	FRAGMENT_SHADER
} shader_type_t;

void* renderer_get_vulkan_instance(renderer_t* renderer) { return (void*)(&(renderer->vk_instance)); }
void* renderer_get_vulkan_device(renderer_t* renderer) { return (void*)(&(renderer->vk_device)); }
void* renderer_get_vulkan_surface(renderer_t* renderer) { return (void*)(&(renderer->vk_surface)); }
void* renderer_get_vulkan_swapchain(renderer_t* renderer) { return (void*)(&(renderer->vk_swapchain)); }
static VkSwapchainKHR vk_get_swapchain(renderer_t* renderer);
static void vk_dump_physical_device_extensions(renderer_t* renderer);
static tuple_t(uint32_t, pVkImage_t) vk_get_images(renderer_t* renderer);
static tuple_t(uint32_t, pVkImageView_t) vk_get_image_views(renderer_t* renderer);
static VkShaderModule vk_get_shader_module(renderer_t* renderer, const char* file_name);
static VkPipelineShaderStageCreateInfo vk_get_pipeline_shader_stage_create_info(VkShaderModule shader_module, shader_type_t shader_type, const char* entry_point);
static VkPipelineVertexInputStateCreateInfo vk_get_pipeline_vertex_input_state_create_info(void);
static VkPipelineInputAssemblyStateCreateInfo vk_get_pipeline_input_assembly_state_create_info(void); 
static VkPipelineViewportStateCreateInfo vk_get_pipeline_viewport_state_create_info(uint32_t viewportWidth, uint32_t viewportHeight);
static VkPipelineRasterizationStateCreateInfo vk_get_pipeline_rasterization_state_create_info(void);
static VkPipelineMultisampleStateCreateInfo vk_get_pipeline_multisample_state_create_info(void);
static VkPipelineColorBlendStateCreateInfo vk_get_pipeline_color_blend_state_create_info(void);
static VkPipelineColorBlendAttachmentState vk_get_pipeline_color_blend_attachment_state(void);
static VkPipelineDynamicStateCreateInfo vk_get_pipeline_dynamic_state_create_info(void);
static VkPipeline vk_get_graphics_pipeline(renderer_t* renderer, VkPipelineLayout pipelineLayout, VkRenderPass renderPass,
											VkPipelineShaderStageCreateInfo* shaderStages,
											VkPipelineVertexInputStateCreateInfo* vertexInputState, 
											VkPipelineInputAssemblyStateCreateInfo* inputAssemblyState, 
											VkPipelineViewportStateCreateInfo* viewportState, 
											VkPipelineRasterizationStateCreateInfo* rasterizationState, 
											VkPipelineMultisampleStateCreateInfo* multisampleState, 
											VkPipelineColorBlendStateCreateInfo* colorBlendState);
static tuple_t(uint32_t, pVkFramebuffer_t) vk_get_framebuffers(renderer_t* renderer);
static tuple_t(uint32_t, pVkCommandBuffer_t) vk_get_commandbuffers(renderer_t* renderer);
static VkAttachmentReference vk_get_attachment_reference(void);
static VkSubpassDependency vk_get_subpass_dependency(void);
static VkSemaphore vk_get_semaphore(renderer_t* renderer);
static VkAttachmentDescription vk_get_attachment_description(VkFormat image_format);
static VkSubpassDescription vk_get_subpass_description(VkAttachmentReference attachment_reference);
static VkRenderPass vk_get_render_pass(renderer_t* renderer, VkAttachmentDescription* attachments, VkSubpassDescription* subpasses, VkSubpassDependency* subpassDependencies);
static VkPipelineLayout vk_get_pipeline_layout(renderer_t* renderer);
static VkViewport vk_get_viewport(uint32_t width, uint32_t height);
static void vk_setup_fixed_function_pipline(renderer_t* renderer);
static void record_commands(renderer_t* renderer, tuple_t(uint32_t, pVkCommandBuffer_t) commandBuffers);

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
	vk_dump_physical_device_extensions(renderer);
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
	renderer->vk_images = vk_get_images(renderer); 
	renderer->vk_image_views = vk_get_image_views(renderer);
 
	VkPipelineShaderStageCreateInfo shaderStages[] =
	{ 
		vk_get_pipeline_shader_stage_create_info(vk_get_shader_module(renderer, "shaders/vertexShader.spv"), VERTEX_SHADER, "main"), 
		vk_get_pipeline_shader_stage_create_info(vk_get_shader_module(renderer, "shaders/fragmentShader.spv"), FRAGMENT_SHADER, "main")
	};

	VkPipelineVertexInputStateCreateInfo vertexInputInfo = vk_get_pipeline_vertex_input_state_create_info();

	//Fixed functions configuration
	VkPipelineInputAssemblyStateCreateInfo inputAssembly = vk_get_pipeline_input_assembly_state_create_info();
	VkPipelineViewportStateCreateInfo viewportState = vk_get_pipeline_viewport_state_create_info(renderer->vk_extent.width, renderer->vk_extent.height);
	VkPipelineRasterizationStateCreateInfo rasterizer = vk_get_pipeline_rasterization_state_create_info();
	VkPipelineMultisampleStateCreateInfo multisampling = vk_get_pipeline_multisample_state_create_info();
	VkPipelineColorBlendStateCreateInfo colorBlending = vk_get_pipeline_color_blend_state_create_info();
	VkPipelineDynamicStateCreateInfo dynamicState = vk_get_pipeline_dynamic_state_create_info();

	renderer->vk_pipeline_layout = vk_get_pipeline_layout(renderer);

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

	renderer->vk_pipeline = vk_get_graphics_pipeline(renderer, renderer->vk_pipeline_layout, renderer->vk_render_pass,
													shaderStages,
													&vertexInputInfo, 
													&inputAssembly, 
													&viewportState, 
													&rasterizer, 
													&multisampling,
													&colorBlending);

    renderer->vk_framebuffers = vk_get_framebuffers(renderer); //swapchainFramebuffers

	record_commands(renderer,  vk_get_commandbuffers(renderer));

    renderer->vk_image_available_semaphore = vk_get_semaphore(renderer);
    renderer->vk_render_finished_semaphore = vk_get_semaphore(renderer);
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

static VkSemaphore vk_get_semaphore(renderer_t* renderer)
{
	VkSemaphore semaphore;
	VkSemaphoreCreateInfo createInfo = { }; 
	createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	vkCall(vkCreateSemaphore(renderer->vk_device, &createInfo, NULL, &semaphore)) 
	return semaphore;
}

static void record_commands(renderer_t* renderer, tuple_t(uint32_t, pVkCommandBuffer_t) commandBuffers)
{
	for (uint32_t i = 0; i < commandBuffers.value1; i++)
	{
    	VkCommandBufferBeginInfo beginInfo = {};
    	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    	beginInfo.flags = 0; // Optional
    	beginInfo.pInheritanceInfo = NULL; // Optional
    	vkCall(vkBeginCommandBuffer(commandBuffers.value2[i], &beginInfo));

    	VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass =  renderer->vk_render_pass;
		renderPassInfo.framebuffer = renderer->vk_framebuffers.value2[i];
		renderPassInfo.renderArea.offset = (VkOffset2D) {0, 0};
		renderPassInfo.renderArea.extent = renderer->vk_extent;

		VkClearValue clearColor = {{{0.1f, 0.05f, 0.1f, 1.0f}}};
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(commandBuffers.value2[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(commandBuffers.value2[i], VK_PIPELINE_BIND_POINT_GRAPHICS, renderer->vk_pipeline);
		vkCmdDraw(commandBuffers.value2[i], 3, 1, 0, 0);
		vkCmdEndRenderPass(commandBuffers.value2[i]);
		vkCall(vkEndCommandBuffer(commandBuffers.value2[i]));
    }
}

static tuple_t(uint32_t, pVkCommandBuffer_t) vk_get_commandbuffers(renderer_t* renderer)
{
	tuple_t(uint32_t, pVkCommandBuffer_t) commandBuffers = (tuple_t(uint32_t, pVkCommandBuffer_t)) { renderer->vk_framebuffers.value1, GC_NEWV(VkCommandBuffer, renderer->vk_framebuffers.value1) };
	VkCommandPool commandPool; 
	VkCommandPoolCreateInfo commandPoolCreateInfo = { } ; 
	commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO; 
	commandPoolCreateInfo.queueFamilyIndex = renderer->vk_graphics_queue_index;
	vkCall(vkCreateCommandPool(renderer->vk_device, &commandPoolCreateInfo, NULL, &commandPool));
	VkCommandBufferAllocateInfo allocInfo = { }; 
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO; 
	allocInfo.commandPool = commandPool; 
	allocInfo.commandBufferCount = commandBuffers.value1;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	vkAllocateCommandBuffers(renderer->vk_device, &allocInfo, commandBuffers.value2);
	renderer->vk_command_pool = commandPool;
	renderer->vk_command_buffers = commandBuffers;
	return commandBuffers;
}

static tuple_t(uint32_t, pVkFramebuffer_t) vk_get_framebuffers(renderer_t* renderer)
{
	tuple_t(uint32_t, pVkFramebuffer_t) swapchainFramebuffers = (tuple_t(uint32_t, pVkFramebuffer_t)) { renderer->vk_images.value1, GC_NEWV(VkFramebuffer, renderer->vk_images.value1) }; 
	for(uint32_t i = 0; i < swapchainFramebuffers.value1; i++)
	{
		VkFramebufferCreateInfo createInfo = { }; 
		createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		createInfo.renderPass = renderer->vk_render_pass;
		createInfo.pAttachments = &(renderer->vk_image_views.value2[i]); 
		createInfo.attachmentCount = 1; 
		createInfo.width = renderer->vk_extent.width; 
		createInfo.height = renderer->vk_extent.height; 
		createInfo.layers = 1;
		vkCall(vkCreateFramebuffer(renderer->vk_device, &createInfo, NULL, &(swapchainFramebuffers.value2[i])));
	}
	return swapchainFramebuffers;
}


static VkPipeline vk_get_graphics_pipeline(renderer_t* renderer, VkPipelineLayout pipelineLayout, VkRenderPass renderPass,
											VkPipelineShaderStageCreateInfo* shaderStages,
											VkPipelineVertexInputStateCreateInfo* vertexInputState, 
											VkPipelineInputAssemblyStateCreateInfo* inputAssemblyState,
											VkPipelineViewportStateCreateInfo* viewportState, 
											VkPipelineRasterizationStateCreateInfo* rasterizationState,
											VkPipelineMultisampleStateCreateInfo* multisampleState, 
											VkPipelineColorBlendStateCreateInfo* colorBlendState)
{
	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = vertexInputState;
	pipelineInfo.pInputAssemblyState = inputAssemblyState;
	pipelineInfo.pViewportState = viewportState;
	pipelineInfo.pRasterizationState = rasterizationState;
	pipelineInfo.pMultisampleState = multisampleState;
	pipelineInfo.pDepthStencilState = NULL; // Optional
	pipelineInfo.pColorBlendState = colorBlendState;
	pipelineInfo.pDynamicState = NULL; // Optional
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
	pipelineInfo.basePipelineIndex = -1; // Optional

	VkPipeline graphicsPipeline;
	vkCall(vkCreateGraphicsPipelines(renderer->vk_device, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &graphicsPipeline));
	return graphicsPipeline;
}


static VkRenderPass vk_get_render_pass(renderer_t* renderer, VkAttachmentDescription* attachments, VkSubpassDescription* subpasses, VkSubpassDependency* subpassDependencies)
{
	VkRenderPass renderPass;
 	VkRenderPassCreateInfo renderPassCreateInfo = {};
	renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.attachmentCount = 1;
	renderPassCreateInfo.pAttachments = attachments;
	renderPassCreateInfo.subpassCount = 1;
	renderPassCreateInfo.pSubpasses = subpasses;
	renderPassCreateInfo.dependencyCount = 1;
	renderPassCreateInfo.pDependencies = subpassDependencies;
	vkCall(vkCreateRenderPass(renderer->vk_device, &renderPassCreateInfo, NULL, &renderPass));
	return renderPass;
}

static VkSubpassDependency vk_get_subpass_dependency()
{
	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	return dependency;
}

static VkSubpassDescription vk_get_subpass_description(VkAttachmentReference attachment_reference)
{
	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &attachment_reference;
	return subpass;
}

static VkAttachmentReference vk_get_attachment_reference()
{
	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	return colorAttachmentRef;
}

static VkAttachmentDescription vk_get_attachment_description(VkFormat image_format)
{
	VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = image_format;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	return colorAttachment;
}

static VkPipelineLayout vk_get_pipeline_layout(renderer_t* renderer)
{
	VkPipelineLayout pipelineLayout;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0; // Optional
	pipelineLayoutInfo.pSetLayouts = NULL; // Optional
	pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
	pipelineLayoutInfo.pPushConstantRanges = NULL; // Optional

	vkCall(vkCreatePipelineLayout(renderer->vk_device, &pipelineLayoutInfo, NULL, &pipelineLayout));
	return pipelineLayout;
}

static VkPipelineDynamicStateCreateInfo vk_get_pipeline_dynamic_state_create_info(void)
{
	VkDynamicState* dynamicStates = GC_NEWV(VkDynamicState, 2);
	dynamicStates[0] = VK_DYNAMIC_STATE_VIEWPORT;
	dynamicStates[1] = VK_DYNAMIC_STATE_LINE_WIDTH;

	VkPipelineDynamicStateCreateInfo createInfo =  { }; 
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	createInfo.dynamicStateCount = 1; 
	createInfo.pDynamicStates = dynamicStates;
	return createInfo;
}

static VkPipelineColorBlendStateCreateInfo vk_get_pipeline_color_blend_state_create_info(void)
{
	VkPipelineColorBlendStateCreateInfo createInfo = { };
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	createInfo.logicOpEnable = VK_FALSE;
	createInfo.logicOp = VK_LOGIC_OP_COPY; // Optional
	createInfo.attachmentCount = 1;
	VkPipelineColorBlendAttachmentState* state = GC_NEW(VkPipelineColorBlendAttachmentState); 
	*state = vk_get_pipeline_color_blend_attachment_state();
	createInfo.pAttachments = state;
	createInfo.blendConstants[0] = 0.0f; // Optional
	createInfo.blendConstants[1] = 0.0f; // Optional
	createInfo.blendConstants[2] = 0.0f; // Optional
	createInfo.blendConstants[3] = 0.0f; // Optional
	return createInfo;
}

static VkPipelineColorBlendAttachmentState vk_get_pipeline_color_blend_attachment_state(void)
{
	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional	
	return colorBlendAttachment;
}

static VkPipelineMultisampleStateCreateInfo vk_get_pipeline_multisample_state_create_info(void)
{
	VkPipelineMultisampleStateCreateInfo createInfo =  { };
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	createInfo.sampleShadingEnable = VK_FALSE;
	createInfo.rasterizationSamples = VK_SAMPLE_COUNT_2_BIT;
	createInfo.minSampleShading = 1.0f; // Optional
	createInfo.pSampleMask = NULL; // Optional
	createInfo.alphaToCoverageEnable = VK_FALSE; // Optional
	createInfo.alphaToOneEnable = VK_FALSE; // Optional
	return createInfo;
}

static VkPipelineRasterizationStateCreateInfo vk_get_pipeline_rasterization_state_create_info(void)
{
	VkPipelineRasterizationStateCreateInfo createInfo =  { }; 
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	createInfo.depthClampEnable = VK_FALSE;
	createInfo.rasterizerDiscardEnable = VK_FALSE;
	createInfo.polygonMode = VK_POLYGON_MODE_FILL;
	createInfo.lineWidth = 1.0f;
	createInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	createInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	createInfo.depthBiasEnable = VK_FALSE;
	createInfo.depthBiasConstantFactor = 0.0f; // Optional
	createInfo.depthBiasClamp = 0.0f; // Optional
	createInfo.depthBiasSlopeFactor = 0.0f; // Optional
	return createInfo;
}

static VkPipelineViewportStateCreateInfo vk_get_pipeline_viewport_state_create_info(uint32_t viewportWidth, uint32_t viewportHeight)
{
	VkPipelineViewportStateCreateInfo createInfo =  { }; 
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	createInfo.viewportCount = 1;
	VkViewport* viewport = GC_NEW(VkViewport);
	*viewport = vk_get_viewport(viewportWidth, viewportHeight);
	VkRect2D* scissor = GC_NEW(VkRect2D);
	*scissor =  (VkRect2D) { .offset =  { 0, 0 }, .extent =  { viewportWidth, viewportHeight } };
	createInfo.pViewports = viewport;
	createInfo.scissorCount = 1; 
	createInfo.pScissors = scissor;
	return createInfo;
}

static VkPipelineInputAssemblyStateCreateInfo vk_get_pipeline_input_assembly_state_create_info()
{
	VkPipelineInputAssemblyStateCreateInfo createInfo  = {  }; 
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	createInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; 
	createInfo.primitiveRestartEnable = VK_FALSE;
	return createInfo;
}

static VkViewport vk_get_viewport(uint32_t width, uint32_t height)
{
	VkViewport viewport = { }; 
	viewport.x = 0; 
	viewport.y = 0; 
	viewport.width = width; 
	viewport.height = height; 
	viewport.minDepth = 0.0f; 
	viewport.maxDepth = 1.0f;
	return viewport;
}

static VkPipelineVertexInputStateCreateInfo vk_get_pipeline_vertex_input_state_create_info()
{
	VkPipelineVertexInputStateCreateInfo createInfo =  { }; 
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO; 
	createInfo.vertexBindingDescriptionCount = 0; 
	createInfo.pVertexBindingDescriptions = NULL; 
	createInfo.vertexAttributeDescriptionCount = 0; 
	createInfo.pVertexAttributeDescriptions = NULL; 
	return createInfo;
}

static VkPipelineShaderStageCreateInfo vk_get_pipeline_shader_stage_create_info(VkShaderModule shader_module, shader_type_t shader_type, const char* entry_point)
{
	VkPipelineShaderStageCreateInfo createInfo = { }; 
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	createInfo.pName = entry_point; 
	createInfo.module = shader_module; 
	createInfo.stage = (shader_type == VERTEX_SHADER) ? VK_SHADER_STAGE_VERTEX_BIT : VK_SHADER_STAGE_FRAGMENT_BIT; 
	return createInfo;
}

static VkShaderModule vk_get_shader_module(renderer_t* renderer, const char* file_name)
{
	tuple_t(uint64_t, pchar_t) shader_bytes = load_binary_from_file(file_name);
	VkShaderModuleCreateInfo createInfo = {}; 
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO; 
	createInfo.codeSize = shader_bytes.value1;
	createInfo.pCode = (const uint32_t*)shader_bytes.value2; 		//convert from char* to uint32_t* 
	VkShaderModule shaderModule;
	vkCreateShaderModule(renderer->vk_device, &createInfo, NULL, &shaderModule); 
	return shaderModule;
}

static tuple_t(uint32_t, pVkImageView_t) vk_get_image_views(renderer_t* renderer)
{
	tuple_t(uint32_t, pVkImageView_t) imageViews  =  { renderer->vk_images.value1, GC_NEWV(VkImageView, renderer->vk_images.value1) }; 
	for(uint32_t i = 0; i < imageViews.value1; i++)
	{
		VkImageViewCreateInfo createInfo =  { };
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO; 
		createInfo.image = renderer->vk_images.value2[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = renderer->vk_format;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY; 
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT; 
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;
		vkCall(vkCreateImageView(renderer->vk_device, &createInfo, NULL, &(imageViews.value2[i])));
	}
	return imageViews;
}

static tuple_t(uint32_t, pVkImage_t) vk_get_images(renderer_t* renderer)
{
	tuple_t(uint32_t, pVkImage_t) pair; 
	vkCall(vkGetSwapchainImagesKHR(renderer->vk_device, renderer->vk_swapchain, &(pair.value1), NULL)); 
	pair.value2 = GC_NEWV(VkImage, pair.value1); 
	vkCall(vkGetSwapchainImagesKHR(renderer->vk_device, renderer->vk_swapchain, &(pair.value1), pair.value2)); 
	return pair;
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

void renderer_terminate(renderer_t* renderer)
{
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

static void vk_dump_physical_device_extensions(renderer_t* renderer)
{
	log_msg("Physical Device Extensions-----------------------");
	tuple_t(uint32_t, pVkExtensionProperties_t) extensions = vk_get_physical_device_extension_properties(renderer->vk_physical_device);
	for(uint32_t i = 0; i < extensions.value1; i++)
		log_msg(extensions.value2[i].extensionName);
}