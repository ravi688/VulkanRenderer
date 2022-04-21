
#include <PlayVk.h>

static VkRenderPass pvkCreateRenderPass2(VkDevice device)
{
	/* Attachment for the sub pass 1 */
	VkAttachmentDescription colorAttachment1 = 
	{
		.format = VK_FORMAT_B8G8R8A8_SRGB,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};

	/* Attachment for the sub pass 2 */
	VkAttachmentDescription colorAttachment2 = 
	{
		.format = VK_FORMAT_B8G8R8A8_SRGB,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
	};

	VkAttachmentReference attachmentReference1 = 
	{
		.attachment = 1,
		.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};

	VkAttachmentReference inputAttachmentReference2 = 
	{
		.attachment = 1,
		.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
	};

	VkAttachmentReference attachmentReference2 = 
	{
		.attachment = 0,
		.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};

	VkSubpassDescription subpass1 = 
	{
		.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
		.colorAttachmentCount = 1,
		.pColorAttachments = &attachmentReference1
	};

	VkSubpassDescription subpass2 = 
	{
		.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
		.colorAttachmentCount = 1,
		.pColorAttachments = &attachmentReference2,
		.inputAttachmentCount = 1,
		.pInputAttachments = &inputAttachmentReference2
	};
	VkSubpassDescription* subpasses = newv(VkSubpassDescription, 2);
	subpasses[0] = subpass1;
	subpasses[1] = subpass2;

	VkAttachmentDescription attachments[2] = { colorAttachment2, colorAttachment1 };

	VkSubpassDependency* dependencies = newv(VkSubpassDependency, 3);

	// dependency 1
	dependencies[0].srcSubpass = 0;
	dependencies[0].dstSubpass = 1;
	dependencies[0].srcStageMask = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
	dependencies[0].dstStageMask = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
	dependencies[0].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependencies[0].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	VkRenderPassCreateInfo cInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.attachmentCount = 2, 
		.pAttachments = &attachments[0],
		.subpassCount = 2,
		.pSubpasses = subpasses,
		.dependencyCount = 1,
		.pDependencies = dependencies
	};
	VkRenderPass renderPass;
	PVK_CHECK(vkCreateRenderPass(device, &cInfo, NULL, &renderPass));
	delete(dependencies);
	delete(subpasses);
	return renderPass;
}

static VkDescriptorPool pvkCreateDescriptorPool(VkDevice device)
{
	VkDescriptorPoolSize poolSize = { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1 };
	VkDescriptorPoolCreateInfo cInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		.maxSets = 1,
		.poolSizeCount = 1,
		.pPoolSizes = &poolSize
	};

	VkDescriptorPool pool;
	PVK_CHECK(vkCreateDescriptorPool(device, &cInfo, NULL, &pool));
	return pool;
}

static VkDescriptorSetLayout pvkCreateDescriptorSetLayout(VkDevice device)
{
	VkDescriptorSetLayoutBinding binding = 
	{
		.binding = 0,
		.descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
		.descriptorCount = 1,
		.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
	};

	VkDescriptorSetLayoutCreateInfo cInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.bindingCount = 1,
		.pBindings = &binding
	};

	VkDescriptorSetLayout setLayout;
	PVK_CHECK(vkCreateDescriptorSetLayout(device, &cInfo, NULL, &setLayout));
	return setLayout;
}

