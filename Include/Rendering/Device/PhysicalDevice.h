#pragma once

#include <array>
#include <vector>

#include <vulkan/vulkan.h>

namespace sandbox
{
	class PhysicalDevice
	{
	public:
		static constexpr std::array<const char *, 1> NEEDED_EXTENSIONS = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

		VkPhysicalDevice physicalDevice;
		VkPhysicalDeviceMemoryProperties properties;
		uint32_t graphicsQueueFamilyIndex;
		uint32_t presentQueueFamilyIndex;

		PhysicalDevice(VkInstance instance, VkSurfaceKHR surface);

		std::vector<VkSurfaceFormatKHR> GenerateSurfaceFormats(VkSurfaceKHR surface) const;

		std::vector<VkPresentModeKHR> GeneratePresentModes(VkSurfaceKHR surface) const;

	private:
		void PickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface);

		bool AreExtensionsSupported() const;

		bool AreFeaturesSupported() const;

		std::vector<VkQueueFamilyProperties> GetQueueFamilies() const;

		bool FindGraphicsFamily();

		bool FindPresentFamily(VkSurfaceKHR surface);
	};
}
