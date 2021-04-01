#ifndef SANDBOXENGINE_SWAPCHAINSUPPORT_H
#define SANDBOXENGINE_SWAPCHAINSUPPORT_H

#include <vector>

#include <vulkan/vulkan.h>

namespace sandbox
{
	/*
	 * Represents the support of a certain physical device and surface duo for swap chains.
	 */
	struct SwapChainSupport
	{
	public:
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;

		static sandbox::SwapChainSupport FromDevice(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

		bool IsComplete() const;

		VkSurfaceFormatKHR ChooseSurfaceFormat();

		VkPresentModeKHR ChoosePresentMode();

		VkExtent2D ChooseExtent(VkExtent2D windowExtent) const;
	};
}

#endif //SANDBOXENGINE_SWAPCHAINSUPPORT_H
