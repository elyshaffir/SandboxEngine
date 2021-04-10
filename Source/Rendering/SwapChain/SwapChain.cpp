#include <Rendering/SwapChain/SwapChain.h>

#include <stdexcept>

static uint32_t GetDeviceMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter,
									VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memoryProperties = { };
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);
	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}

	throw std::runtime_error("Failed to find suitable memory type");
}

static void CreateImageWithInfo(VkDevice device, VkPhysicalDevice physicalDevice,
								const VkImageCreateInfo & imageInfo, VkMemoryPropertyFlags properties, VkImage & image,
								VkDeviceMemory & imageMemory)
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
	allocInfo.memoryTypeIndex = GetDeviceMemoryType(physicalDevice, memoryRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocate image memory");
	}

	if (vkBindImageMemory(device, image, imageMemory, 0) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to bind image memory");
	}
}

sandbox::SwapChain::SwapChain(const SwapChainSupport & supportDetails, const QueueFamilyIndices & queueFamilyIndices,
							  VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface)
		: swapChain(), renderPass()
{
	Create(supportDetails, queueFamilyIndices, device, surface);
	CreateImageViews(supportDetails, device);
	CreateRenderPass(supportDetails, device);
	CreateDepthResources(supportDetails, device, physicalDevice);
	CreateFramebuffers(supportDetails, device);
	CreateSyncObjects(device);
}

VkResult sandbox::SwapChain::AcquireNextImage(VkDevice device, uint32_t currentFrame, uint32_t * imageIndex)
{
	vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());

	VkResult result = vkAcquireNextImageKHR(device, swapChain, std::numeric_limits<uint64_t>::max(),
											imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, imageIndex);

	return result;
}

VkResult sandbox::SwapChain::SubmitCommandBuffers(VkDevice device, uint32_t currentFrame,
												  const VkCommandBuffer * buffers, uint32_t * imageIndex)
{
	if (imagesInFlight[*imageIndex] != VK_NULL_HANDLE)
	{
		vkWaitForFences(device, 1, &imagesInFlight[*imageIndex], VK_TRUE, UINT64_MAX);
	}
	imagesInFlight[*imageIndex] = inFlightFences[currentFrame];

	VkSubmitInfo submitInfo = { };
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = buffers;

	VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	vkResetFences(device, 1, &inFlightFences[currentFrame]);
	if (vkQueueSubmit(device.graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS)
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

	presentInfo.pImageIndices = imageIndex;

	VkResult result = vkQueuePresentKHR(device.presentQueue, &presentInfo);

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

	return result;
}

void sandbox::SwapChain::Create(const SwapChainSupport & supportDetails, const QueueFamilyIndices & queueFamilyIndices,
								VkDevice device, VkSurfaceKHR surface)
{
	VkSwapchainCreateInfoKHR createInfo = { };
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;
	supportDetails.PopulateSwapChainCreateInfo(&createInfo);
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	queueFamilyIndices.PopulateSwapChainCreateInfo(&createInfo);

	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.clipped = VK_TRUE;

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

void sandbox::SwapChain::CreateRenderPass(const SwapChainSupport & supportDetails, VkDevice device)
{
	VkAttachmentDescription depthAttachment = { };
	supportDetails.PopulateDepthAttachment(&depthAttachment);
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef = { };
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription colorAttachment = { };
	supportDetails.PopulateColorAttachment(&colorAttachment);
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef = { };
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = { };
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;

	VkSubpassDependency dependency = { };

	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
	VkRenderPassCreateInfo renderPassInfo = { };
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create render pass");
	}
}

void sandbox::SwapChain::CreateDepthResources(const SwapChainSupport & supportDetails, VkDevice device,
											  VkPhysicalDevice physicalDevice)
{
	depthImages.resize(images.size());
	depthImageMemories.resize(images.size());
	depthImageViews.resize(images.size());

	for (uint32_t i = 0; i < depthImages.size(); i++)
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

		CreateImageWithInfo(device, physicalDevice, depthImageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImages[i],
							depthImageMemories[i]);

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

void sandbox::SwapChain::CreateFramebuffers(const SwapChainSupport & supportDetails, VkDevice device)
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
