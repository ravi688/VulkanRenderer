

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

static void vk_dump_physical_device_extensions(renderer_t* renderer);
static tuple_t(uint32_t, pVkImage_t) vk_get_images(renderer_t* renderer);
static tuple_t(uint32_t, pVkImageView_t) vk_get_image_views(renderer_t* renderer);
static VkShaderModule vk_get_shader_module(renderer_t* renderer, const char* file_name);

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

void* renderer_get_vulkan_instance(renderer_t* renderer) { return (void*)(&(renderer->vk_instance)); }
void* renderer_get_vulkan_device(renderer_t* renderer) { return (void*)(&(renderer->vk_device)); }
void* renderer_get_vulkan_surface(renderer_t* renderer) { return (void*)(&(renderer->vk_surface)); }
void* renderer_get_vulkan_swapchain(renderer_t* renderer) { return (void*)(&(renderer->vk_swapchain)); }
static VkSwapchainKHR vk_get_swapchain(renderer_t* renderer);


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

	VkShaderModule vertexShaderModule = vk_get_shader_module(renderer, "shaders/vertexShader.spv"); 
	VkShaderModule fragmentShaderModule = vk_get_shader_module(renderer, "shaders/fragmentShader.spv"); 

	VkPipelineShaderStageCreateInfo vertexShaderStageInfo =  { }; 
	vertexShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertexShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertexShaderStageInfo.module = vertexShaderModule; 
	vertexShaderStageInfo.pName = "main";
	
	VkPipelineShaderStageCreateInfo fragmentShaderStageInfo	=  { }; 
	fragmentShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO; 
	fragmentShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT; 
	fragmentShaderStageInfo.module = fragmentShaderModule; 
	fragmentShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertexShaderStageInfo, fragmentShaderStageInfo};

	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 0;
	vertexInputInfo.pVertexBindingDescriptions = NULL; // Optional
	vertexInputInfo.vertexAttributeDescriptionCount = 0;
	vertexInputInfo.pVertexAttributeDescriptions = NULL; 

	//Fixed functions configuration
	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;	

	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float) renderer->vk_extent.width;
	viewport.height = (float) renderer->vk_extent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset = (VkOffset2D) {0, 0};
	scissor.extent = renderer->vk_extent;

	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	rasterizer.depthBiasClamp = 0.0f; // Optional
	rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f; // Optional
	multisampling.pSampleMask = NULL; // Optional
	multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
	multisampling.alphaToOneEnable = VK_FALSE; // Optional

	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional


	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f; // Optional
	colorBlending.blendConstants[1] = 0.0f; // Optional
	colorBlending.blendConstants[2] = 0.0f; // Optional
	colorBlending.blendConstants[3] = 0.0f; // Optional

	VkDynamicState dynamicStates[] = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_LINE_WIDTH
	};

	VkPipelineDynamicStateCreateInfo dynamicState = {};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = 2;
	dynamicState.pDynamicStates = dynamicStates;

	VkPipelineLayout pipelineLayout;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0; // Optional
	pipelineLayoutInfo.pSetLayouts = NULL; // Optional
	pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
	pipelineLayoutInfo.pPushConstantRanges = NULL; // Optional

	vkCall(vkCreatePipelineLayout(renderer->vk_device, &pipelineLayoutInfo, NULL, &pipelineLayout));

	renderer->vk_pipeline_layout = pipelineLayout;

	VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = renderer->vk_format;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	
	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPass renderPass;
	VkRenderPassCreateInfo renderPassCreateInfo = {};
	renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.attachmentCount = 1;
	renderPassCreateInfo.pAttachments = &colorAttachment;
	renderPassCreateInfo.subpassCount = 1;
	renderPassCreateInfo.pSubpasses = &subpass;
	renderPassCreateInfo.dependencyCount = 1;
	renderPassCreateInfo.pDependencies = &dependency;
	vkCall(vkCreateRenderPass(renderer->vk_device, &renderPassCreateInfo, NULL, &renderPass));
	renderer->vk_render_pass = renderPass;

	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;

	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = NULL; // Optional
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = NULL; // Optional
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
	pipelineInfo.basePipelineIndex = -1; // Optional

	VkPipeline graphicsPipeline;
	vkCall(vkCreateGraphicsPipelines(renderer->vk_device, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &graphicsPipeline));
	renderer->vk_pipeline = graphicsPipeline;


	tuple_t(uint32_t, pVkFramebuffer_t) swapChainFramebuffers = (tuple_t(uint32_t, pVkFramebuffer_t)) { renderer->vk_images.value1, GC_NEWV(VkFramebuffer, renderer->vk_images.value1) };
	for(uint32_t i = 0; i < swapChainFramebuffers.value1; i++)
	{
		 VkImageView attachments[] = {
        	renderer->vk_image_views.value2[i]
    	};

    	VkFramebufferCreateInfo framebufferInfo = {};
    	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    	framebufferInfo.renderPass = renderPass;
    	framebufferInfo.attachmentCount = 1;
    	framebufferInfo.pAttachments = attachments;
    	framebufferInfo.width = renderer->vk_extent.width;
    	framebufferInfo.height = renderer->vk_extent.height;
    	framebufferInfo.layers = 1;
    	vkCall(vkCreateFramebuffer(renderer->vk_device, &framebufferInfo, NULL, &(swapChainFramebuffers.value2[i])));
    }
    renderer->vk_framebuffers = swapChainFramebuffers;

    VkCommandPool commandPool;
    VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = renderer->vk_graphics_queue_index;
	poolInfo.flags = 0; // Optional
	vkCall(vkCreateCommandPool(renderer->vk_device, &poolInfo, NULL, &commandPool));
	renderer->vk_command_pool = commandPool;

	tuple_t(uint32_t, pVkCommandBuffer_t) commandBuffers = (tuple_t(uint32_t, pVkCommandBuffer_t)) { renderer->vk_framebuffers.value1, GC_NEWV(VkCommandBuffer, renderer->vk_framebuffers.value1) }; 
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t) commandBuffers.value1;
	vkCall(vkAllocateCommandBuffers(renderer->vk_device, &allocInfo, commandBuffers.value2));

	for (uint32_t i = 0; i < commandBuffers.value1; i++)
	{
    	VkCommandBufferBeginInfo beginInfo = {};
    	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    	beginInfo.flags = 0; // Optional
    	beginInfo.pInheritanceInfo = NULL; // Optional
    	vkCall(vkBeginCommandBuffer(commandBuffers.value2[i], &beginInfo));

    	VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass;
		renderPassInfo.framebuffer = renderer->vk_framebuffers.value2[i];
		renderPassInfo.renderArea.offset = (VkOffset2D) {0, 0};
		renderPassInfo.renderArea.extent = renderer->vk_extent;

		VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(commandBuffers.value2[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(commandBuffers.value2[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
		vkCmdDraw(commandBuffers.value2[i], 3, 1, 0, 0);
		vkCmdEndRenderPass(commandBuffers.value2[i]);
		vkCall(vkEndCommandBuffer(commandBuffers.value2[i]));
    }

    renderer->vk_command_buffers = commandBuffers;

    VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;

	VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    vkCall(vkCreateSemaphore(renderer->vk_device, &semaphoreInfo, NULL, &imageAvailableSemaphore));
    vkCall(vkCreateSemaphore(renderer->vk_device, &semaphoreInfo, NULL, &renderFinishedSemaphore));

    renderer->vk_image_available_semaphore = imageAvailableSemaphore;
    renderer->vk_render_finished_semaphore = renderFinishedSemaphore;
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