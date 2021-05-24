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

		VkCommandBuffer BeginCommandBuffer(size_t imageIndex);

		void EndCommandBuffers(size_t imageIndex);

		void CreateCommandBuffers(VkDevice device, uint32_t imageCount);
	};
}
