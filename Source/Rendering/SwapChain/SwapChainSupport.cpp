#include <Rendering/SwapChain/SwapChainSupport.h>

#include <algorithm>

#include <glog/logging.h>

sandbox::SwapChainSupport::SwapChainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
											VkExtent2D windowExtent)
		: surfaceCapabilities(), chosenSurfaceFormat(), chosenPresentMode(), chosenExtent(), imageCount(), depthFormat()
{
	Create(physicalDevice, surface);
	ChooseSurfaceFormat();
	ChoosePresentMode();
	ChooseExtent(windowExtent);
	CalculateImageCount();
	FindDepthFormat(physicalDevice);
}

bool sandbox::SwapChainSupport::IsComplete() const
{
	return !formats.empty() && !presentModes.empty();
}

void sandbox::SwapChainSupport::PopulateSwapChainCreateInfo(VkSwapchainCreateInfoKHR * swapChainCreateInfo) const
{
	swapChainCreateInfo->minImageCount = imageCount;
	swapChainCreateInfo->imageFormat = chosenSurfaceFormat.format;
	swapChainCreateInfo->imageColorSpace = chosenSurfaceFormat.colorSpace;
	swapChainCreateInfo->imageExtent = chosenExtent;
	swapChainCreateInfo->preTransform = surfaceCapabilities.currentTransform;
	swapChainCreateInfo->presentMode = chosenPresentMode;
}

void sandbox::SwapChainSupport::PopulateDepthAttachment(VkAttachmentDescription * depthAttachment) const
{
	depthAttachment->format = depthFormat;
}

void sandbox::SwapChainSupport::PopulateImageViewCreateInfo(VkImageViewCreateInfo * imageViewCreateInfo) const
{
	imageViewCreateInfo->format = chosenSurfaceFormat.format;
}

void sandbox::SwapChainSupport::PopulateColorAttachment(VkAttachmentDescription * colorAttachment) const
{
	colorAttachment->format = chosenSurfaceFormat.format;
}

void sandbox::SwapChainSupport::PopulateDepthImageCreateInfo(VkImageCreateInfo * depthImageCreateInfo) const
{
	depthImageCreateInfo->extent.width = chosenExtent.width;
	depthImageCreateInfo->extent.height = chosenExtent.height;
	depthImageCreateInfo->format = depthFormat;
}

void sandbox::SwapChainSupport::PopulateDepthImageViewCreateInfo(VkImageViewCreateInfo * depthImageViewCreateInfo) const
{
	depthImageViewCreateInfo->format = depthFormat;
}

void sandbox::SwapChainSupport::PopulateFramebufferCreateInfo(VkFramebufferCreateInfo * framebufferCreateInfo) const
{
	framebufferCreateInfo->width = chosenExtent.width;
	framebufferCreateInfo->height = chosenExtent.height;
}

void sandbox::SwapChainSupport::Create(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
{
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities);

	uint32_t formatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);

	if (formatCount != 0)
	{
		formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, formats.data());
	}

	uint32_t presentModeCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);

	if (presentModeCount != 0)
	{
		presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data());
	}
}

void sandbox::SwapChainSupport::ChooseSurfaceFormat()
{
	for (const auto & availableFormat : formats)
	{
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
			availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			chosenSurfaceFormat = availableFormat;
		}
	}

	chosenSurfaceFormat = formats[0];
}

void sandbox::SwapChainSupport::ChoosePresentMode()
{
	for (const auto & availablePresentMode : presentModes)
	{
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			LOG(INFO) << "Present mode: Mailbox";
			chosenPresentMode = availablePresentMode;
			return;
		}
	}

	LOG(INFO) << "Present mode: V-Sync";
	chosenPresentMode = VK_PRESENT_MODE_FIFO_KHR;
}

void sandbox::SwapChainSupport::ChooseExtent(VkExtent2D windowExtent)
{
	if (surfaceCapabilities.currentExtent.width != UINT32_MAX)
	{
		chosenExtent = surfaceCapabilities.currentExtent;
	}
	else
	{
		VkExtent2D actualExtent = windowExtent;
		actualExtent.width = std::max(surfaceCapabilities.minImageExtent.width,
									  std::min(surfaceCapabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(surfaceCapabilities.minImageExtent.height,
									   std::min(surfaceCapabilities.maxImageExtent.height, actualExtent.height));

		chosenExtent = actualExtent;
	}
}

void sandbox::SwapChainSupport::CalculateImageCount()
{
	imageCount = surfaceCapabilities.minImageCount + 1;
	if (surfaceCapabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.maxImageCount)
	{
		imageCount = surfaceCapabilities.maxImageCount;
	}
}

void sandbox::SwapChainSupport::FindDepthFormat(VkPhysicalDevice physicalDevice)
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
