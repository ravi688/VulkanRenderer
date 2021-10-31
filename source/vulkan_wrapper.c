

#include <vulkan/vulkan_wrapper.h>
#include <exception/exception.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string/string.h>		//custom string library
#include <string.h>				//standard string library
#include <garbage_collector/garbage_collector.h>
#include <utilities/file_utility.h>
#include <debug.h>

define_exception(VULKAN_ABORTED);
define_exception(VULKAN_LAYER_NOT_SUPPORTED);
define_exception(VULKAN_DEVICE_NOT_FOUND);
define_exception(VULKAN_EXTENSION_NOT_SUPPORTED);
define_exception(VULKAN_GRAPHICS_QUEUE_NOT_FOUND); 
define_exception(VULKAN_SURFACE_NOT_SUPPORTED);
define_exception(VULKAN_PHYSICAL_DEVICE_EXTENSION_NOT_SUPPORTED);

VkDeviceMemory vk_get_device_memory_for_buffer(VkDevice device, VkPhysicalDevice physicalDevice, VkBuffer buffer, uint64_t size, uint32_t memoryProperties)
{
	VkDeviceMemory deviceMemory;
    VkMemoryRequirements memRequirements;
	VkPhysicalDeviceMemoryProperties memProperties;

	vkGetBufferMemoryRequirements(device, buffer, &memRequirements);
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	int32_t selectedMemoryType = -1;
	uint32_t typeFilter = memRequirements.memoryTypeBits;
	uint32_t properties = memoryProperties;
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
	    if ((typeFilter & (1 << i)) && ((memProperties.memoryTypes[i].propertyFlags & properties) == properties))
	    {
	    	selectedMemoryType = (int32_t)i;
	        break;
	    }
	}
EXCEPTION_BLOCK
(
	if(selectedMemoryType == -1)
		throw_exception(VULKAN_ABORTED);
)

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = (uint32_t)selectedMemoryType;
	vkCall(vkAllocateMemory(device, &allocInfo, NULL, &deviceMemory));
	vkCall(vkBindBufferMemory(device, buffer, deviceMemory, 0));
	return deviceMemory;
}

VkBuffer vk_get_buffer(VkDevice device, VkDeviceSize size, VkBufferUsageFlags usageFlags, VkSharingMode sharingMode)
{
	VkBuffer buffer;
	VkBufferCreateInfo createInfo = { }; 
	createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO; 
	createInfo.size = size; 
	createInfo.usage = usageFlags;
	createInfo.sharingMode = sharingMode;
	vkCall(vkCreateBuffer(device, &createInfo, NULL, &buffer));
	return buffer;
}

VkSemaphore vk_get_semaphore(VkDevice device)
{
	VkSemaphore semaphore;
	VkSemaphoreCreateInfo createInfo = { }; 
	createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	vkCall(vkCreateSemaphore(device, &createInfo, NULL, &semaphore)); 
	return semaphore;
}

VkCommandPool vk_get_command_pool(VkDevice device, uint32_t queueFamilyIndex)
{
	VkCommandPoolCreateInfo createInfo = { };
	createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	createInfo.queueFamilyIndex = queueFamilyIndex;
	VkCommandPool commandPool;
	vkCall(vkCreateCommandPool(device, &createInfo, NULL, &commandPool));
	return commandPool;
}

tuple_t(uint32_t, pVkCommandBuffer_t) vk_get_command_buffers(VkDevice device, VkCommandPool commandPool, uint32_t count)
{
	tuple_t(uint32_t, pVkCommandBuffer_t) commandBuffers = { count, GC_NEWV(VkCommandBuffer, count) };
	VkCommandBufferAllocateInfo allocInfo = { }; 
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO; 
	allocInfo.commandPool = commandPool; 
	allocInfo.commandBufferCount = count;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	vkCall(vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.value2));
	return commandBuffers;
}

