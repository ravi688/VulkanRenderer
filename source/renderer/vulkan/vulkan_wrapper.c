
#include <renderer/debug.h>
#include <renderer/assert.h>
#include <vulkan/vulkan_wrapper.h>
#include <exception/exception.h>
#include <string/string.h>		//custom string library
#include <string.h>				//standard string library
#include <memory_allocator/memory_allocator.h>
#include <disk_manager/file_reader.h>
#include <bufferlib/buffer.h>

define_exception(VULKAN_ABORTED);
define_exception(VULKAN_LAYER_NOT_SUPPORTED);
define_exception(VULKAN_DEVICE_NOT_FOUND);
define_exception(VULKAN_EXTENSION_NOT_SUPPORTED);
define_exception(VULKAN_GRAPHICS_QUEUE_NOT_FOUND); 
define_exception(VULKAN_SURFACE_NOT_SUPPORTED);
define_exception(VULKAN_PHYSICAL_DEVICE_EXTENSION_NOT_SUPPORTED);
define_exception(VULKAN_UNSUPPORTED_SHADER_TYPE);
define_exception(VULKAN_UNSUPPORTED_IMAGE_TILING);

RENDERER_API function_signature(VkFormat, vk_find_supported_format, VkPhysicalDevice physical_device, const VkFormat* const formats, uint32_t format_count, VkImageTiling tiling, VkFormatFeatureFlags format_features)
{
	CALLTRACE_BEGIN();
	VkFormatProperties properties;
	for(uint32_t i = 0; i < format_count; i++)
	{
		VkFormat format = ref(VkFormat, formats, i);
		vkGetPhysicalDeviceFormatProperties(physical_device, format, &properties);
		switch(tiling)
		{
			case VK_IMAGE_TILING_LINEAR:
				if((properties.linearTilingFeatures & format_features) == format_features)
					CALLTRACE_RETURN(format);
			break;
			
			case VK_IMAGE_TILING_OPTIMAL:
				if((properties.optimalTilingFeatures & format_features) == format_features)
					CALLTRACE_RETURN(format);
			break;

			default:
				EXCEPTION_BLOCK(throw_exception(VULKAN_UNSUPPORTED_IMAGE_TILING));
			break;
		}
	}
	log_wrn("No vulkan format is found for image tiling: %u, format features: %u\n", tiling, format_features);
	CALLTRACE_RETURN(VK_FORMAT_UNDEFINED); // invalid
}

RENDERER_API function_signature(VkDescriptorSetLayout, vk_get_descriptor_set_layout, VkDevice device, VkDescriptorSetLayoutBinding* bindings, uint32_t binding_count)
{
	CALLTRACE_BEGIN();
	VkDescriptorSetLayoutCreateInfo layout_create_info =
	{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.bindingCount = binding_count,
		.pBindings = bindings
	};
	VkDescriptorSetLayout set_layout;
	vkCall(vkCreateDescriptorSetLayout(device, &layout_create_info, NULL, &set_layout));
	CALLTRACE_RETURN(set_layout);
}

/*---------- COMMAND BUFFERS ------------ */
RENDERER_API function_signature(VkCommandBuffer, vk_get_begin_single_time_command_buffer, VkDevice device, VkCommandPool command_pool)
{
	VkCommandBufferAllocateInfo alloc_info =
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandPool = command_pool,
		.commandBufferCount = 1
	};

	VkCommandBuffer command_buffer;
	vkCall(vkAllocateCommandBuffers(device, &alloc_info, &command_buffer));

	VkCommandBufferBeginInfo begin_info =
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
	};
	vkCall(vkBeginCommandBuffer(command_buffer, &begin_info));
	return command_buffer;
}

RENDERER_API function_signature(void, vk_end_single_time_command_buffer, VkDevice device, VkCommandPool command_pool, VkCommandBuffer command_buffer, VkQueue queue)
{
	vkEndCommandBuffer(command_buffer);

	VkSubmitInfo submit_info =
	{
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.commandBufferCount = 1,
		.pCommandBuffers = &command_buffer
	};

	vkQueueSubmit(queue, 1, &submit_info, VK_NULL_HANDLE);
	vkQueueWaitIdle(queue);
	vkFreeCommandBuffers(device, command_pool, 1, &command_buffer);
}
/*-----------------------------------------*/
RENDERER_API function_signature(uint32_t, vk_find_physical_device_memory_type, VkPhysicalDevice physical_device, uint32_t required_memory_type_bits, uint32_t required_memory_properties)
{
	CALLTRACE_BEGIN();
	VkPhysicalDeviceMemoryProperties memory_properties;
	vkGetPhysicalDeviceMemoryProperties(physical_device, &memory_properties);

	int32_t selected_memory_type = -1;
	for(uint32_t i = 0; i < memory_properties.memoryTypeCount; i++)
	{
		if((required_memory_type_bits & (1 << 0)) && ((memory_properties.memoryTypes[i].propertyFlags & required_memory_properties) == required_memory_properties))
		{
			selected_memory_type = (int32_t)i;
			break;
		}
	}
	EXCEPTION_BLOCK
	(
		if(selected_memory_type == -1)
			throw_exception(VULKAN_ABORTED);
	)
	CALLTRACE_RETURN((uint32_t)selected_memory_type);
}

RENDERER_API function_signature(VkDeviceMemory, vk_get_device_memory_for_image, VkDevice device, VkPhysicalDevice physical_device, VkImage image, VkMemoryPropertyFlags memory_properties)
{
	CALLTRACE_BEGIN();
	VkMemoryRequirements memory_requirements;
	vkGetImageMemoryRequirements(device, image, &memory_requirements);

	VkMemoryAllocateInfo alloc_info =
	{
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.allocationSize = memory_requirements.size,
		.memoryTypeIndex = vk_find_physical_device_memory_type(physical_device, memory_requirements.memoryTypeBits, memory_properties)
	};
	VkDeviceMemory device_memory;
	vkCall(vkAllocateMemory(device, &alloc_info, NULL, &device_memory));
	vkCall(vkBindImageMemory(device, image, device_memory, 0));
	CALLTRACE_RETURN(device_memory);
}

RENDERER_API function_signature(VkDeviceMemory, vk_get_device_memory_for_buffer, VkDevice device, VkPhysicalDevice physical_device, VkBuffer buffer, VkMemoryPropertyFlags memory_properties)
{
	CALLTRACE_BEGIN();
    VkMemoryRequirements memory_requirements;
	vkGetBufferMemoryRequirements(device, buffer, &memory_requirements);

	VkMemoryAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	alloc_info.allocationSize = memory_requirements.size;
	alloc_info.memoryTypeIndex = vk_find_physical_device_memory_type(physical_device, memory_requirements.memoryTypeBits, memory_properties);
	VkDeviceMemory device_memory;
	vkCall(vkAllocateMemory(device, &alloc_info, NULL, &device_memory));
	vkCall(vkBindBufferMemory(device, buffer, device_memory, 0));
	CALLTRACE_RETURN(device_memory);
}

