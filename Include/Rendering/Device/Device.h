#ifndef SANDBOXENGINE_DEVICE_H
#define SANDBOXENGINE_DEVICE_H

#include <array>

#include <vulkan/vulkan.h>

namespace sandbox
{
	class Device
	{
	public:
		VkDevice device;

		static constexpr std::array<const char *, 1> DEVICE_EXTENSIONS = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

		Device(VkInstance instance, VkSurfaceKHR surface);

		~Device();

	private:
		VkPhysicalDevice physicalDevice;
		VkPhysicalDeviceProperties physicalDeviceProperties;
		VkQueue graphicsQueue;
		VkQueue presentQueue;
		VkCommandPool commandPool;

		void PickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface);

		void CreateLogicalDevice(VkSurfaceKHR surface);

		void CreateCommandPool(VkSurfaceKHR surface);
	};
}

#endif //SANDBOXENGINE_DEVICE_H