tuple_t(uint32_t, pVkFramebuffer_t) vk_get_framebuffers(VkDevice device, uint32_t count, VkRenderPass renderPass, VkExtent2D extent, uint32_t layer, VkImageView* attachments)
{
	tuple_t(uint32_t, pVkFramebuffer_t) framebuffers = { count, GC_NEWV(VkFramebuffer, count) };
	for(uint32_t i = 0; i < count; i++)
	{
		VkFramebufferCreateInfo createInfo = { }; 
		createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		createInfo.renderPass = renderPass;
		createInfo.pAttachments = &attachments[i]; 
		createInfo.attachmentCount = 1; 
		createInfo.width = extent.width; 
		createInfo.height = extent.height;
		createInfo.layers = layer;
		VkFramebuffer framebuffer;
		vkCall(vkCreateFramebuffer(device, &createInfo, NULL, &framebuffer));
		framebuffers.value2[i] = framebuffer;

	}
	return framebuffers;
}

VkQueue vk_get_device_queue(VkDevice device, uint32_t queueFamilyIndex, uint32_t queueIndex)
{
	VkQueue queue; 
	vkGetDeviceQueue(device, queueFamilyIndex, queueIndex, &queue); 
	return queue;
}

VkSwapchainKHR vk_get_swapchain(VkDevice device, VkSwapchainCreateInfoKHR* createInfo)
{
	VkSwapchainKHR swapchain;
	vkCall(vkCreateSwapchainKHR(device, createInfo, NULL, &swapchain)); 
	return swapchain;
}

VkPipeline vk_get_graphics_pipeline(VkDevice device, VkPipelineLayout pipelineLayout, VkRenderPass renderPass,
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
	//TODO: Make this customizable; for more shader stages in the pipeline
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
	vkCall(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &graphicsPipeline));
	return graphicsPipeline;
}

VkRenderPass vk_get_render_pass(VkDevice device, VkFormat format)
{
	VkAttachmentDescription colorAttachmentDescription = vk_get_attachment_description(format);
	VkAttachmentReference colorAttachmentReference = vk_get_attachment_reference();  //TODO: this should be like vk_get_attachment_reference(index: 0);
	VkSubpassDescription subpass  = 
	{
		.pColorAttachments = &colorAttachmentReference,
		.colorAttachmentCount = 1, 
		.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS
	};

	VkRenderPass renderPass;
 	VkRenderPassCreateInfo renderPassCreateInfo = {};
	renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.attachmentCount = 1;
	renderPassCreateInfo.pAttachments = &colorAttachmentDescription;
	renderPassCreateInfo.subpassCount = 1;
	renderPassCreateInfo.pSubpasses = &subpass;
	renderPassCreateInfo.dependencyCount = 0;
	renderPassCreateInfo.pDependencies = VK_NULL_HANDLE;
	vkCall(vkCreateRenderPass(device, &renderPassCreateInfo, NULL, &renderPass));
	return renderPass;
}

VkSubpassDependency vk_get_subpass_dependency()
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

VkSubpassDescription vk_get_subpass_description(VkAttachmentReference attachment_reference)
{
	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &attachment_reference;
	return subpass;
}

VkAttachmentReference vk_get_attachment_reference()
{
	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	return colorAttachmentRef;
}

VkAttachmentDescription vk_get_attachment_description(VkFormat image_format)
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

VkPipelineLayout vk_get_pipeline_layout(VkDevice device)
{
	VkPipelineLayout pipelineLayout;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0; // Optional
	pipelineLayoutInfo.pSetLayouts = NULL; // Optional
	pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
	pipelineLayoutInfo.pPushConstantRanges = NULL; // Optional

	vkCall(vkCreatePipelineLayout(device, &pipelineLayoutInfo, NULL, &pipelineLayout));
	return pipelineLayout;
}

VkPipelineDynamicStateCreateInfo vk_get_pipeline_dynamic_state_create_info(void)
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

VkPipelineColorBlendStateCreateInfo vk_get_pipeline_color_blend_state_create_info(void)
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

VkPipelineColorBlendAttachmentState vk_get_pipeline_color_blend_attachment_state(void)
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