RENDERER_API function_signature(VkBuffer, vk_get_buffer, VkDevice device, VkDeviceSize size, VkBufferUsageFlags usageFlags, VkSharingMode sharingMode)
{
	CALLTRACE_BEGIN();
	VkBuffer buffer;
	VkBufferCreateInfo createInfo = { }; 
	createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO; 
	createInfo.size = size; 
	createInfo.usage = usageFlags;
	createInfo.sharingMode = sharingMode;
	vkCall(vkCreateBuffer(device, &createInfo, NULL, &buffer));
	CALLTRACE_RETURN(buffer);
}

RENDERER_API function_signature(VkSemaphore, vk_get_semaphore, VkDevice device)
{
	CALLTRACE_BEGIN();
	VkSemaphore semaphore;
	VkSemaphoreCreateInfo createInfo = { }; 
	createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	vkCall(vkCreateSemaphore(device, &createInfo, NULL, &semaphore)); 
	CALLTRACE_RETURN(semaphore);
}

RENDERER_API function_signature(VkDescriptorPool, vk_get_descriptor_pool, VkDevice device)
{
	CALLTRACE_BEGIN();
	VkDescriptorPoolSize sizes[2] =
	{
		{ .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, .descriptorCount = 8 },
		{ .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, .descriptorCount = 10 }
	};
	VkDescriptorPoolCreateInfo pool_create_info =
	{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		.poolSizeCount = 2,
		.pPoolSizes = &sizes[0],
		.maxSets = 6,
		.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT
	};
	VkDescriptorPool descriptor_pool;
	vkCall(vkCreateDescriptorPool(device, &pool_create_info, NULL, &descriptor_pool));
	CALLTRACE_RETURN(descriptor_pool);
}

RENDERER_API function_signature(VkCommandPool, vk_get_command_pool, VkDevice device, uint32_t queueFamilyIndex)
{
	CALLTRACE_BEGIN();
	VkCommandPoolCreateInfo createInfo = { };
	createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	createInfo.queueFamilyIndex = queueFamilyIndex;
	createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	VkCommandPool commandPool;
	vkCall(vkCreateCommandPool(device, &createInfo, NULL, &commandPool));
	CALLTRACE_RETURN(commandPool);
}

RENDERER_API function_signature(tuple_t(uint32_t,  pVkCommandBuffer_t), vk_get_command_buffers, VkDevice device, VkCommandPool commandPool, uint32_t count)
{
	CALLTRACE_BEGIN();
	tuple_t(uint32_t, pVkCommandBuffer_t) commandBuffers = { count, heap_newv(VkCommandBuffer, count) };
	VkCommandBufferAllocateInfo allocInfo = { }; 
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO; 
	allocInfo.commandPool = commandPool; 
	allocInfo.commandBufferCount = count;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	vkCall(vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.value2));
	CALLTRACE_RETURN(commandBuffers);
}

RENDERER_API function_signature(void, vk_get_framebuffers_out, VkDevice device, VkRenderPass render_pass, VkExtent2D extent, uint32_t layer, VkImageView** attachments_list, uint32_t* attachments_count,  uint32_t count, VkFramebuffer* out_framebuffers)
{
	CALLTRACE_BEGIN();
	for(uint32_t i = 0; i < count; i++)
	{
		VkFramebufferCreateInfo createInfo = { };
		createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		createInfo.renderPass = render_pass;
		createInfo.pAttachments = attachments_list[i];
		createInfo.attachmentCount = attachments_count[i];
		createInfo.width = extent.width;
		createInfo.height = extent.height;
		createInfo.layers = layer;
		VkFramebuffer framebuffer;
		vkCall(vkCreateFramebuffer(device, &createInfo, NULL, &framebuffer));
		ref(VkFramebuffer, out_framebuffers, i) = framebuffer;
	}
	CALLTRACE_END();
}

RENDERER_API function_signature(tuple_t(uint32_t,  pVkFramebuffer_t), vk_get_framebuffers, VkDevice device, VkRenderPass render_pass, VkExtent2D extent, uint32_t layer, VkImageView** attachments_list, uint32_t* attachments_count, uint32_t count)
{
	CALLTRACE_BEGIN();
	tuple_t(uint32_t, pVkFramebuffer_t) framebuffers = { count, heap_newv(VkFramebuffer, count) };
	vk_get_framebuffers_out(device, render_pass, extent, layer, attachments_list, attachments_count, count, framebuffers.value2);
	CALLTRACE_RETURN(framebuffers);
}

RENDERER_API function_signature(VkQueue, vk_get_device_queue, VkDevice device, uint32_t queueFamilyIndex, uint32_t queueIndex)
{
	CALLTRACE_BEGIN();
	VkQueue queue; 
	vkGetDeviceQueue(device, queueFamilyIndex, queueIndex, &queue); 
	CALLTRACE_RETURN(queue);
}

RENDERER_API function_signature(VkSwapchainKHR, vk_get_swapchain, VkDevice device, VkSwapchainCreateInfoKHR* createInfo)
{
	CALLTRACE_BEGIN();
	VkSwapchainKHR swapchain;
	vkCall(vkCreateSwapchainKHR(device, createInfo, NULL, &swapchain)); 
	CALLTRACE_RETURN(swapchain);
}

RENDERER_API function_signature(VkPipeline, vk_get_graphics_pipeline, VkDevice device, VkPipelineLayout pipelineLayout, VkRenderPass renderPass,
											VkPipelineShaderStageCreateInfo* shaderStages,
											VkPipelineVertexInputStateCreateInfo* vertexInputState, 
											VkPipelineInputAssemblyStateCreateInfo* inputAssemblyState,
											VkPipelineViewportStateCreateInfo* viewportState, 
											VkPipelineRasterizationStateCreateInfo* rasterizationState,
											VkPipelineMultisampleStateCreateInfo* multisampleState, 
											VkPipelineColorBlendStateCreateInfo* colorBlendState, 
											VkPipelineDepthStencilStateCreateInfo* depthStencilState)
{
	CALLTRACE_BEGIN();
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
	pipelineInfo.pDepthStencilState = depthStencilState;
	pipelineInfo.pColorBlendState = colorBlendState;
	pipelineInfo.pDynamicState = NULL; // Optional
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
	pipelineInfo.basePipelineIndex = -1; // Optional

	VkPipeline graphicsPipeline;
	vkCall(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &graphicsPipeline));
	CALLTRACE_RETURN(graphicsPipeline);
}

