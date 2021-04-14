#ifndef SANDBOXENGINE_DEVICE_H
#define SANDBOXENGINE_DEVICE_H

#include <Rendering/Device/QueueFamilyIndices.h>
#include <Rendering/SwapChain/SwapChainSupport.h>
#include <Rendering/SwapChain/SwapChain.h>

#include <array>

#include <vulkan/vulkan.h>

namespace sandbox
{
	class Device
	{
	public:
		VkDevice device;

		static constexpr std::array<const char *, 1> DEVICE_EXTENSIONS = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

		Device(VkInstance instance, VkSurfaceKHR surface, VkExtent2D windowExtent);

		~Device();

	private:
		VkPhysicalDevice physicalDevice;
		VkPhysicalDeviceProperties physicalDeviceProperties;
		VkQueue graphicsQueue;
		VkQueue presentQueue;
		VkCommandPool commandPool;
		std::vector<VkCommandBuffer> commandBuffers;
		QueueFamilyIndices queueFamilyIndices;
		SwapChainSupport swapChainSupport;
		SwapChain swapChain;

		void PickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface, VkExtent2D windowExtent);

		void CreateLogicalDevice();

		void CreateCommandPool();

		void CreateSwapChain(VkSurfaceKHR surface);

		void CreateCommandBuffers();
	};
}

#endif //SANDBOXENGINE_DEVICE_H
