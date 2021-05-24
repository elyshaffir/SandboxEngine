#pragma once

#include <Rendering/SwapChain/SwapChainSupport.h>

#include <vulkan/vulkan.h>

namespace sandbox
{
	class RenderPass
	{
	public:
		VkRenderPass renderPass;

		RenderPass() = default;

		RenderPass(const SwapChainSupport & supportDetails, VkDevice device);

		void Begin(VkCommandBuffer commandBuffer, VkFramebuffer framebuffer, VkExtent2D swapChainExtent) const;

		void Destroy(VkDevice device) const;
	};
}