RENDERER_API function_signature(VkRenderPass, vk_get_render_pass, VkDevice device, VkFormat format, VkFormat depth_format)
{
	CALLTRACE_BEGIN();
	VkAttachmentDescription color_attachment = 
	{
		.format = format,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
	};
	VkAttachmentDescription depth_attachment = 
	{
		.format = depth_format,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	};
	VkAttachmentReference color_attachment_reference =
	{
		.attachment = 0,
		.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};
	VkAttachmentReference depth_attachment_reference = 
	{
		.attachment = 1,
		.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	};
	VkSubpassDescription subpass  = vk_get_subpass_description(&color_attachment_reference, 1, &depth_attachment_reference);
	VkAttachmentDescription attachments[2] = { color_attachment, depth_attachment };
	VkSubpassDependency dependency = vk_get_subpass_dependency();
	VkRenderPass renderPass;
 	VkRenderPassCreateInfo create_info =
 	{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.attachmentCount = 2,
		.pAttachments = &attachments[0],
		.subpassCount = 1,
		.pSubpasses = &subpass,
		.dependencyCount = 1,
		.pDependencies = &dependency
	};
	vkCall(vkCreateRenderPass(device, &create_info, NULL, &renderPass));
	CALLTRACE_RETURN(renderPass);
}

RENDERER_API function_signature_void(VkSubpassDependency, vk_get_subpass_dependency)
{
	CALLTRACE_BEGIN();
	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	CALLTRACE_RETURN(dependency);
}

RENDERER_API function_signature(VkSubpassDescription, vk_get_subpass_description, VkAttachmentReference* color_attachments, uint32_t color_attachment_count, VkAttachmentReference* depth_stencil_attachment)
{
	CALLTRACE_BEGIN();
	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = color_attachment_count;
	subpass.pColorAttachments = color_attachments;
	subpass.pDepthStencilAttachment = depth_stencil_attachment;
	CALLTRACE_RETURN(subpass);
}

RENDERER_API function_signature(VkPipelineLayout, vk_get_pipeline_layout, VkDevice device, uint32_t set_layout_count, VkDescriptorSetLayout* set_layouts, uint32_t push_constant_range_count, VkPushConstantRange* push_constant_ranges)
{
	CALLTRACE_BEGIN();
	VkPipelineLayoutCreateInfo pipeline_layout_info =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.setLayoutCount = set_layout_count,
		.pSetLayouts = set_layouts,
		.pushConstantRangeCount = push_constant_range_count,
		.pPushConstantRanges = refp(VkPushConstantRange, push_constant_ranges, 0)
	};
	VkPipelineLayout pipeline_layout;
	vkCall(vkCreatePipelineLayout(device, &pipeline_layout_info, NULL, &pipeline_layout));
	CALLTRACE_RETURN(pipeline_layout);
}

RENDERER_API function_signature_void(VkPipelineDynamicStateCreateInfo, vk_get_pipeline_dynamic_state_create_info)
{
	CALLTRACE_BEGIN();
	VkDynamicState* dynamicStates = heap_newv(VkDynamicState, 2);
	ref(VkDynamicState, dynamicStates, 0) = VK_DYNAMIC_STATE_VIEWPORT;
	ref(VkDynamicState, dynamicStates, 1) = VK_DYNAMIC_STATE_LINE_WIDTH;

	VkPipelineDynamicStateCreateInfo createInfo =  { }; 
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	createInfo.dynamicStateCount = 1; 
	createInfo.pDynamicStates = dynamicStates;
	CALLTRACE_RETURN(createInfo);
}

RENDERER_API function_signature_void(VkPipelineColorBlendStateCreateInfo, vk_get_pipeline_color_blend_state_create_info)
{
	CALLTRACE_BEGIN();
	VkPipelineColorBlendStateCreateInfo createInfo = { };
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	createInfo.logicOpEnable = VK_FALSE;
	createInfo.logicOp = VK_LOGIC_OP_COPY; // Optional
	createInfo.attachmentCount = 1;
	VkPipelineColorBlendAttachmentState* state = heap_new(VkPipelineColorBlendAttachmentState); 
	*state = vk_get_pipeline_color_blend_attachment_state();
	createInfo.pAttachments = state;
	createInfo.blendConstants[0] = 0.0f; // Optional
	createInfo.blendConstants[1] = 0.0f; // Optional
	createInfo.blendConstants[2] = 0.0f; // Optional
	createInfo.blendConstants[3] = 0.0f; // Optional
	CALLTRACE_RETURN(createInfo);
}

RENDERER_API function_signature_void(VkPipelineDepthStencilStateCreateInfo, vk_get_pipeline_depth_stencil_state_create_info)
{
	CALLTRACE_BEGIN();
	VkPipelineDepthStencilStateCreateInfo create_info = 
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
		.depthTestEnable = VK_TRUE,
		.depthWriteEnable = VK_TRUE,
		.depthCompareOp = VK_COMPARE_OP_LESS,
		.depthBoundsTestEnable = VK_FALSE,
		.minDepthBounds = 0.0f, 	// optional
		.maxDepthBounds = 1.0f,		// optional
		.stencilTestEnable = VK_FALSE,
		.front = (VkStencilOpState) { }, // optional
		.back = (VkStencilOpState) { } 	 // optional
	};
	CALLTRACE_RETURN(create_info);
}

RENDERER_API function_signature_void(VkPipelineColorBlendAttachmentState, vk_get_pipeline_color_blend_attachment_state)
{
	CALLTRACE_BEGIN();
	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional	
	CALLTRACE_RETURN(colorBlendAttachment);
}

RENDERER_API function_signature_void(VkPipelineMultisampleStateCreateInfo, vk_get_pipeline_multisample_state_create_info)
{
	CALLTRACE_BEGIN();
	VkPipelineMultisampleStateCreateInfo createInfo =  { };
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	createInfo.sampleShadingEnable = VK_FALSE;
	createInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	createInfo.minSampleShading = 1.0f; // Optional
	createInfo.pSampleMask = NULL; // Optional
	createInfo.alphaToCoverageEnable = VK_FALSE; // Optional
	createInfo.alphaToOneEnable = VK_FALSE; // Optional
	CALLTRACE_RETURN(createInfo);
}

RENDERER_API function_signature_void(VkPipelineRasterizationStateCreateInfo, vk_get_pipeline_rasterization_state_create_info)
{
	CALLTRACE_BEGIN();
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
	CALLTRACE_RETURN(createInfo);
}

RENDERER_API function_signature(VkPipelineViewportStateCreateInfo, vk_get_pipeline_viewport_state_create_info, uint32_t viewportWidth, uint32_t viewportHeight)
{
	CALLTRACE_BEGIN();
	VkPipelineViewportStateCreateInfo createInfo =  { }; 
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	createInfo.viewportCount = 1;
	VkViewport* viewport = heap_new(VkViewport);
	ref(VkViewport, viewport, 0) = vk_get_viewport(viewportWidth, viewportHeight);
	VkRect2D* scissor = heap_new(VkRect2D);
	ref(VkRect2D, scissor, 0) =  (VkRect2D) { .offset =  { 0, 0 }, .extent =  { viewportWidth, viewportHeight } };
	createInfo.pViewports = viewport;
	createInfo.scissorCount = 1; 
	createInfo.pScissors = scissor;
	CALLTRACE_RETURN(createInfo);
}

