#include <Rendering/SwapChain/SwapChain.h>

#include <stdexcept>
#include <array>


sandbox::SwapChain::SwapChain(VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface,
							  VkPresentModeKHR presentMode, VkExtent2D windowExtent,
							  const std::vector<VkSurfaceFormatKHR> & availableFormats,
							  const VkPhysicalDeviceMemoryProperties & physicalDeviceMemoryProperties,
							  uint32_t graphicsFamilyIndex, uint32_t presentFamilyIndex,
							  VkSwapchainKHR oldSwapChain)
		: swapChain(), renderPass(), inFlightFrameIndex(), imageCount(), extent(), surfaceFormat(), depthFormat(),
		  imageIndex()
{
	VkSurfaceCapabilitiesKHR surfaceCapabilities = { };
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities);
	CalculateImageCount(surfaceCapabilities);
	ChooseExtent(windowExtent, surfaceCapabilities);
	ChooseSurfaceFormat(availableFormats);
	ChooseDepthFormat(physicalDevice);
	renderPass = RenderPass(device, depthFormat);
	Create(device, surface, surfaceCapabilities, presentMode, graphicsFamilyIndex, presentFamilyIndex, oldSwapChain);
	graphicsCommandPool = CommandPool(device, graphicsFamilyIndex);
	CreateImages(device, physicalDeviceMemoryProperties);
	CreateSyncObjects(device);
}

bool sandbox::SwapChain::BeginDrawFrame(VkDevice device)
{
	vkWaitForFences(device, 1, &inFlightFences[inFlightFrameIndex], VK_TRUE, UINT64_MAX);

	VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[inFlightFrameIndex],
											VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		return false;
	}

	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("Failed to acquire swap chain image");
	}
	images[imageIndex].BeginCommandBuffer();
	renderPass.Begin(images[imageIndex].framebuffer, images[imageIndex].commandBuffer, extent);

	VkViewport viewport = { };
	viewport.width = static_cast<float>(extent.width);
	viewport.height = static_cast<float>(extent.height);
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {{ }, extent};

	vkCmdSetViewport(images[imageIndex].commandBuffer, 0, 1, &viewport);
	vkCmdSetScissor(images[imageIndex].commandBuffer, 0, 1, &scissor);

	return true;
}

VkCommandBuffer sandbox::SwapChain::GetCurrentFrameCommandBuffer()
{
	return images[imageIndex].commandBuffer;
}

VkResult sandbox::SwapChain::FinalizeDrawFrame(VkDevice device, VkQueue graphicsQueue, VkQueue presentQueue)
{
	vkCmdEndRenderPass(images[imageIndex].commandBuffer);
	images[imageIndex].EndCommandBuffer();
	if (images[imageIndex].inFlightFence != VK_NULL_HANDLE)
	{
		vkWaitForFences(device, 1, &images[imageIndex].inFlightFence, VK_TRUE, UINT64_MAX);
	}
	images[imageIndex].inFlightFence = inFlightFences[inFlightFrameIndex];

	VkSubmitInfo submitInfo = { };
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[inFlightFrameIndex]};
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &images[imageIndex].commandBuffer;

	VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[inFlightFrameIndex]};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	vkResetFences(device, 1, &inFlightFences[inFlightFrameIndex]);
	if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[inFlightFrameIndex]) != VK_SUCCESS)
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

	inFlightFrameIndex = (inFlightFrameIndex + 1) % MAX_FRAMES_IN_FLIGHT;

	return result;
}

void sandbox::SwapChain::Destroy(VkDevice device, bool recycle)
{
	if (!recycle)
	{
		vkDestroySwapchainKHR(device, swapChain, nullptr);
	}

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(device, inFlightFences[i], nullptr);
	}

	for (auto image : images)
	{
		image.Destroy(device, graphicsCommandPool.commandPool);
	}

	graphicsCommandPool.Destroy(device);
	renderPass.Destroy(device);
}

void sandbox::SwapChain::Create(VkDevice device, VkSurfaceKHR surface,
								const VkSurfaceCapabilitiesKHR & surfaceCapabilities,
								VkPresentModeKHR presentMode, uint32_t graphicsFamilyIndex, uint32_t presentFamilyIndex,
								VkSwapchainKHR oldSwapChain)
{
	VkSwapchainCreateInfoKHR createInfo = { };
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.minImageCount = imageCount;
	createInfo.imageExtent = extent;
	createInfo.preTransform = surfaceCapabilities.currentTransform;
	createInfo.presentMode = presentMode;
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
}

void sandbox::SwapChain::ChooseExtent(VkExtent2D windowExtent, const VkSurfaceCapabilitiesKHR & surfaceCapabilities)
{
	if (surfaceCapabilities.currentExtent.width != UINT32_MAX)
	{
		extent = surfaceCapabilities.currentExtent;
	}
	else
	{
		VkExtent2D actualExtent = windowExtent;
		actualExtent.width = std::max(surfaceCapabilities.minImageExtent.width,
									  std::min(surfaceCapabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(surfaceCapabilities.minImageExtent.height,
									   std::min(surfaceCapabilities.maxImageExtent.height, actualExtent.height));

		extent = actualExtent;
	}
}

void sandbox::SwapChain::ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR> & availableFormats)
{
	for (const auto & availableFormat : availableFormats)
	{
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
			availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			surfaceFormat = availableFormat;
			return;
		}
	}
	surfaceFormat = availableFormats[0];
}

void sandbox::SwapChain::ChooseDepthFormat(VkPhysicalDevice physicalDevice)
{
	std::vector<VkFormat> candidates = {
			VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT
	};
	for (VkFormat format : candidates)
	{
		VkFormatProperties formatProperties = { };
		vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &formatProperties);

		if ((formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) ==
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
		{
			depthFormat = format;
			return;
		}
	}
	throw std::runtime_error("Failed to find supported format");
}

void sandbox::SwapChain::CreateImages(VkDevice device,
									  const VkPhysicalDeviceMemoryProperties & physicalDeviceMemoryProperties)
{
	std::vector<VkImage> swapChainImageHandles;
	uint32_t swapChainImageCount = 0;
	vkGetSwapchainImagesKHR(device, swapChain, &swapChainImageCount, nullptr);
	swapChainImageHandles.resize(swapChainImageCount);
	vkGetSwapchainImagesKHR(device, swapChain, &swapChainImageCount, swapChainImageHandles.data());

	std::vector<VkCommandBuffer> graphicsCommandBuffers;
	graphicsCommandPool.AllocateCommandBuffers(device, swapChainImageCount, graphicsCommandBuffers);

	for (uint32_t swapChainImageIndex = 0; swapChainImageIndex < swapChainImageCount; swapChainImageIndex++)
	{
		images.emplace_back(device, renderPass.renderPass, depthFormat, extent,
							swapChainImageHandles[swapChainImageIndex], graphicsCommandBuffers[swapChainImageIndex],
							physicalDeviceMemoryProperties, surfaceFormat);
	}
}

void sandbox::SwapChain::CalculateImageCount(const VkSurfaceCapabilitiesKHR & surfaceCapabilities)
{
	imageCount = surfaceCapabilities.minImageCount + 1;
	if (surfaceCapabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.maxImageCount)
	{
		imageCount = surfaceCapabilities.maxImageCount;
	}
}

void sandbox::SwapChain::CreateSyncObjects(VkDevice device)
{
	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

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
