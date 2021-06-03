#pragma once

#include <Rendering/Device/PhysicalDevice.h>
#include <Rendering/SwapChain/SwapChain.h>
#include <Rendering/Model/VertexBuffer.h>

#include <array>
#include <set>

namespace sandbox
{
	class Device
	{
	public:
		VkDevice device;
		SwapChain swapChain;

		Device(VkInstance instance, VkSurfaceKHR surface, VkExtent2D windowExtent);

		void Destroy();

		void CreateLogicalDevice(const std::set<uint32_t> & queueFamilies);

		void AllocateVertexBuffer(VertexBuffer & vertexBuffer);

		bool FinalizeDrawFrame();

		void RecreateSwapChain(VkSurfaceKHR surface, VkExtent2D windowExtent);

	private:
		PhysicalDevice physicalDevice;
		VkQueue graphicsQueue;
		VkQueue presentQueue;

		void CreateBuffer(VkDeviceSize size, VkBuffer & buffer, VkDeviceMemory & bufferMemory) const;
	};
}
