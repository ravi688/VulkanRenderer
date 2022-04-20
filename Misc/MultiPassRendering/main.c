
#include <PlayVk.h>

int main()
{
	VkInstance instance = pvkCreateVulkanInstanceWithExtensions(2, "VK_KHR_win32_surface", "VK_KHR_surface");
	PvkWindow* window = pvkWindowCreate(800, 800, "Vulkan Multipass Rendering", false);
	VkSurfaceKHR surface = pvkWindowCreateVulkanSurface(window, instance);
	VkPhysicalDevice physicalGPU = pvkGetPhysicalDevice(instance, surface, 
														VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU, 
														VK_FORMAT_B8G8R8A8_SRGB, 
														VK_COLOR_SPACE_SRGB_NONLINEAR_KHR, 
														VK_PRESENT_MODE_MAILBOX_KHR);
	uint32_t graphicsQueueFamilyIndex = pvkFindQueueFamilyIndex(physicalGPU, VK_QUEUE_GRAPHICS_BIT);
	uint32_t presentQueueFamilyIndex = pvkFindQueueFamilyIndexWithPresentSupport(physicalGPU, surface);
	uint32_t queueFamilyIndicies[2] = { graphicsQueueFamilyIndex, presentQueueFamilyIndex };
	VkDevice logicalGPU = pvkCreateLogicalDeviceWithExtensions(instance, 
																physicalGPU,
																2, queueFamilyIndicies,
																1, VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	VkQueue graphicsQueue, presentQueue;
	vkGetDeviceQueue(logicalGPU, graphicsQueueFamilyIndex, 0, &graphicsQueue);
	vkGetDeviceQueue(logicalGPU, presentQueueFamilyIndex, 0, &presentQueue);

	VkSwapchainKHR swapchain = pvkCreateSwapchain(logicalGPU, surface, 
													800, 800, 
													VK_FORMAT_B8G8R8A8_SRGB, 
													VK_COLOR_SPACE_SRGB_NONLINEAR_KHR, 
													VK_PRESENT_MODE_MAILBOX_KHR,
													2, queueFamilyIndicies);

	VkCommandPool commandPool = pvkCreateCommandPool(logicalGPU, 0, graphicsQueueFamilyIndex);
	VkCommandBuffer* commandBuffers = pvkAllocateCommandBuffers(logicalGPU, commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

	VkSemaphore imageAvailableSemaphore = pvkCreateSemaphore(logicalGPU);
	VkSemaphore renderFinishSemaphore = pvkCreateSemaphore(logicalGPU);

	VkRenderPass renderPass = pvkCreateRenderPass(logicalGPU);
	PvkFramebuffer* framebuffers = pvkCreateFramebuffers(logicalGPU, swapchain, renderPass, VK_FORMAT_B8G8R8A8_SRGB, 800, 800);

	while(!pvkWindowShouldClose(window))
	{
		uint32_t index = 0;
		PVK_CHECK(vkAcquireNextImageKHR(logicalGPU, swapchain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &index));

		pvkBeginCommandBuffer(commandBuffers[index]);
		
		pvkBeginRenderPass(commandBuffers[index], renderPass, framebuffers[index].handle, 800, 800);



		pvkEndRenderPass(commandBuffers[index]);

		pvkEndCommandBuffer(commandBuffers[index]);

		pvkSubmit(commandBuffers[index], graphicsQueue, imageAvailableSemaphore, renderFinishSemaphore);
		pvkPresent(index, swapchain, presentQueue, renderFinishSemaphore);

		pvkWindowPollEvents(window);
	}

	pvkDestroyFramebuffers(logicalGPU, framebuffers);
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
