#pragma once

#include <Rendering/SwapChain/SwapChainImage.h>

#include <vulkan/vulkan.h>

namespace sandbox
{
	class RenderPass
	{
	public:
		VkRenderPass renderPass;

		RenderPass() = default;

		explicit RenderPass(VkDevice device, VkFormat depthFormat);

		void Begin(VkFramebuffer framebuffer, VkCommandBuffer commandBuffer, VkExtent2D swapChainExtent) const;

		void Destroy(VkDevice device) const;
	};
}