RENDERER_API function_signature_void(VkPipelineInputAssemblyStateCreateInfo, vk_get_pipeline_input_assembly_state_create_info)
{
	CALLTRACE_BEGIN();
	VkPipelineInputAssemblyStateCreateInfo createInfo  = {  }; 
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	createInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; 
	createInfo.primitiveRestartEnable = VK_FALSE;
	CALLTRACE_RETURN(createInfo);
}

RENDERER_API function_signature(VkViewport, vk_get_viewport, uint32_t width, uint32_t height)
{
	CALLTRACE_BEGIN();
	VkViewport viewport = { }; 
	viewport.x = 0; 
	viewport.y = 0; 
	viewport.width = width; 
	viewport.height = height; 
	viewport.minDepth = 0.0f; 
	viewport.maxDepth = 1.0f;
	CALLTRACE_RETURN(viewport);
}

RENDERER_API function_signature(tuple_t(uint32_t,  pVkVertexInputBindingDescription_t), vk_get_vertex_input_binding_descriptions, uint32_t binding_count, uint32_t* strides, VkVertexInputRate* input_rates)
{
	CALLTRACE_BEGIN();
	//TODO: heap_newv() is used here, it must be freed using heap_free()
	tuple_t(uint32_t, pVkVertexInputBindingDescription_t) bindingDescriptions =  { binding_count , heap_newv(VkVertexInputBindingDescription, binding_count) };
	for(uint32_t i = 0; i < bindingDescriptions.value1; i++)
	{
		ref(VkVertexInputBindingDescription, bindingDescriptions.value2, i).binding = i;
		ref(VkVertexInputBindingDescription, bindingDescriptions.value2, i).stride = ref(uint32_t, strides, i);
		ref(VkVertexInputBindingDescription, bindingDescriptions.value2, i).inputRate = ref(VkVertexInputRate, input_rates, i);
	}
	CALLTRACE_RETURN(bindingDescriptions);
}

//WARNING: It's an unsafe function, it doesn't checks for invalid memory access due to incomplete safe memory module implementation
RENDERER_API function_signature(tuple_t(uint32_t,  pVkVertexInputAttributeDescription_t), vk_get_vertex_input_attribute_descriptions, uint32_t binding_count, vertex_attribute_binding_info_t* attribute_infos)
{
	CALLTRACE_BEGIN();
	BUFFER attribute_descriptions = buf_create(sizeof(VkVertexInputAttributeDescription), 0, 0);
	uint32_t count = 0;
	for(uint32_t i = 0; i < binding_count; i++)
	{
		//TODO: this should be vertex_attribute_binding_info_t info = ref(vertex_attribute_binding_info_t, attribute_infos, i);
		//currently safe memory doesn't support checks for intermediate memory access apart from the base address
		vertex_attribute_binding_info_t info = attribute_infos[i];
		for(uint32_t j = 0; j < info.attribute_count; j++, count++)
		{
			VkVertexInputAttributeDescription description =
			{
				.binding = i,
				.location = count,
				//TODO: this should be .format = ref(VkFormat, info.attribute_formats, j), currently safe memory doesn't support checks for intermediate memory access apart from the base address
				.format = info.attribute_formats[j],
				//TODO: this should be .offset = ref(uint32_t, info.attribute_offsets, j), currently safe memory doesn't support checks for intermediate memory access apart from the base address
				.offset = info.attribute_offsets[j]
			};
			buf_push(&attribute_descriptions, &description);
		}
	}
	buf_fit(&attribute_descriptions);
	//TODO: We are returning the internal bytes of BUFFER, which should be freed using free() (stdlib.h) after being used.
	CALLTRACE_RETURN((tuple_t(uint32_t, pVkVertexInputAttributeDescription_t)) { attribute_descriptions.element_count, attribute_descriptions.bytes });
}

RENDERER_API function_signature(VkPipelineVertexInputStateCreateInfo, vk_get_pipeline_vertex_input_state_create_info, uint32_t binding_count, uint32_t* strides, VkVertexInputRate* input_rates, vertex_attribute_binding_info_t* attribute_infos)
{
	CALLTRACE_BEGIN();
	tuple_t(uint32_t, pVkVertexInputBindingDescription_t) vertexBindings = vk_get_vertex_input_binding_descriptions(binding_count, strides, input_rates);
	tuple_t(uint32_t, pVkVertexInputAttributeDescription_t) vertexAttributes = vk_get_vertex_input_attribute_descriptions(binding_count, attribute_infos);
	VkPipelineVertexInputStateCreateInfo createInfo =  { }; 
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO; 
	createInfo.vertexBindingDescriptionCount = vertexBindings.value1; 
	createInfo.pVertexBindingDescriptions = vertexBindings.value2; 
	createInfo.vertexAttributeDescriptionCount = vertexAttributes.value1; 
	createInfo.pVertexAttributeDescriptions = vertexAttributes.value2; 
	CALLTRACE_RETURN(createInfo);
}

RENDERER_API function_signature(VkPipelineShaderStageCreateInfo, vk_get_pipeline_shader_stage_create_info, VkShaderModule shader_module, vulkan_shader_type_t vulkan_shader_type, const char* entry_point)
{
	CALLTRACE_BEGIN();
	VkPipelineShaderStageCreateInfo createInfo = { }; 
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	createInfo.pName = entry_point; 
	createInfo.module = shader_module; 
	switch(vulkan_shader_type)
	{
		case VULKAN_SHADER_TYPE_VERTEX:
			createInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
			break;
		case VULKAN_SHADER_TYPE_FRAGMENT:
			createInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			break;
		case VULKAN_SHADER_TYPE_GEOMETRY:
			createInfo.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
			break;
		case VULKAN_SHADER_TYPE_TESSELLATION:
			createInfo.stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
			break;
		default:
			throw_exception(VULKAN_UNSUPPORTED_SHADER_TYPE);
			break;
	}
	CALLTRACE_RETURN(createInfo);
}

RENDERER_API function_signature(VkShaderModule, vk_get_shader_module, VkDevice device, void* spirv, uint32_t length)
{
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = length;
	createInfo.pCode = spirv;
	VkShaderModule shader_module;
	vkCall(vkCreateShaderModule(device, &createInfo, NULL, &shader_module));
	return shader_module;
}

