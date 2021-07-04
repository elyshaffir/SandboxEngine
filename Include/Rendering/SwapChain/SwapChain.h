#pragma once

#include <Rendering/SwapChain/RenderPass.h>
#include <Rendering/SwapChain/SwapChainImage.h>
#include <Rendering/Device/CommandPool.h>

#include <vector>

#include <vulkan/vulkan.h>

namespace sandbox
{
	class SwapChain
	{
	public:
		RenderPass renderPass;
		VkSwapchainKHR swapChain;

		SwapChain() = default; // todo: find a way to remove

		SwapChain(VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface,
				  VkPresentModeKHR presentMode, VkExtent2D windowExtent,
				  const std::vector<VkSurfaceFormatKHR> & availableFormats,
				  const VkPhysicalDeviceMemoryProperties & physicalDeviceMemoryProperties,
				  uint32_t graphicsFamilyIndex, uint32_t presentFamilyIndex,
				  VkSwapchainKHR oldSwapChain = VK_NULL_HANDLE);

		bool BeginDrawFrame(VkDevice device);

		VkCommandBuffer GetCurrentFrameCommandBuffer();

		VkResult FinalizeDrawFrame(VkDevice device, VkQueue graphicsQueue, VkQueue presentQueue);

		void Destroy(VkDevice device, bool recycle);

	private:
		static constexpr size_t MAX_FRAMES_IN_FLIGHT = 2;

		uint32_t imageCount;
		std::vector<SwapChainImage> images;
		VkExtent2D extent;
		VkSurfaceFormatKHR surfaceFormat;
		VkSurfaceCapabilitiesKHR surfaceCapabilities;
		VkFormat depthFormat;
		VkPresentModeKHR presentMode;
		size_t inFlightFrameIndex;
		uint32_t imageIndex;
		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;
		CommandPool graphicsCommandPool;

		void ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR> & availableFormats);

		void ChooseDepthFormat(VkPhysicalDevice physicalDevice);

		void CreateImages(VkDevice device, const VkPhysicalDeviceMemoryProperties & physicalDeviceMemoryProperties);

		void ChooseExtent(VkExtent2D windowExtent);

		void CalculateImageCount();

		void Create(VkDevice device, VkSurfaceKHR surface, uint32_t graphicsFamilyIndex,
					uint32_t presentFamilyIndex, VkSwapchainKHR oldSwapChain);

		void CreateSyncObjects(VkDevice device);
	};
}