int main()
{
	VkInstance instance = pvkCreateVulkanInstanceWithExtensions(2, "VK_KHR_win32_surface", "VK_KHR_surface");
	PvkWindow* window = pvkWindowCreate(800, 800, "Vulkan Multipass Rendering", false);
	VkSurfaceKHR surface = pvkWindowCreateVulkanSurface(window, instance);
	VkPhysicalDevice physicalGPU = pvkGetPhysicalDevice(instance, surface, 
														VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU, 
														VK_FORMAT_B8G8R8A8_SRGB, 
														VK_COLOR_SPACE_SRGB_NONLINEAR_KHR, 
														VK_PRESENT_MODE_FIFO_KHR);
	uint32_t graphicsQueueFamilyIndex = pvkFindQueueFamilyIndex(physicalGPU, VK_QUEUE_GRAPHICS_BIT);
	uint32_t presentQueueFamilyIndex = pvkFindQueueFamilyIndexWithPresentSupport(physicalGPU, surface);
	uint32_t queueFamilyIndices[2] = { graphicsQueueFamilyIndex, presentQueueFamilyIndex };
	VkDevice logicalGPU = pvkCreateLogicalDeviceWithExtensions(instance, 
																physicalGPU,
																2, queueFamilyIndices,
																1, VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	VkQueue graphicsQueue, presentQueue;
	vkGetDeviceQueue(logicalGPU, graphicsQueueFamilyIndex, 0, &graphicsQueue);
	vkGetDeviceQueue(logicalGPU, presentQueueFamilyIndex, 0, &presentQueue);

	VkSwapchainKHR swapchain = pvkCreateSwapchain(logicalGPU, surface, 
													800, 800, 
													VK_FORMAT_B8G8R8A8_SRGB, 
													VK_COLOR_SPACE_SRGB_NONLINEAR_KHR, 
													VK_PRESENT_MODE_FIFO_KHR,
													2, queueFamilyIndices);

	VkCommandPool commandPool = pvkCreateCommandPool(logicalGPU, 0, graphicsQueueFamilyIndex);
	VkCommandBuffer* commandBuffers = pvkAllocateCommandBuffers(logicalGPU, commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

	VkSemaphore imageAvailableSemaphore = pvkCreateSemaphore(logicalGPU);
	VkSemaphore renderFinishSemaphore = pvkCreateSemaphore(logicalGPU);

	/* Render Pass & Framebuffer attachments */
	VkRenderPass renderPass = pvkCreateRenderPass2(logicalGPU);
	VkImageView* swapchainImageViews = pvkCreateSwapchainImageViews(logicalGPU, swapchain, VK_FORMAT_B8G8R8A8_SRGB);
	PvkImage auxImage = pvkCreateImage(physicalGPU, logicalGPU, 
										VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
										VK_FORMAT_B8G8R8A8_SRGB, 800, 800, 
										VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT, 
										2, queueFamilyIndices);
	VkImageView auxAttachment = pvkCreateImageView(logicalGPU, auxImage.handle, VK_FORMAT_B8G8R8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
	VkImageView attachments[6] = 
	{
		swapchainImageViews[0], auxAttachment,				// framebuffer for swapchain image 0
		swapchainImageViews[1], auxAttachment,				// framebuffer for swapchain image 1
		swapchainImageViews[2], auxAttachment				// framebuffer for swapchain image 2
	};
	VkFramebuffer* framebuffers = pvkCreateFramebuffers(logicalGPU, renderPass, 800, 800, 2, attachments);

	/* Resource Descriptors */
	VkDescriptorPool descriptorPool = pvkCreateDescriptorPool(logicalGPU);
	VkDescriptorSetLayout setLayout = pvkCreateDescriptorSetLayout(logicalGPU);
	VkDescriptorSet* set = pvkAllocateDescriptorSets(logicalGPU, descriptorPool, 1, &setLayout);
	pvkWriteInputAttachmentToDescriptor(logicalGPU, set[0], 0, auxAttachment);

	/* Graphics Pipeline & Shaders */
	VkShaderModule fragmentShader = pvkCreateShaderModule(logicalGPU, "shaders/shader.frag.spv");
	VkShaderModule vertexShader = pvkCreateShaderModule(logicalGPU, "shaders/shader.vert.spv");

	VkShaderModule fragmentShaderPass2 = pvkCreateShaderModule(logicalGPU, "shaders/shader.pass2.frag.spv");
	VkShaderModule vertexShaderPass2 = pvkCreateShaderModule(logicalGPU, "shaders/shader.pass2.vert.spv");

	VkPipelineLayout pipelineLayout = pvkCreatePipelineLayout(logicalGPU, 0, NULL);
	VkPipelineLayout pipelineLayout2 = pvkCreatePipelineLayout(logicalGPU, 1, &setLayout);
	VkPipeline pipeline = pvkCreateGraphicsPipeline(logicalGPU, pipelineLayout, renderPass, 0, 800, 800, 2,
													(PvkShader) { fragmentShader, PVK_SHADER_TYPE_FRAGMENT },
													(PvkShader) { vertexShader, PVK_SHADER_TYPE_VERTEX });
	VkPipeline pipeline2 = pvkCreateGraphicsPipeline(logicalGPU, pipelineLayout2, renderPass, 1, 800, 800, 2,
													(PvkShader) { fragmentShaderPass2, PVK_SHADER_TYPE_FRAGMENT },
													(PvkShader) { vertexShaderPass2, PVK_SHADER_TYPE_VERTEX });
	PvkGeometry* boxGeometry = pvkCreateBoxGeometry(physicalGPU, logicalGPU, 2, queueFamilyIndices, 0.5f);

	/* Command buffer recording */
	for(int index = 0; index < 3; index++)
	{
		pvkBeginCommandBuffer(commandBuffers[index]);
		
		pvkBeginRenderPass(commandBuffers[index], renderPass, framebuffers[index], 800, 800);

		/* first pass */
		vkCmdBindPipeline(commandBuffers[index], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
		pvkDrawGeometry(commandBuffers[index], boxGeometry);

		vkCmdNextSubpass(commandBuffers[index], VK_SUBPASS_CONTENTS_INLINE);

		/* second pass */
		vkCmdBindPipeline(commandBuffers[index], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline2);
		vkCmdBindDescriptorSets(commandBuffers[index], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout2, 0, 1, set, 0, NULL);
		pvkDrawGeometry(commandBuffers[index], boxGeometry);

		pvkEndRenderPass(commandBuffers[index]);

		pvkEndCommandBuffer(commandBuffers[index]);
	}

	/* Rendering & Presentation */
	while(!pvkWindowShouldClose(window))
	{
		uint32_t index = 0;
		PVK_CHECK(vkAcquireNextImageKHR(logicalGPU, swapchain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &index));

		pvkSubmit(commandBuffers[index], graphicsQueue, imageAvailableSemaphore, renderFinishSemaphore);
		pvkPresent(index, swapchain, presentQueue, renderFinishSemaphore);

		pvkWindowPollEvents(window);
	}

	pvkDestroyGeometry(logicalGPU, boxGeometry);
	vkDestroyShaderModule(logicalGPU, fragmentShaderPass2, NULL);
	vkDestroyShaderModule(logicalGPU, vertexShaderPass2, NULL);
	vkDestroyPipeline(logicalGPU, pipeline2, NULL);
	vkDestroyPipeline(logicalGPU, pipeline, NULL);
	vkDestroyPipelineLayout(logicalGPU, pipelineLayout2, NULL);
	vkDestroyPipelineLayout(logicalGPU, pipelineLayout, NULL);
	vkDestroyShaderModule(logicalGPU, fragmentShader, NULL);
	vkDestroyShaderModule(logicalGPU, vertexShader, NULL);
	delete(set);
	vkDestroyDescriptorSetLayout(logicalGPU, setLayout, NULL);
	vkDestroyDescriptorPool(logicalGPU, descriptorPool, NULL);
	pvkDestroyFramebuffers(logicalGPU, framebuffers);
	delete(framebuffers);
	vkDestroyImageView(logicalGPU, auxAttachment, NULL);
	pvkDestroyImage(logicalGPU, auxImage);
	pvkDestroySwapchainImageViews(logicalGPU, swapchain, swapchainImageViews);
	vkDestroyRenderPass(logicalGPU, renderPass, NULL);
	vkDestroySemaphore(logicalGPU, imageAvailableSemaphore, NULL);
	vkDestroySemaphore(logicalGPU, renderFinishSemaphore, NULL);
	delete(commandBuffers);
	vkDestroyCommandPool(logicalGPU, commandPool, NULL);
	vkDestroySwapchainKHR(logicalGPU, swapchain, NULL);
	vkDestroyDevice(logicalGPU, NULL);
	vkDestroySurfaceKHR(instance, surface, NULL);
	pvkWindowDestroy(window);
	vkDestroyInstance(instance, NULL);
	return 0;
}
