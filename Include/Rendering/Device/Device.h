#pragma once

#include <Rendering/Device/DeviceMemoryProperties.h>
#include <Rendering/SwapChain/SwapChainSupport.h>
#include <Rendering/Queue/GraphicsQueue.h>
#include <Rendering/Queue/PresentQueue.h>
#include <Rendering/SwapChain/SwapChain.h>

#include <array>
#include <set>

namespace sandbox
{
	class Device
	{
	public:
		VkDevice device;

		static constexpr std::array<const char *, 1> DEVICE_EXTENSIONS = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

		Device(VkInstance instance, VkSurfaceKHR surface, VkExtent2D windowExtent);

		void Destroy();

		void CreateLogicalDevice(const std::set<uint32_t> & queueFamilies);

		VkRenderPass GetRenderPass() const;

		void RecordRenderPass(VkPipeline pipeline, Model & model);

		void AllocateVertexBuffer(VertexBuffer & vertexBuffer);

		void DrawFrame();

	private:
		VkPhysicalDevice physicalDevice;
		DeviceMemoryProperties memoryProperties;
		SwapChainSupport swapChainSupport;
		GraphicsQueue graphicsQueue;
		PresentQueue presentQueue;
		SwapChain swapChain;

		void PickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface, VkExtent2D windowExtent);

		void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
						  VkBuffer & buffer, VkDeviceMemory & bufferMemory) const;
	};
}