RENDERER_API function_signature(VkShaderModule, vk_get_shader_module_load, VkDevice device, const char* file_name)
{
	CALLTRACE_BEGIN();
	BUFFER* shader_bytes = load_binary_from_file(file_name);
	VkShaderModuleCreateInfo createInfo = {}; 
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO; 
	createInfo.codeSize = shader_bytes->element_count;
	createInfo.pCode = (const uint32_t*)shader_bytes->bytes; 		//convert from char* to uint32_t*
	VkShaderModule shaderModule;
	vkCreateShaderModule(device, &createInfo, NULL, &shaderModule); 
	buf_free(shader_bytes);
	CALLTRACE_RETURN(shaderModule);
}

RENDERER_API function_signature(void, vk_get_images_out, VkDevice device, VkSwapchainKHR swapchain, VkImage* out_images, u32 check_image_count)
{
	CALLTRACE_BEGIN();
	u32 image_count = check_image_count;
	vkCall(vkGetSwapchainImagesKHR(device, swapchain, &check_image_count, out_images));
	ASSERT(check_image_count == image_count, "check_image_count != image_count\n");
	CALLTRACE_END();
}

RENDERER_API function_signature(tuple_t(uint32_t,  pVkImage_t), vk_get_images, VkDevice device, VkSwapchainKHR swapchain)
{
	CALLTRACE_BEGIN();
	tuple_t(uint32_t, pVkImage_t) pair; 
	vkCall(vkGetSwapchainImagesKHR(device, swapchain, &(pair.value1), NULL)); 
	pair.value2 = heap_newv(VkImage, pair.value1); 
	vkCall(vkGetSwapchainImagesKHR(device, swapchain, &(pair.value1), pair.value2)); 
	CALLTRACE_RETURN(pair);
}

RENDERER_API function_signature(void, vk_get_image_views_out, VkDevice device, VkFormat format, uint32_t imageCount, VkImage* images, VkImageView* out_image_views)
{
	CALLTRACE_BEGIN();
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
		vkCall(vkCreateImageView(device, &createInfo, NULL, &ref(VkImageView, out_image_views, i)));
	}
	CALLTRACE_END();
}

RENDERER_API function_signature(tuple_t(uint32_t,  pVkImageView_t), vk_get_image_views, VkDevice device, VkFormat format, uint32_t imageCount, VkImage* images)
{
	CALLTRACE_BEGIN();
	tuple_t(uint32_t, pVkImageView_t) imageViews  =  { imageCount, heap_newv(VkImageView, imageCount) };
	vk_get_image_views_out(device, format, imageViews.value1, images, imageViews.value2);
	CALLTRACE_RETURN(imageViews);
}

RENDERER_API function_signature(uint32_t, vk_get_graphics_queue_family_index, VkPhysicalDevice physicalDevice)
{
	CALLTRACE_BEGIN();
	tuple_t(uint32_t, pVkQueueFamilyProperties_t) queueFamilyProperites = vk_get_queue_family_properties(physicalDevice);
	for(uint32_t i = 0; i < queueFamilyProperites.value1; i++)
		if(ref(VkQueueFamilyProperties, queueFamilyProperites.value2, i).queueFlags & VK_QUEUE_GRAPHICS_BIT)
			CALLTRACE_RETURN(i);
	CALLTRACE_RETURN(UINT32_MAX);
}

RENDERER_API function_signature(VkDevice, vk_get_device, VkPhysicalDevice physicalDevice)
{
	CALLTRACE_BEGIN();
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
	CALLTRACE_RETURN(device);
}

RENDERER_API function_signature(VkPhysicalDevice, vk_get_suitable_physical_device, tuple_t(uint32_t, pVkPhysicalDevice_t) physical_devices)
{
	CALLTRACE_BEGIN();
	EXCEPTION_BLOCK(
		if(physical_devices.value1 <= 0)
			throw_exception(VULKAN_DEVICE_NOT_FOUND);
	);
	CALLTRACE_RETURN(ref(VkPhysicalDevice, physical_devices.value2, 0));
}

RENDERER_API function_signature(bool, vk_check_layer_support, tuple_t(uint32_t, ppVkChar_t) layers)
{
	CALLTRACE_BEGIN();
	tuple_t(uint32_t, pVkLayerProperties_t) layer_properties = vk_get_instance_layer_properties(); 
	for(uint32_t i = 0; i < layers.value1; i++)
	{
		bool contains = false;
		for(uint32_t j = 0; j < layer_properties.value1; j++)
		{
			if(strcmp(ref(char*, layers.value2, i), ref(VkLayerProperties, layer_properties.value2, j).layerName) == 0)
			{
				contains = true;
				break;
			}
		}
		if(!contains)
			CALLTRACE_RETURN(false);
	}
	CALLTRACE_RETURN(true);
}


RENDERER_API function_signature(bool, vk_check_physical_device_extension_support, VkPhysicalDevice device, tuple_t(uint32_t, ppVkChar_t) extensions)
{
	CALLTRACE_BEGIN();
	tuple_t(uint32_t, pVkExtensionProperties_t) extension_properties = vk_get_physical_device_extension_properties(device); 
	for (uint32_t i = 0; i < extensions.value1; ++i)
	{
		bool contains = false; 
		for(uint32_t j = 0; j < extension_properties.value1; j++)
		{
			if(strcmp(ref(char*, extensions.value2, i), ref(VkExtensionProperties, extension_properties.value2, j).extensionName) == 0)
			{
				contains = true; 
				break;
			}
		}
		if(!contains)
			CALLTRACE_RETURN(false);
	}
	CALLTRACE_RETURN(true);
}

RENDERER_API function_signature(bool, vk_check_instance_extension_support, tuple_t(uint32_t, ppVkChar_t) extensions)
{
	CALLTRACE_BEGIN();
	tuple_t(uint32_t, pVkExtensionProperties_t) extension_properties = vk_get_instance_extension_properties();
	for(uint32_t i = 0; i < extensions.value1; i++)
	{
		bool contains = false; 
		for(uint32_t j = 0; j < extension_properties.value1; j++)
		{
			if(strcmp(extensions.value2[i], ref(VkExtensionProperties, extension_properties.value2, j).extensionName) == 0)
			{
				contains = true; 
				break;
			}
		}
		if(!contains)
			CALLTRACE_RETURN(false);
	}
	CALLTRACE_RETURN(true);
}

RENDERER_API function_signature(VkInstance, __vk_create_instance, uint32_t enabledExtensionCount, const char* const* enabledExtensionNames)
{
	CALLTRACE_BEGIN();
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
	CALLTRACE_RETURN(instance);
}

RENDERER_API function_signature(tuple_t(uint32_t,  pVkPhysicalDevice_t), vk_get_physical_devices, VkInstance instance)
{
	CALLTRACE_BEGIN();
	tuple_t(uint32_t, pVkPhysicalDevice_t) pair;
	vkCall(vkEnumeratePhysicalDevices(instance, &(pair.value1), NULL)); 
	pair.value2 = (pVkPhysicalDevice_t)heap_alloc(sizeof(VkPhysicalDevice) * pair.value1);
	vkCall(vkEnumeratePhysicalDevices(instance, &(pair.value1), pair.value2));
	CALLTRACE_RETURN(pair);
}

