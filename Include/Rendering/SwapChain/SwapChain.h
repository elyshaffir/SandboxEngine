#pragma once

#include <Rendering/SwapChain/SwapChainSupport.h>
#include <Rendering/Device/QueueFamilyIndices.h>

#include <vector>
#include <array>

#include <vulkan/vulkan.h>

namespace sandbox
{
	class SwapChain
	{
	public:
		VkRenderPass renderPass;

		SwapChain() = default;

		SwapChain(const SwapChainSupport & supportDetails, const QueueFamilyIndices & queueFamilyIndices,
				  VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface);

		void Destroy(VkDevice device);

		VkResult AcquireNextImage(VkDevice device, uint32_t * imageIndex);

		VkResult SubmitCommandBuffers(VkDevice device, const VkCommandBuffer * buffers, uint32_t * imageIndex,
									  VkQueue graphicsQueue,
									  VkQueue presentQueue);

		VkFramebuffer GetFrameBuffer(uint32_t framebufferIndex);

	private:
		static constexpr size_t MAX_FRAMES_IN_FLIGHT = 2;
		VkSwapchainKHR swapChain;
		size_t frameIndex;
		std::vector<VkFramebuffer> framebuffers;
		std::vector<VkImage> depthImages;
		std::vector<VkDeviceMemory> depthImageMemories;
		std::vector<VkImageView> depthImageViews;
		std::vector<VkImage> images;
		std::vector<VkImageView> imageViews;
		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;
		std::vector<VkFence> imagesInFlight;

		void Create(const SwapChainSupport & supportDetails, const QueueFamilyIndices & queueFamilyIndices,
					VkDevice device, VkSurfaceKHR surface);

		void CreateImageViews(const SwapChainSupport & supportDetails, VkDevice device);

		void CreateRenderPass(const SwapChainSupport & supportDetails, VkDevice device);

		void CreateDepthResources(const SwapChainSupport & supportDetails, VkDevice device,
								  VkPhysicalDevice physicalDevice);

		void CreateFramebuffers(const SwapChainSupport & supportDetails, VkDevice device);

		void CreateSyncObjects(VkDevice device);
	};
}