VkPipelineMultisampleStateCreateInfo vk_get_pipeline_multisample_state_create_info(void)
{
	VkPipelineMultisampleStateCreateInfo createInfo =  { };
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	createInfo.sampleShadingEnable = VK_FALSE;
	createInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	createInfo.minSampleShading = 1.0f; // Optional
	createInfo.pSampleMask = NULL; // Optional
	createInfo.alphaToCoverageEnable = VK_FALSE; // Optional
	createInfo.alphaToOneEnable = VK_FALSE; // Optional
	return createInfo;
}

VkPipelineRasterizationStateCreateInfo vk_get_pipeline_rasterization_state_create_info(void)
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

VkPipelineViewportStateCreateInfo vk_get_pipeline_viewport_state_create_info(uint32_t viewportWidth, uint32_t viewportHeight)
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

VkPipelineInputAssemblyStateCreateInfo vk_get_pipeline_input_assembly_state_create_info()
{
	VkPipelineInputAssemblyStateCreateInfo createInfo  = {  }; 
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	createInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; 
	createInfo.primitiveRestartEnable = VK_FALSE;
	return createInfo;
}

VkViewport vk_get_viewport(uint32_t width, uint32_t height)
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

tuple_t(uint32_t, pVkVertexInputBindingDescription_t) vk_get_vertex_input_binding_descriptions(uint32_t stride, VkVertexInputRate vertexInputRate)
{
	tuple_t(uint32_t, pVkVertexInputBindingDescription_t) bindingDescriptions =  { 1 , GC_NEWV(VkVertexInputBindingDescription, 1) }; 
	bindingDescriptions.value2[0].binding = 0;
	bindingDescriptions.value2[0].stride = stride; 
	bindingDescriptions.value2[0].inputRate = vertexInputRate;
	return bindingDescriptions;
}

tuple_t(uint32_t, pVkVertexInputAttributeDescription_t) vk_get_vertex_input_attribute_descriptions(uint32_t attributeCount, VkFormat* attributeFormats, uint32_t* attributeOffsets)
{
	tuple_t(uint32_t, pVkVertexInputAttributeDescription_t) attributeDescriptions = { attributeCount, GC_NEWV(VkVertexInputAttributeDescription, attributeCount) };
	for(uint32_t i = 0; i < attributeDescriptions.value1; i++)
	{
		attributeDescriptions.value2[i].binding = 0; 
		attributeDescriptions.value2[i].location = i;
		attributeDescriptions.value2[i].format = attributeFormats[i]; 
		attributeDescriptions.value2[i].offset = attributeOffsets[i];
	}
	return attributeDescriptions;
}

VkPipelineVertexInputStateCreateInfo vk_get_pipeline_vertex_input_state_create_info(uint32_t attributeCount, uint32_t stride, VkVertexInputRate vertexInputRate, VkFormat* attributeFormats, uint32_t* attributeOffsets)
{
	tuple_t(uint32_t, pVkVertexInputBindingDescription_t) vertexBindings = vk_get_vertex_input_binding_descriptions(stride, vertexInputRate); 
	tuple_t(uint32_t, pVkVertexInputAttributeDescription_t) vertexAttributes = vk_get_vertex_input_attribute_descriptions(attributeCount, attributeFormats, attributeOffsets);
	VkPipelineVertexInputStateCreateInfo createInfo =  { }; 
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO; 
	createInfo.vertexBindingDescriptionCount = vertexBindings.value1; 
	createInfo.pVertexBindingDescriptions = vertexBindings.value2; 
	createInfo.vertexAttributeDescriptionCount = vertexAttributes.value1; 
	createInfo.pVertexAttributeDescriptions = vertexAttributes.value2; 
	return createInfo;
}

VkPipelineShaderStageCreateInfo vk_get_pipeline_shader_stage_create_info(VkShaderModule shader_module, shader_type_t shader_type, const char* entry_point)
{
	VkPipelineShaderStageCreateInfo createInfo = { }; 
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	createInfo.pName = entry_point; 
	createInfo.module = shader_module; 
	createInfo.stage = (shader_type == VERTEX_SHADER) ? VK_SHADER_STAGE_VERTEX_BIT : VK_SHADER_STAGE_FRAGMENT_BIT; 
	return createInfo;
}

