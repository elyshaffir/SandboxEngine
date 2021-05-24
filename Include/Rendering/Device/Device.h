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
		RenderPass renderPass;

		static constexpr std::array<const char *, 1> DEVICE_EXTENSIONS = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

		Device(VkInstance instance, VkSurfaceKHR surface, VkExtent2D windowExtent);

		void Destroy();

		void CreateLogicalDevice(const std::set<uint32_t> & queueFamilies);

		void AllocateVertexBuffer(VertexBuffer & vertexBuffer);

		/*
		 * Prepares the ground for binding the graphics pipeline and drawing a frame.
		 *
		 * @param frameCommandBuffer: The command buffer to which the draw commands should be
		 * 							  submitted will be outputted here.
		 * @param imageIndex: The index of the next image in the swap chain will be outputted here.
		 *
		 * Returns true if swap chain is still valid, false otherwise.
		 */
		bool PrepareDrawFrame(VkCommandBuffer & frameCommandBuffer, uint32_t & imageIndex);

		/*
		 * Finalizes the drawing of a frame.
		 *
		 * @param frameCommandBuffer: The command buffer in which the draw commands are submitted.
		 *
		 * @param imageIndex: The index of the next image in the swap chain.
		 *
		 * Returns true if swap chain is still valid, false otherwise.
		 */
		bool FinalizeDrawFrame(VkCommandBuffer frameCommandBuffer, uint32_t imageIndex);

		void RecreateSwapChain(VkSurfaceKHR surface, VkExtent2D windowExtent);

	private:
		VkPhysicalDevice physicalDevice;
		DeviceMemoryProperties memoryProperties;
		SwapChainSupport swapChainSupport;
		GraphicsQueue graphicsQueue;
		PresentQueue presentQueue;
		SwapChain swapChain;

		void PickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface, VkExtent2D windowExtent);

		void CreateBuffer(VkDeviceSize size, VkBuffer & buffer, VkDeviceMemory & bufferMemory) const;
	};
}
