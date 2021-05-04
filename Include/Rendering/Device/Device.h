#pragma once

#include <Rendering/SwapChain/SwapChainSupport.h>
#include <Rendering/Queue/GraphicsQueue.h>
#include <Rendering/Queue/PresentQueue.h>
#include <Rendering/SwapChain/SwapChain.h>

#include <array>
#include <vulkan/vulkan.h>
#include <set>

namespace sandbox
{
	class Device
	{
	public:
		VkDevice device;
		SwapChain swapChain;

		static constexpr std::array<const char *, 1> DEVICE_EXTENSIONS = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

		Device(VkInstance instance, VkSurfaceKHR surface, VkExtent2D windowExtent);

		void Destroy();

		void CreateLogicalDevice(const std::set<uint32_t>& queueFamilies);

		void RecordRenderPass(VkPipeline pipeline);

		void DrawFrame();

	private:
		VkPhysicalDevice physicalDevice;
		SwapChainSupport swapChainSupport;
		GraphicsQueue graphicsQueue;
		PresentQueue presentQueue;

		void PickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface, VkExtent2D windowExtent);
	};
}
