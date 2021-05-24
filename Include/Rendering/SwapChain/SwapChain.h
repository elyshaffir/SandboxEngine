#pragma once

#include <Rendering/Device/DeviceMemoryProperties.h>
#include <Rendering/SwapChain/SwapChainSupport.h>
#include <Rendering/SwapChain/RenderPass.h>

#include <vector>

#include <vulkan/vulkan.h>

namespace sandbox
{
	class SwapChain
	{
	public:
		VkSwapchainKHR swapChain;
		std::vector<VkFramebuffer> framebuffers;

		SwapChain() = default;

		SwapChain(const SwapChainSupport & supportDetails, const DeviceMemoryProperties & deviceMemoryProperties,
				  VkDevice device, VkSurfaceKHR surface, uint32_t graphicsFamilyIndex, uint32_t presentFamilyIndex,
				  VkRenderPass renderPass, VkSwapchainKHR oldSwapChain = VK_NULL_HANDLE);

		void Destroy(VkDevice device, bool recycle);

		VkResult AcquireNextImage(VkDevice device, uint32_t * imageIndex);

		VkResult SubmitCommandBuffer(VkDevice device, VkCommandBuffer buffer, uint32_t imageIndex,
									 VkQueue graphicsQueue, VkQueue presentQueue);

	private:
		static constexpr size_t MAX_FRAMES_IN_FLIGHT = 2;

		size_t frameIndex;
		std::vector<VkImage> depthImages;
		std::vector<VkDeviceMemory> depthImageMemories;
		std::vector<VkImageView> depthImageViews;
		std::vector<VkImage> images;
		std::vector<VkImageView> imageViews;
		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;
		std::vector<VkFence> imagesInFlight;

		void Create(const SwapChainSupport & supportDetails, VkDevice device, VkSurfaceKHR surface,
					uint32_t graphicsFamilyIndex, uint32_t presentFamilyIndex,
					VkSwapchainKHR oldSwapChain = VK_NULL_HANDLE);

		void CreateImageViews(const SwapChainSupport & supportDetails, VkDevice device);

		void CreateDepthResources(const SwapChainSupport & supportDetails, VkDevice device,
								  const DeviceMemoryProperties & deviceMemoryProperties);

		void CreateFramebuffers(const SwapChainSupport & supportDetails, VkDevice device,
								VkRenderPass renderPass);

		void CreateSyncObjects(VkDevice device);
	};
}
