#ifndef SANDBOXENGINE_SWAPCHAINSUPPORTDETAILS_H
#define SANDBOXENGINE_SWAPCHAINSUPPORTDETAILS_H

#include <vector>

#include <vulkan/vulkan.h>

namespace sandbox
{
	struct SwapChainSupportDetails
	{
	public:
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;

		static sandbox::SwapChainSupportDetails FromDevice(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
	};
}

#endif //SANDBOXENGINE_SWAPCHAINSUPPORTDETAILS_H