VkShaderModule vk_get_shader_module(VkDevice device, const char* file_name)
{
	tuple_t(uint64_t, pchar_t) shader_bytes = load_binary_from_file(file_name);
	VkShaderModuleCreateInfo createInfo = {}; 
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO; 
	createInfo.codeSize = shader_bytes.value1;
	createInfo.pCode = (const uint32_t*)shader_bytes.value2; 		//convert from char* to uint32_t* 
	VkShaderModule shaderModule;
	vkCreateShaderModule(device, &createInfo, NULL, &shaderModule); 
	return shaderModule;
}

tuple_t(uint32_t, pVkImage_t) vk_get_images(VkDevice device, VkSwapchainKHR swapchain)
{
	tuple_t(uint32_t, pVkImage_t) pair; 
	vkCall(vkGetSwapchainImagesKHR(device, swapchain, &(pair.value1), NULL)); 
	pair.value2 = GC_NEWV(VkImage, pair.value1); 
	vkCall(vkGetSwapchainImagesKHR(device, swapchain, &(pair.value1), pair.value2)); 
	return pair;
}

tuple_t(uint32_t, pVkImageView_t) vk_get_image_views(VkDevice device, VkFormat format, uint32_t imageCount, VkImage* images)
{
	tuple_t(uint32_t, pVkImageView_t) imageViews  =  { imageCount, GC_NEWV(VkImageView, imageCount) }; 
	for(uint32_t i = 0; i < imageCount; i++)
	{
		VkImageViewCreateInfo createInfo =  { };
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO; 
		createInfo.image = images[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = format;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY; 
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT; 
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;
		vkCall(vkCreateImageView(device, &createInfo, NULL, &(imageViews.value2[i])));
	}
	return imageViews;
}

uint32_t vk_get_graphics_queue_family_index(VkPhysicalDevice physicalDevice)
{
	tuple_t(uint32_t, pVkQueueFamilyProperties_t) queueFamilyProperites = vk_get_queue_family_properties(physicalDevice);
	for(uint32_t i = 0; i < queueFamilyProperites.value1; i++)
		if(queueFamilyProperites.value2[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			return i;
	return UINT32_MAX;
}

VkDevice vk_get_device(VkPhysicalDevice physicalDevice)
{
	VkPhysicalDeviceFeatures features =  { };

	float priority = 1.0f;
	VkDeviceQueueCreateInfo graphicsQueueCreateInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		.queueFamilyIndex = vk_get_graphics_queue_family_index(physicalDevice),
		.queueCount = 1, 
		.pQueuePriorities = &priority
	};

EXCEPTION_BLOCK
(
	if(graphicsQueueCreateInfo.queueFamilyIndex == UINT32_MAX)
		throw_exception(VULKAN_GRAPHICS_QUEUE_NOT_FOUND);
)

	VkDeviceCreateInfo deviceCreateInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.pEnabledFeatures = &features,
		.queueCreateInfoCount = 1,
		.pQueueCreateInfos = &graphicsQueueCreateInfo,
		.enabledLayerCount = 0, 
		.ppEnabledLayerNames = NULL, 
		.enabledExtensionCount = 1,
		.ppEnabledExtensionNames = string_array(1, "VK_KHR_swapchain")
	} ;
	VkDevice device;
	vkCall(vkCreateDevice(physicalDevice, &deviceCreateInfo, NULL, &device)); 
	return device;
}

VkPhysicalDevice vk_get_suitable_physical_device(tuple_t(uint32_t, pVkPhysicalDevice_t) physical_devices)
{
	EXCEPTION_BLOCK(
		if(physical_devices.value1 <= 0)
			throw_exception(VULKAN_DEVICE_NOT_FOUND);
	);
	return physical_devices.value2[0];
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


bool vk_check_physical_device_extension_support(VkPhysicalDevice device, tuple_t(uint32_t, ppVkChar_t) extensions)
{
	tuple_t(uint32_t, pVkExtensionProperties_t) extension_properties = vk_get_physical_device_extension_properties(device); 
	for (uint32_t i = 0; i < extensions.value1; ++i)
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

bool vk_check_instance_extension_support(tuple_t(uint32_t, ppVkChar_t) extensions)
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

VkInstance __vk_create_instance(uint32_t enabledExtensionCount, const char* const* enabledExtensionNames)
{
	VkInstanceCreateInfo instance_create_info =  { };
	instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO; 
	instance_create_info.pNext = NULL; 
	instance_create_info.flags = 0; 
	instance_create_info.pApplicationInfo = NULL; 
	instance_create_info.enabledLayerCount = 0;
	instance_create_info.ppEnabledLayerNames = NULL; 
	instance_create_info.enabledExtensionCount = enabledExtensionCount; 
	instance_create_info.ppEnabledExtensionNames = enabledExtensionNames; 
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

tuple_t(uint32_t, pVkExtensionProperties_t) vk_get_physical_device_extension_properties(VkPhysicalDevice physical_device)
{
	tuple_t(uint32_t, pVkExtensionProperties_t) pair; 
	vkCall(vkEnumerateDeviceExtensionProperties(physical_device, NULL, &(pair.value1), NULL)); 
	pair.value2 = GC_NEWV(VkExtensionProperties, pair.value1); 
	vkCall(vkEnumerateDeviceExtensionProperties(physical_device, NULL, &(pair.value1), pair.value2)); 
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

VkSurfaceCapabilitiesKHR vk_get_physical_device_surface_capabilities(VkPhysicalDevice physical_device, VkSurfaceKHR surface)
{
	VkSurfaceCapabilitiesKHR capabilities; 
	vkCall(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &capabilities));
	return capabilities;
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

tuple_t(uint32_t, pVkQueueFamilyProperties_t) vk_get_queue_family_properties(VkPhysicalDevice physical_device)
{
	tuple_t(uint32_t, pVkQueueFamilyProperties_t) pair; 
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &(pair.value1), NULL);
	pair.value2 = GC_NEWV(VkQueueFamilyProperties, pair.value1); 
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &(pair.value1), pair.value2); 
	return pair;
}

tuple_t(uint32_t, pVkSurfaceFormatKHR_t) vk_get_physical_device_surface_formats(VkPhysicalDevice physical_device, VkSurfaceKHR surface)
{
	tuple_t(uint32_t, pVkSurfaceFormatKHR_t) pair;
	vkCall(vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &(pair.value1), NULL)); 
	pair.value2 = GC_NEWV(VkSurfaceFormatKHR, pair.value1); 
	vkCall(vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &(pair.value1), pair.value2)); 
	return pair;
}

tuple_t(uint32_t, pVkPresentModeKHR_t) vk_get_physical_device_surface_present_modes(VkPhysicalDevice physical_device, VkSurfaceKHR surface)
{
	tuple_t(uint32_t, pVkPresentModeKHR_t) pair; 
	vkCall(vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &(pair.value1), NULL)); 
	pair.value2 = GC_NEWV(VkPresentModeKHR, pair.value1); 
	vkCall(vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &(pair.value1), pair.value2)); 
	return pair;
}