RENDERER_API function_signature_void(tuple_t(uint32_t,  pVkExtensionProperties_t), vk_get_instance_extension_properties)
{
	CALLTRACE_BEGIN();
	tuple_t(uint32_t, pVkExtensionProperties_t) pair;
	vkCall(vkEnumerateInstanceExtensionProperties(NULL, &(pair.value1), NULL));
	pair.value2 = heap_newv(VkExtensionProperties, pair.value1);
	vkCall(vkEnumerateInstanceExtensionProperties(NULL, &(pair.value1), pair.value2));
	CALLTRACE_RETURN(pair);
}

RENDERER_API function_signature(tuple_t(uint32_t,  pVkExtensionProperties_t), vk_get_physical_device_extension_properties, VkPhysicalDevice physical_device)
{
	CALLTRACE_BEGIN();
	tuple_t(uint32_t, pVkExtensionProperties_t) pair; 
	vkCall(vkEnumerateDeviceExtensionProperties(physical_device, NULL, &(pair.value1), NULL)); 
	pair.value2 = heap_newv(VkExtensionProperties, pair.value1); 
	vkCall(vkEnumerateDeviceExtensionProperties(physical_device, NULL, &(pair.value1), pair.value2)); 
	CALLTRACE_RETURN(pair);
}

RENDERER_API function_signature_void(tuple_t(uint32_t,  pVkLayerProperties_t), vk_get_instance_layer_properties)
{
	CALLTRACE_BEGIN();
	tuple_t(uint32_t, pVkLayerProperties_t) pair; 
	vkCall(vkEnumerateInstanceLayerProperties(&(pair.value1), NULL)); 
	pair.value2 = heap_newv(VkLayerProperties, pair.value1);
	vkCall(vkEnumerateInstanceLayerProperties(&(pair.value1), pair.value2));
	CALLTRACE_RETURN(pair);
}

RENDERER_API function_signature(VkPhysicalDeviceProperties, vk_get_physical_device_properties, VkPhysicalDevice physical_device)
{
	CALLTRACE_BEGIN();
	VkPhysicalDeviceProperties properties; 
	vkGetPhysicalDeviceProperties(physical_device, &properties);
	CALLTRACE_RETURN(properties);
}

RENDERER_API function_signature(VkSurfaceCapabilitiesKHR, vk_get_physical_device_surface_capabilities, VkPhysicalDevice physical_device, VkSurfaceKHR surface)
{
	CALLTRACE_BEGIN();
	VkSurfaceCapabilitiesKHR capabilities; 
	vkCall(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &capabilities));
	CALLTRACE_RETURN(capabilities);
}

RENDERER_API function_signature(VkPhysicalDeviceFeatures, vk_get_physical_device_features, VkPhysicalDevice physical_device)
{
	CALLTRACE_BEGIN();
	VkPhysicalDeviceFeatures features;
	vkGetPhysicalDeviceFeatures(physical_device, &features); 
	CALLTRACE_RETURN(features);
}

RENDERER_API function_signature(VkPhysicalDeviceMemoryProperties, vk_get_physical_device_memory_properties, VkPhysicalDevice physical_device)
{
	CALLTRACE_BEGIN();
	VkPhysicalDeviceMemoryProperties memory_properties;
	vkGetPhysicalDeviceMemoryProperties(physical_device, &memory_properties);
	CALLTRACE_RETURN(memory_properties);
}

RENDERER_API function_signature(tuple_t(uint32_t,  pVkQueueFamilyProperties_t), vk_get_queue_family_properties, VkPhysicalDevice physical_device)
{
	CALLTRACE_BEGIN();
	tuple_t(uint32_t, pVkQueueFamilyProperties_t) pair; 
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &(pair.value1), NULL);
	pair.value2 = heap_newv(VkQueueFamilyProperties, pair.value1); 
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &(pair.value1), pair.value2); 
	CALLTRACE_RETURN(pair);
}

RENDERER_API function_signature(tuple_t(uint32_t,  pVkSurfaceFormatKHR_t), vk_get_physical_device_surface_formats, VkPhysicalDevice physical_device, VkSurfaceKHR surface)
{
	CALLTRACE_BEGIN();
	tuple_t(uint32_t, pVkSurfaceFormatKHR_t) pair;
	vkCall(vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &(pair.value1), NULL)); 
	pair.value2 = heap_newv(VkSurfaceFormatKHR, pair.value1); 
	vkCall(vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &(pair.value1), pair.value2)); 
	CALLTRACE_RETURN(pair);
}

RENDERER_API function_signature(tuple_t(uint32_t,  pVkPresentModeKHR_t), vk_get_physical_device_surface_present_modes, VkPhysicalDevice physical_device, VkSurfaceKHR surface)
{
	CALLTRACE_BEGIN();
	tuple_t(uint32_t, pVkPresentModeKHR_t) pair; 
	vkCall(vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &(pair.value1), NULL)); 
	pair.value2 = heap_newv(VkPresentModeKHR, pair.value1); 
	vkCall(vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &(pair.value1), pair.value2)); 
	CALLTRACE_RETURN(pair);
}

RENDERER_API function_signature_void(void, vk_setup_validation_layers)
{
	CALLTRACE_BEGIN();
	EXCEPTION_BLOCK
	(
		if(!vk_check_layer_support((tuple_t(uint32_t, ppVkChar_t)){ 1, string_array(1, "VK_LAYER_KHRONOS_validation") }))
			throw_exception(VULKAN_LAYER_NOT_SUPPORTED);

		if(!vk_check_instance_extension_support((tuple_t(uint32_t, ppVkChar_t)) { 1, string_array(1, "My Extension") } ))
			throw_exception(VULKAN_EXTENSION_NOT_SUPPORTED);
	);
	CALLTRACE_END();
}


// RENDERER_API function_signature(void, vk_dump_physical_devices, tuple_t(uint32_t, pVkPhysicalDevice_t)* physical_devices)
// {
// 	CALLTRACE_BEGIN();
// 	for(uint32_t i = 0; i < physical_devices->value1; i++)
// 	{
// 		VkPhysicalDevice device = ref(VkPhysicalDevice, physical_devices->value2, i);
// 		VkPhysicalDeviceProperties properties = vk_get_physical_device_properties(device);
// 		VkPhysicalDeviceMemoryProperties memory_properties = vk_get_physical_device_memory_properties(device);
// 		VkPhysicalDeviceFeatures features = vk_get_physical_device_features(device); 

