#pragma once

#include <Rendering/Queue/Queue.h>
#include <Rendering/Model/Model.h>

namespace sandbox
{
	class GraphicsQueue : public Queue
	{
	public:
		bool FindFamily(VkPhysicalDevice physicalDevice);

		void Create(VkDevice device, uint32_t imageCount);

		void RecordCommandBuffers(VkRenderPass renderPass, size_t imageIndex, VkFramebuffer framebuffer,
								  VkExtent2D chosenExtent, VkPipeline pipeline, const Model & model);

		VkCommandBuffer * GetCommandBuffer(uint32_t index);

		void CreateCommandBuffers(VkDevice device, uint32_t imageCount);
	};
}
