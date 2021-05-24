#include <Rendering/SwapChain/SwapChain.h>

#include <stdexcept>
#include <array>

static void CreateImageWithInfo(VkDevice device, const sandbox::DeviceMemoryProperties & deviceMemoryProperties,
								const VkImageCreateInfo & imageInfo, VkImage & image, VkDeviceMemory & imageMemory)
{
	if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create image");
	}

	VkMemoryRequirements memoryRequirements = { };
	vkGetImageMemoryRequirements(device, image, &memoryRequirements);

	VkMemoryAllocateInfo allocInfo = { };
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memoryRequirements.size;
	allocInfo.memoryTypeIndex = deviceMemoryProperties.FindMemoryType(memoryRequirements.memoryTypeBits,
																	  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocate image memory");
	}

	if (vkBindImageMemory(device, image, imageMemory, 0) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to bind image memory");
	}
}

sandbox::SwapChain::SwapChain(const SwapChainSupport & supportDetails,
							  const DeviceMemoryProperties & deviceMemoryProperties,
							  VkDevice device, VkSurfaceKHR surface, uint32_t graphicsFamilyIndex,
							  uint32_t presentFamilyIndex,
							  VkRenderPass renderPass, VkSwapchainKHR oldSwapChain)
		: swapChain(), frameIndex()
{
	Create(supportDetails, device, surface, graphicsFamilyIndex, presentFamilyIndex, oldSwapChain);
	CreateImageViews(supportDetails, device);
	CreateDepthResources(supportDetails, device, deviceMemoryProperties);
	CreateFramebuffers(supportDetails, device, renderPass);
	CreateSyncObjects(device);
}

void sandbox::SwapChain::Destroy(VkDevice device, bool recycle)
{
	for (VkImageView imageView : imageViews)
	{
		vkDestroyImageView(device, imageView, nullptr);
	}

	imageViews.clear();

	if (!recycle)
	{
		vkDestroySwapchainKHR(device, swapChain, nullptr);
	}

	for (size_t i = 0; i < depthImages.size(); i++)
	{
		vkDestroyImageView(device, depthImageViews[i], nullptr);
		vkDestroyImage(device, depthImages[i], nullptr);
		vkFreeMemory(device, depthImageMemories[i], nullptr);
	}

	for (VkFramebuffer framebuffer : framebuffers)
	{
		vkDestroyFramebuffer(device, framebuffer, nullptr);
	}

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(device, inFlightFences[i], nullptr);
	}
}

VkResult sandbox::SwapChain::AcquireNextImage(VkDevice device, uint32_t * imageIndex)
{
	vkWaitForFences(device, 1, &inFlightFences[frameIndex], VK_TRUE, UINT64_MAX);

	VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[frameIndex],
											VK_NULL_HANDLE, imageIndex);

	return result;
}

VkResult sandbox::SwapChain::SubmitCommandBuffer(VkDevice device, VkCommandBuffer buffer, uint32_t imageIndex,
												 VkQueue graphicsQueue, VkQueue presentQueue)
{
	if (imagesInFlight[imageIndex] != VK_NULL_HANDLE)
	{
		vkWaitForFences(device, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
	}
	imagesInFlight[imageIndex] = inFlightFences[frameIndex];

	VkSubmitInfo submitInfo = { };
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[frameIndex]};
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &buffer;

	VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[frameIndex]};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	vkResetFences(device, 1, &inFlightFences[frameIndex]);
	if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[frameIndex]) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to submit draw command buffer");
	}

	VkPresentInfoKHR presentInfo = { };
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = {swapChain};
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;

	presentInfo.pImageIndices = &imageIndex;

	VkResult result = vkQueuePresentKHR(presentQueue, &presentInfo);

	frameIndex = (frameIndex + 1) % MAX_FRAMES_IN_FLIGHT;

	return result;
}

void sandbox::SwapChain::Create(const SwapChainSupport & supportDetails, VkDevice device, VkSurfaceKHR surface,
								uint32_t graphicsFamilyIndex, uint32_t presentFamilyIndex, VkSwapchainKHR oldSwapChain)
{
	VkSwapchainCreateInfoKHR createInfo = { };
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;
	supportDetails.PopulateSwapChainCreateInfo(&createInfo);
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	if (graphicsFamilyIndex != presentFamilyIndex)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		uint32_t queueFamilyIndices[2] = {graphicsFamilyIndex, presentFamilyIndex};
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = oldSwapChain;

	if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create swap chain");
	}

	uint32_t imageCount = 0;
	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
	images.resize(imageCount);
	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, images.data());
}

void sandbox::SwapChain::CreateImageViews(const SwapChainSupport & supportDetails, VkDevice device)
{
	imageViews.resize(images.size());
	for (size_t i = 0; i < images.size(); i++)
	{
		VkImageViewCreateInfo viewCreateInfo = { };
		viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewCreateInfo.image = images[i];
		viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		supportDetails.PopulateImageViewCreateInfo(&viewCreateInfo);
		viewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewCreateInfo.subresourceRange.levelCount = 1;
		viewCreateInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(device, &viewCreateInfo, nullptr, &imageViews[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create texture image view");
		}
	}
}

void sandbox::SwapChain::CreateDepthResources(const SwapChainSupport & supportDetails, VkDevice device,
											  const DeviceMemoryProperties & deviceMemoryProperties)
{
	depthImages.resize(images.size());
	depthImageMemories.resize(images.size());
	depthImageViews.resize(images.size());

	for (size_t i = 0; i < depthImages.size(); i++)
	{
		VkImageCreateInfo depthImageInfo = { };
		depthImageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		depthImageInfo.imageType = VK_IMAGE_TYPE_2D;
		supportDetails.PopulateDepthImageCreateInfo(&depthImageInfo);
		depthImageInfo.extent.depth = 1;
		depthImageInfo.mipLevels = 1;
		depthImageInfo.arrayLayers = 1;
		depthImageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		depthImageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthImageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		depthImageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		depthImageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		CreateImageWithInfo(device, deviceMemoryProperties, depthImageInfo, depthImages[i], depthImageMemories[i]);

		VkImageViewCreateInfo depthImageViewInfo = { };
		depthImageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		depthImageViewInfo.image = depthImages[i];
		depthImageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		supportDetails.PopulateDepthImageViewCreateInfo(&depthImageViewInfo);
		depthImageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		depthImageViewInfo.subresourceRange.levelCount = 1;
		depthImageViewInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(device, &depthImageViewInfo, nullptr, &depthImageViews[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create texture image view");
		}
	}
}

void sandbox::SwapChain::CreateFramebuffers(const SwapChainSupport & supportDetails, VkDevice device,
											VkRenderPass renderPass)
{
	framebuffers.resize(images.size());
	for (size_t i = 0; i < images.size(); i++)
	{
		std::array<VkImageView, 2> attachments = {imageViews[i], depthImageViews[i]};

		VkFramebufferCreateInfo framebufferInfo = { };
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		supportDetails.PopulateFramebufferCreateInfo(&framebufferInfo);
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffers[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create framebuffer");
		}
	}
}

void sandbox::SwapChain::CreateSyncObjects(VkDevice device)
{
	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
	imagesInFlight.resize(images.size(), VK_NULL_HANDLE);

	VkSemaphoreCreateInfo semaphoreInfo = { };
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo = { };
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create synchronization objects for a frame");
		}
	}
}
