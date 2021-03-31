#ifndef SANDBOXENGINE_SWAPCHAIN_H
#define SANDBOXENGINE_SWAPCHAIN_H

#include <vector>

#include <vulkan/vulkan.h>

namespace sandbox
{
	class SwapChain
	{
	private:
		VkSwapchainKHR swapChain;
		VkFormat imageFormat;
		VkExtent2D extent; // TODO: Am i needed?
		std::vector<VkFramebuffer> framebuffers;
		VkRenderPass renderPass;
		std::vector<VkImage> depthImages;
		std::vector<VkDeviceMemory> depthImageMemories;
		std::vector<VkImageView> depthImageViews;
		std::vector<VkImage> images;
		std::vector<VkImageView> imageViews;
		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;
		std::vector<VkFence> imagesInFlight;
		uint32_t currentFrame = 0; // TODO: Can I be moved to the Renderer?

		void Create(VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface, VkExtent2D windowExtent);

		void CreateImageViews(VkDevice device);

		void CreateRenderPass(VkDevice device, VkPhysicalDevice physicalDevice);

		void CreateDepthResources(VkDevice device, VkPhysicalDevice physicalDevice);
	};
}

#endif //SANDBOXENGINE_SWAPCHAIN_H
