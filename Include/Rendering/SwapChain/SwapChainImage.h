#pragma once

#include <vulkan/vulkan.h>

namespace sandbox
{
	class SwapChainImage
	{
	public:
		VkFramebuffer framebuffer;
		VkCommandBuffer commandBuffer;
		VkFence inFlightFence;

		SwapChainImage(VkDevice device, VkRenderPass renderPass, VkFormat depthFormat, VkExtent2D swapChainExtent,
					   VkImage swapChainImage, VkCommandBuffer commandBuffer,
					   const VkPhysicalDeviceMemoryProperties & physicalDeviceMemoryProperties,
					   const VkSurfaceFormatKHR & surfaceFormat);

		void BeginCommandBuffer() const;

		void EndCommandBuffer() const;

		void Destroy(VkDevice device, VkCommandPool commandPool) const;

	private:
		VkDeviceMemory depthImageMemory;
		VkImage depthImage;
		VkImage swapChainImage;
		VkImageView depthImageView;
		VkImageView imageView;

		void CreateImageView(VkDevice device, const VkSurfaceFormatKHR & surfaceFormat);

		void CreateDepthImage(VkDevice device, VkFormat depthFormat, VkExtent2D swapChainExtent,
							  const VkPhysicalDeviceMemoryProperties & physicalDeviceMemoryProperties);

		void CreateDepthImageView(VkDevice device, VkFormat depthFormat);

		void CreateFramebuffer(VkDevice device, VkRenderPass renderPass, VkExtent2D swapChainExtent);
	};
}
