#ifndef SANDBOXENGINE_SWAPCHAINSUPPORT_H
#define SANDBOXENGINE_SWAPCHAINSUPPORT_H

#include <Rendering/Device/QueueFamilyIndices.h>

#include <vector>
#include <array>

#include <vulkan/vulkan.h>

namespace sandbox
{
	/*
	 * Represents the support of a certain physical device and surface duo for swap chains.
	 */
	class SwapChainSupport
	{
	public:
		SwapChainSupport() = default;

		SwapChainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkExtent2D windowExtent);

		bool IsComplete() const;

		void PopulateSwapChainCreateInfo(VkSwapchainCreateInfoKHR * swapChainCreateInfo) const;

		void PopulateDepthAttachment(VkAttachmentDescription * depthAttachment) const;

		void PopulateImageViewCreateInfo(VkImageViewCreateInfo * imageViewCreateInfo) const;

		void PopulateColorAttachment(VkAttachmentDescription * colorAttachment) const;

		void PopulateDepthImageCreateInfo(VkImageCreateInfo * depthImageCreateInfo) const;

		void PopulateDepthImageViewCreateInfo(VkImageViewCreateInfo * depthImageViewCreateInfo) const;

		void PopulateFramebufferCreateInfo(VkFramebufferCreateInfo * framebufferCreateInfo) const;

		void ResizeCommandBuffersVector(std::vector<VkCommandBuffer> & commandBuffers) const;

	private:
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
		VkSurfaceFormatKHR chosenSurfaceFormat;
		VkPresentModeKHR chosenPresentMode;
		VkExtent2D chosenExtent;
		uint32_t imageCount;
		VkFormat depthFormat;

		void Create(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

		void ChooseSurfaceFormat();

		void ChoosePresentMode();

		void ChooseExtent(VkExtent2D windowExtent);

		void CalculateImageCount();

		void FindDepthFormat(VkPhysicalDevice physicalDevice);
	};
}

#endif //SANDBOXENGINE_SWAPCHAINSUPPORT_H
