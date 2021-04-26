#pragma once

#include <Rendering/SwapChain/SwapChainSupport.h>

#include <array>
#include <vulkan/vulkan.h>
#include <set>

namespace sandbox
{
	class Device
	{
	public:
		VkDevice device;
		VkPhysicalDevice physicalDevice;
		SwapChainSupport swapChainSupport;

		static constexpr std::array<const char *, 1> DEVICE_EXTENSIONS = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

		Device(VkInstance instance, VkSurfaceKHR surface, VkExtent2D windowExtent);

		void Destroy() const;

		void CreateLogicalDevice(const std::set<uint32_t>& queueFamilies);

	private:
		void PickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface, VkExtent2D windowExtent);
	};
}
