#include <Rendering/SwapChain/SwapChainSupport.h>

#include <limits>
#include <algorithm>

#include <glog/logging.h>

sandbox::SwapChainSupport::SwapChainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
											VkExtent2D windowExtent)
		: capabilities(), chosenSurfaceFormat(), chosenPresentMode(), chosenExtent(), imageCount(), depthFormat()
{
	Create(physicalDevice, surface);
	ChooseSurfaceFormat();
	ChoosePresentMode();
	ChooseExtent(windowExtent);
	CalculateImageCount();
	FindDepthFormat(physicalDevice);
}

void sandbox::SwapChainSupport::Create(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
{
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);

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
	swapChainCreateInfo->preTransform = capabilities.currentTransform;
	swapChainCreateInfo->presentMode = chosenPresentMode;
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

	// for (const auto &availablePresentMode : availablePresentModes) { // TODO resolve
	//   if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
	//     std::cout << "Present mode: Immediate" << std::endl;
	//     return availablePresentMode;
	//   }
	// }

	LOG(INFO) << "Present mode: V-Sync";
	chosenPresentMode = VK_PRESENT_MODE_FIFO_KHR;
}

void sandbox::SwapChainSupport::ChooseExtent(VkExtent2D windowExtent)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		chosenExtent = capabilities.currentExtent;
	}
	else
	{
		VkExtent2D actualExtent = windowExtent;
		actualExtent.width = std::max(capabilities.minImageExtent.width,
									  std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height,
									   std::min(capabilities.maxImageExtent.height, actualExtent.height));

		chosenExtent = actualExtent;
	}
}

void sandbox::SwapChainSupport::CalculateImageCount()
{
	imageCount = capabilities.minImageCount + 1;
	if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
	{
		imageCount = capabilities.maxImageCount;
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
