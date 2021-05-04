#pragma once

#include <Rendering/Queue/Queue.h>

namespace sandbox
{
	class GraphicsQueue : public Queue
	{
	public:
		bool FindFamily(VkPhysicalDevice physicalDevice);

		void Create(VkDevice device, uint32_t imageCount);

		void RecordRenderPass(VkRenderPass renderPass, const std::vector<VkFramebuffer> & framebuffers,
							  VkExtent2D chosenExtent, VkPipeline pipeline);

		VkCommandBuffer * GetCommandBuffer(uint32_t index);
	private:

		void CreateCommandBuffers(VkDevice device, uint32_t imageCount);
	};
}
