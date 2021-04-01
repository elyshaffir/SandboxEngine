#include <Rendering/SwapChain/SwapChainSupportDetails.h>

#include <limits>
#include <algorithm>

#include <glog/logging.h>

sandbox::SwapChainSupportDetails sandbox::SwapChainSupportDetails::FromDevice(VkPhysicalDevice physicalDevice,
																			  VkSurfaceKHR surface)
{
	SwapChainSupportDetails details = { };
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.capabilities);

	uint32_t formatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);

	if (formatCount != 0)
	{
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);

	if (presentModeCount != 0)
	{
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount,
												  details.presentModes.data());
	}
	return details;
}

VkSurfaceFormatKHR sandbox::SwapChainSupportDetails::ChooseSurfaceFormat()
{
	for (const auto & availableFormat : formats)
	{
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
			availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return availableFormat;
		}
	}

	return formats[0];
}

VkPresentModeKHR sandbox::SwapChainSupportDetails::ChoosePresentMode()
{
	for (const auto & availablePresentMode : presentModes)
	{
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			LOG(INFO) << "Present mode: Mailbox";
			return availablePresentMode;
		}
	}

	// for (const auto &availablePresentMode : availablePresentModes) { // TODO resolve
	//   if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
	//     std::cout << "Present mode: Immediate" << std::endl;
	//     return availablePresentMode;
	//   }
	// }

	LOG(INFO) << "Present mode: V-Sync";
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D sandbox::SwapChainSupportDetails::ChooseExtent(VkExtent2D windowExtent) const
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return capabilities.currentExtent;
	}
	else
	{
		VkExtent2D actualExtent = windowExtent;
		actualExtent.width = std::max(capabilities.minImageExtent.width,
									  std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height,
									   std::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}