// 		puts("Physical Device Properties----------------------");
// 		puts(vk_physical_device_properties_to_string(&properties));
// 		puts("Physical Device Memory Properties---------------");
// 		puts(vk_physical_device_memory_properties_to_string(&memory_properties));
// 		puts("Physical Device Features------------------------"); 
// 		puts(vk_physical_device_features_to_string(&features));
// 	}
// }

// RENDERER_API function_signature(void, vk_dump_queue_families, tuple_t(uint32_t, pVkQueueFamilyProperties_t)* queue_families)
// {
// 	CALLTRACE_BEGIN();
// 	puts("Physical Device Queue Family::QueueFlags---------");
// 	for(uint32_t i = 0; i < queue_families->value1; i++)
// 		puts(vk_physical_device_queue_family_to_string(ref(VkQueueFamilyProperties, queue_families->value2, i)));
// }

// RENDERER_API function_signature_void(void, vk_dump_instance_layers)
// {
// 	CALLTRACE_BEGIN();
// 	tuple_t(uint32_t, pVkLayerProperties_t) layer_properties = vk_get_instance_layer_properties();
// 	puts("Instance Layer Properties----------------------"); 
// 	for(uint32_t i = 0; i < layer_properties.value1; i++)
// 		puts(ref(VkLayerProperties, layer_properties.value2, i).layerName);
// }

// RENDERER_API function_signature_void(void, vk_dump_instance_extensions)
// {
// 	CALLTRACE_BEGIN();
// 	tuple_t(uint32_t, pVkExtensionProperties_t) extension_properties = vk_get_instance_extension_properties();
// 	puts("Instance Extension Properties----------------------");
// 	for(uint32_t i = 0; i < extension_properties.value1; i++)
// 		puts(ref(VkExtensionProperties, extension_properties.value2, i).extensionName);
// }

// RENDERER_API function_signature(void, vk_dump_physical_device_extensions, VkPhysicalDevice* physicalDevice)
// {
// 	CALLTRACE_BEGIN();
// 	log_msg("Physical Device Extensions-----------------------");
// 	tuple_t(uint32_t, pVkExtensionProperties_t) extensions = vk_get_physical_device_extension_properties(*physicalDevice);
// 	for(uint32_t i = 0; i < extensions.value1; i++)
// 		log_msg(ref(VkExtensionProperties, extensions.value2, i).extensionName);
// }


// RENDERER_API function_signature(const char*,  vk_physical_device_queue_family_to_string, VkQueueFamilyProperties properties)
// {
// 	CALLTRACE_BEGIN();
// 	char* buffer = heap_newv(char, 1024);		//1KB
// 	sprintf(buffer, "Queue Count: %u, ", properties.queueCount);
// 	strcat(buffer, "Queue Flags: ");
// 	if(properties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
// 		strcat(buffer, "[VK_QUEUE_GRAPHICS_BIT]"); 
// 	if(properties.queueFlags & VK_QUEUE_COMPUTE_BIT)
// 		strcat(buffer, "[VK_QUEUE_COMPUTE_BIT]"); 
// 	if(properties.queueFlags & VK_QUEUE_TRANSFER_BIT)
// 		strcat(buffer, "[VK_QUEUE_TRANSFER_BIT]");
// 	strcat(buffer, ", ");
// 	VkExtent3D extents = properties.minImageTransferGranularity;
// 	sprintf(buffer + strlen(buffer), "minImageTransferGranularity: (%u, %u, %u)", extents.width, extents.height, extents.depth);
// 	CALLTRACE_RETURN(buffer);
// }

// RENDERER_API function_signature(const char*,  vk_physical_device_memory_properties_to_string, VkPhysicalDeviceMemoryProperties* memory_properties)
// {
// 	CALLTRACE_BEGIN();
// 	CALLTRACE_RETURN(string(128, 
// 		"Memory Type Count: %u\n", 
// 		memory_properties->memoryTypeCount
// 		));
// }

// RENDERER_API function_signature(const char*,  vk_physical_device_limits_to_string, VkPhysicalDeviceLimits* device_limits)
// {
// 	CALLTRACE_BEGIN();
// 	CALLTRACE_RETURN(string(512, 
// 		"Max Image Dimension 1D [width]: %u\n"
// 		"Max Image Dimension 2D max(width, height): %u\n"
// 		"Max Image Dimension 3D max(width, height, depth): %u\n"
// 		"Max Image Dimension Cube max(width, height): %u\n"
// 		"Max Image Layers: %u\n"
// 		"Max Texel Buffer Elements: %u\n"
// 		,
// 		device_limits->maxImageDimension1D,
// 		device_limits->maxImageDimension2D, 
// 		device_limits->maxImageDimension3D, 
// 		device_limits->maxImageDimensionCube, 
// 		device_limits->maxImageArrayLayers,
// 		device_limits->maxTexelBufferElements
// 		// device_limits->maxUniformBufferRange, 
// 		// device_limits->maxStorageBufferRange, 
// 		// device_limits->maxPushConstantsSize,
// 		// device_limits->maxMemoryAllocationCount, 
// 		// device_limits->maxSamplerAllocationCount, 
// 		// device_limits->bufferImageGranularity,
// 		// device_limits->sparseAddressSpaceSize, 
// 		// device_limits->maxBoundDescriptorSets,
// 		// device_limits->maxPerStageDescriptorSampler,
// 		));
// }

