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
		SwapChain swapChain;
		std::vector<VkCommandBuffer> commandBuffers;
		VkQueue graphicsQueue;
		VkQueue presentQueue;

		static constexpr std::array<const char *, 1> DEVICE_EXTENSIONS = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

		Device(VkInstance instance, VkSurfaceKHR surface, VkExtent2D windowExtent);

		void Destroy() const;

		void CreateCommandBuffers(VkPipeline pipeline);

	private:
		VkPhysicalDevice physicalDevice;
		VkPhysicalDeviceProperties physicalDeviceProperties;
		VkCommandPool commandPool;
		QueueFamilyIndices queueFamilyIndices;

		SwapChainSupport swapChainSupport;

		void PickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface, VkExtent2D windowExtent);

		void CreateLogicalDevice();

		void CreateCommandPool();

		void CreateSwapChain(VkSurfaceKHR surface);
	};
}

#endif //SANDBOXENGINE_DEVICE_H