void vk_setup_validation_layers()
{
	EXCEPTION_BLOCK
	(
		if(!vk_check_layer_support((tuple_t(uint32_t, ppVkChar_t)){ 1, string_array(1, "VK_LAYER_KHRONOS_validation") }))
			throw_exception(VULKAN_LAYER_NOT_SUPPORTED);

		if(!vk_check_instance_extension_support((tuple_t(uint32_t, ppVkChar_t)) { 1, string_array(1, "My Extension") } ))
			throw_exception(VULKAN_EXTENSION_NOT_SUPPORTED);
	);
}


void vk_dump_physical_devices(tuple_t(uint32_t, pVkPhysicalDevice_t)* physical_devices)
{
	for(uint32_t i = 0; i < physical_devices->value1; i++)
	{
		VkPhysicalDevice device = physical_devices->value2[i];
		VkPhysicalDeviceProperties properties = vk_get_physical_device_properties(device);
		VkPhysicalDeviceMemoryProperties memory_properties = vk_get_physical_device_memory_properties(device);
		VkPhysicalDeviceFeatures features = vk_get_physical_device_features(device); 

		puts("Physical Device Properties----------------------");
		puts(vk_physical_device_properties_to_string(&properties));
		puts("Physical Device Memory Properties---------------");
		puts(vk_physical_device_memory_properties_to_string(&memory_properties));
		puts("Physical Device Features------------------------"); 
		puts(vk_physical_device_features_to_string(&features));
	}
}