// RENDERER_API function_signature(const char*,  vk_physical_device_features_to_string, VkPhysicalDeviceFeatures* device_features)
// {
// 	CALLTRACE_BEGIN();
// 	//2KB of memory
// 	CALLTRACE_RETURN(string(2048,
// 	"robustBufferAccess = %s\n"
//     "fullDrawIndexUint32 = %s\n"
//     "imageCubeArray = %s\n"
//     "independentBlend = %s\n"
//     "geometryShader = %s\n"
//     "tessellationShader = %s\n"
//     "sampleRateShading = %s\n"
//     "dualSrcBlend = %s\n"
//     "logicOp = %s\n"
//     "multiDrawIndirect = %s\n"
//     "drawIndirectFirstInstance = %s\n"
//     "depthClamp = %s\n"
//     "depthBiasClamp = %s\n"
//     "fillModeNonSolid = %s\n"
//     "depthBounds = %s\n"
//     "wideLines = %s\n"
//     "largePoints = %s\n"
//     "alphaToOne = %s\n"
//     "multiViewport = %s\n"
//     "samplerAnisotropy = %s\n"
//     "textureCompressionETC2 = %s\n"
//     "textureCompressionASTC_LDR = %s\n"
//     "textureCompressionBC = %s\n"
//     "occlusionQueryPrecise = %s\n"
//     "pipelineStatisticsQuery = %s\n"
//     "vertexPipelineStoresAndAtomics = %s\n"
//     "fragmentStoresAndAtomics = %s\n"
//     "shaderTessellationAndGeometryPointSize = %s\n"
//     "shaderImageGatherExtended = %s\n"
//     "shaderStorageImageExtendedFormats = %s\n"
//     "shaderStorageImageMultisample = %s\n"
//     "shaderStorageImageReadWithoutFormat = %s\n"
//     "shaderStorageImageWriteWithoutFormat = %s\n"
//     "shaderUniformBufferArrayDynamicIndexing = %s\n"
//     "shaderSampledImageArrayDynamicIndexing = %s\n"
//     "shaderStorageBufferArrayDynamicIndexing = %s\n"
//     "shaderStorageImageArrayDynamicIndexing = %s\n"
//     "shaderClipDistance = %s\n"
//     "shaderCullDistance = %s\n"
//     "shaderFloat64 = %s\n"
//     "shaderInt64 = %s\n"
//     "shaderInt16 = %s\n"
//     "shaderResourceResidency = %s\n"
//     "shaderResourceMinLod = %s\n"
//     "sparseBinding = %s\n"
//     "sparseResidencyBuffer = %s\n"
//     "sparseResidencyImage2D = %s\n"
//     "sparseResidencyImage3D = %s\n"
//     "sparseResidency2Samples = %s\n"
//     "sparseResidency4Samples = %s\n"
//     "sparseResidency8Samples = %s\n"
//     "sparseResidency16Samples = %s\n"
//     "sparseResidencyAliased = %s\n"
//     "variableMultisampleRate = %s\n"
//     "inheritedQueries = %s\n"
// 	,string_bool(device_features->robustBufferAccess)
//     ,string_bool(device_features->fullDrawIndexUint32)
//     ,string_bool(device_features->imageCubeArray)
//     ,string_bool(device_features->independentBlend)
//     ,string_bool(device_features->geometryShader)
//     ,string_bool(device_features->tessellationShader)
//     ,string_bool(device_features->sampleRateShading)
//     ,string_bool(device_features->dualSrcBlend)
//     ,string_bool(device_features->logicOp)
//     ,string_bool(device_features->multiDrawIndirect)
//     ,string_bool(device_features->drawIndirectFirstInstance)
//     ,string_bool(device_features->depthClamp)
//     ,string_bool(device_features->depthBiasClamp)
//     ,string_bool(device_features->fillModeNonSolid)
//     ,string_bool(device_features->depthBounds)
//     ,string_bool(device_features->wideLines)
//     ,string_bool(device_features->largePoints)
//     ,string_bool(device_features->alphaToOne)
//     ,string_bool(device_features->multiViewport)
//     ,string_bool(device_features->samplerAnisotropy)
//     ,string_bool(device_features->textureCompressionETC2)
//     ,string_bool(device_features->textureCompressionASTC_LDR)
//     ,string_bool(device_features->textureCompressionBC)
//     ,string_bool(device_features->occlusionQueryPrecise)
//     ,string_bool(device_features->pipelineStatisticsQuery)
//     ,string_bool(device_features->vertexPipelineStoresAndAtomics)
//     ,string_bool(device_features->fragmentStoresAndAtomics)
//     ,string_bool(device_features->shaderTessellationAndGeometryPointSize)
//     ,string_bool(device_features->shaderImageGatherExtended)
//     ,string_bool(device_features->shaderStorageImageExtendedFormats)
//     ,string_bool(device_features->shaderStorageImageMultisample)
//     ,string_bool(device_features->shaderStorageImageReadWithoutFormat)
//     ,string_bool(device_features->shaderStorageImageWriteWithoutFormat)
//     ,string_bool(device_features->shaderUniformBufferArrayDynamicIndexing)
//     ,string_bool(device_features->shaderSampledImageArrayDynamicIndexing)
//     ,string_bool(device_features->shaderStorageBufferArrayDynamicIndexing)
//     ,string_bool(device_features->shaderStorageImageArrayDynamicIndexing)
//     ,string_bool(device_features->shaderClipDistance)
//     ,string_bool(device_features->shaderCullDistance)
//     ,string_bool(device_features->shaderFloat64)
//     ,string_bool(device_features->shaderInt64)
//     ,string_bool(device_features->shaderInt16)
//     ,string_bool(device_features->shaderResourceResidency)
//     ,string_bool(device_features->shaderResourceMinLod)
//     ,string_bool(device_features->sparseBinding)
//     ,string_bool(device_features->sparseResidencyBuffer)
//     ,string_bool(device_features->sparseResidencyImage2D)
//     ,string_bool(device_features->sparseResidencyImage3D)
//     ,string_bool(device_features->sparseResidency2Samples)
//     ,string_bool(device_features->sparseResidency4Samples)
//     ,string_bool(device_features->sparseResidency8Samples)
//     ,string_bool(device_features->sparseResidency16Samples)
//     ,string_bool(device_features->sparseResidencyAliased)
//     ,string_bool(device_features->variableMultisampleRate)
//     ,string_bool(device_features->inheritedQueries)));
// }

// #define vk_physical_device_limits_to_string(...) define_alias_function_macro(vk_physical_device_limits_to_string, __VA_ARGS__)
// RENDERER_API function_signature(const char*,  vk_physical_device_type_to_string, VkPhysicalDeviceType *device_type)
// {
// 	CALLTRACE_BEGIN();
// 	switch(*device_type)
// 	{
// 		case VK_PHYSICAL_DEVICE_TYPE_OTHER: 
// 			CALLTRACE_RETURN("VK_PHYSICAL_DEVICE_TYPE_OTHER");
// 		case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
// 			CALLTRACE_RETURN("VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU");
// 		case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU: 
// 			CALLTRACE_RETURN("VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU");
// 		case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
// 			CALLTRACE_RETURN("VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU");
// 		case VK_PHYSICAL_DEVICE_TYPE_CPU:
// 			CALLTRACE_RETURN("VK_PHYSICAL_DEVICE_TYPE_CPU");
// 		default: 
// 			CALLTRACE_RETURN("(Unknown)");
// 	}
// }

// RENDERER_API function_signature(const char*,  vk_physical_device_properties_to_string, VkPhysicalDeviceProperties* properties)
// {
// 	CALLTRACE_BEGIN();
// 	CALLTRACE_RETURN(string(1024,
// 			"API Version: %d\n"
// 			"Driver Version: %d\n"
// 			"Vendor ID: %d\n"
// 			"Device ID: %d\n"
// 			"Device Name: %s\n"
// 			"Device Type: %s\n"
// 			"Pipeline Cache UUID: %d\n"
// 			"Device Limits: \n"
// 			"%s",
// 			properties->apiVersion,
// 			properties->driverVersion,
// 			properties->vendorID,
// 			properties->deviceID,
// 			properties->deviceName,
// 			vk_physical_device_type_to_string(&(properties->deviceType)),
// 			properties->pipelineCacheUUID,
// 			vk_physical_device_limits_to_string(&(properties->limits))));
// }