void vk_dump_queue_families(tuple_t(uint32_t, pVkQueueFamilyProperties_t)* queue_families)
{
	puts("Physical Device Queue Family::QueueFlags---------");
	for(uint32_t i = 0; i < queue_families->value1; i++)
		puts(vk_physical_device_queue_family_to_string(queue_families->value2[i]));
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

void vk_dump_physical_device_extensions(VkPhysicalDevice* physicalDevice)
{
	log_msg("Physical Device Extensions-----------------------");
	tuple_t(uint32_t, pVkExtensionProperties_t) extensions = vk_get_physical_device_extension_properties(*physicalDevice);
	for(uint32_t i = 0; i < extensions.value1; i++)
		log_msg(extensions.value2[i].extensionName);
}


const char* vk_physical_device_queue_family_to_string(VkQueueFamilyProperties properties)
{
	char* buffer = GC_NEWV(char, 1024);		//1KB
	sprintf(buffer, "Queue Count: %u, ", properties.queueCount);
	strcat(buffer, "Queue Flags: ");
	if(properties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		strcat(buffer, "[VK_QUEUE_GRAPHICS_BIT]"); 
	if(properties.queueFlags & VK_QUEUE_COMPUTE_BIT)
		strcat(buffer, "[VK_QUEUE_COMPUTE_BIT]"); 
	if(properties.queueFlags & VK_QUEUE_TRANSFER_BIT)
		strcat(buffer, "[VK_QUEUE_TRANSFER_BIT]");
	strcat(buffer, ", ");
	VkExtent3D extents = properties.minImageTransferGranularity;
	sprintf(buffer + strlen(buffer), "minImageTransferGranularity: (%u, %u, %u)", extents.width, extents.height, extents.depth);
	return buffer;
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

const char* vk_physical_device_features_to_string(VkPhysicalDeviceFeatures* device_features)
{
	//2KB of memory
	return string(2048,
	"robustBufferAccess = %s\n"
    "fullDrawIndexUint32 = %s\n"
    "imageCubeArray = %s\n"
    "independentBlend = %s\n"
    "geometryShader = %s\n"
    "tessellationShader = %s\n"
    "sampleRateShading = %s\n"
    "dualSrcBlend = %s\n"
    "logicOp = %s\n"
    "multiDrawIndirect = %s\n"
    "drawIndirectFirstInstance = %s\n"
    "depthClamp = %s\n"
    "depthBiasClamp = %s\n"
    "fillModeNonSolid = %s\n"
    "depthBounds = %s\n"
    "wideLines = %s\n"
    "largePoints = %s\n"
    "alphaToOne = %s\n"
    "multiViewport = %s\n"
    "samplerAnisotropy = %s\n"
    "textureCompressionETC2 = %s\n"
    "textureCompressionASTC_LDR = %s\n"
    "textureCompressionBC = %s\n"
    "occlusionQueryPrecise = %s\n"
    "pipelineStatisticsQuery = %s\n"
    "vertexPipelineStoresAndAtomics = %s\n"
    "fragmentStoresAndAtomics = %s\n"
    "shaderTessellationAndGeometryPointSize = %s\n"
    "shaderImageGatherExtended = %s\n"
    "shaderStorageImageExtendedFormats = %s\n"
    "shaderStorageImageMultisample = %s\n"
    "shaderStorageImageReadWithoutFormat = %s\n"
    "shaderStorageImageWriteWithoutFormat = %s\n"
    "shaderUniformBufferArrayDynamicIndexing = %s\n"
    "shaderSampledImageArrayDynamicIndexing = %s\n"
    "shaderStorageBufferArrayDynamicIndexing = %s\n"
    "shaderStorageImageArrayDynamicIndexing = %s\n"
    "shaderClipDistance = %s\n"
    "shaderCullDistance = %s\n"
    "shaderFloat64 = %s\n"
    "shaderInt64 = %s\n"
    "shaderInt16 = %s\n"
    "shaderResourceResidency = %s\n"
    "shaderResourceMinLod = %s\n"
    "sparseBinding = %s\n"
    "sparseResidencyBuffer = %s\n"
    "sparseResidencyImage2D = %s\n"
    "sparseResidencyImage3D = %s\n"
    "sparseResidency2Samples = %s\n"
    "sparseResidency4Samples = %s\n"
    "sparseResidency8Samples = %s\n"
    "sparseResidency16Samples = %s\n"
    "sparseResidencyAliased = %s\n"
    "variableMultisampleRate = %s\n"
    "inheritedQueries = %s\n"
	,string_bool(device_features->robustBufferAccess)
    ,string_bool(device_features->fullDrawIndexUint32)
    ,string_bool(device_features->imageCubeArray)
    ,string_bool(device_features->independentBlend)
    ,string_bool(device_features->geometryShader)
    ,string_bool(device_features->tessellationShader)
    ,string_bool(device_features->sampleRateShading)
    ,string_bool(device_features->dualSrcBlend)
    ,string_bool(device_features->logicOp)
    ,string_bool(device_features->multiDrawIndirect)
    ,string_bool(device_features->drawIndirectFirstInstance)
    ,string_bool(device_features->depthClamp)
    ,string_bool(device_features->depthBiasClamp)
    ,string_bool(device_features->fillModeNonSolid)
    ,string_bool(device_features->depthBounds)
    ,string_bool(device_features->wideLines)
    ,string_bool(device_features->largePoints)
    ,string_bool(device_features->alphaToOne)
    ,string_bool(device_features->multiViewport)
    ,string_bool(device_features->samplerAnisotropy)
    ,string_bool(device_features->textureCompressionETC2)
    ,string_bool(device_features->textureCompressionASTC_LDR)
    ,string_bool(device_features->textureCompressionBC)
    ,string_bool(device_features->occlusionQueryPrecise)
    ,string_bool(device_features->pipelineStatisticsQuery)
    ,string_bool(device_features->vertexPipelineStoresAndAtomics)
    ,string_bool(device_features->fragmentStoresAndAtomics)
    ,string_bool(device_features->shaderTessellationAndGeometryPointSize)
    ,string_bool(device_features->shaderImageGatherExtended)
    ,string_bool(device_features->shaderStorageImageExtendedFormats)
    ,string_bool(device_features->shaderStorageImageMultisample)
    ,string_bool(device_features->shaderStorageImageReadWithoutFormat)
    ,string_bool(device_features->shaderStorageImageWriteWithoutFormat)
    ,string_bool(device_features->shaderUniformBufferArrayDynamicIndexing)
    ,string_bool(device_features->shaderSampledImageArrayDynamicIndexing)
    ,string_bool(device_features->shaderStorageBufferArrayDynamicIndexing)
    ,string_bool(device_features->shaderStorageImageArrayDynamicIndexing)
    ,string_bool(device_features->shaderClipDistance)
    ,string_bool(device_features->shaderCullDistance)
    ,string_bool(device_features->shaderFloat64)
    ,string_bool(device_features->shaderInt64)
    ,string_bool(device_features->shaderInt16)
    ,string_bool(device_features->shaderResourceResidency)
    ,string_bool(device_features->shaderResourceMinLod)
    ,string_bool(device_features->sparseBinding)
    ,string_bool(device_features->sparseResidencyBuffer)
    ,string_bool(device_features->sparseResidencyImage2D)
    ,string_bool(device_features->sparseResidencyImage3D)
    ,string_bool(device_features->sparseResidency2Samples)
    ,string_bool(device_features->sparseResidency4Samples)
    ,string_bool(device_features->sparseResidency8Samples)
    ,string_bool(device_features->sparseResidency16Samples)
    ,string_bool(device_features->sparseResidencyAliased)
    ,string_bool(device_features->variableMultisampleRate)
    ,string_bool(device_features->inheritedQueries));
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