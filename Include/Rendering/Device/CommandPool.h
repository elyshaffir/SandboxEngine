#pragma once

#include <vector>

#include <vulkan/vulkan.h>

namespace sandbox
{
	class CommandPool
	{
	public:
		VkCommandPool commandPool;

		CommandPool() // alert: this is ehre
		{

		}

		CommandPool(VkDevice device, uint32_t queueFamilyIndex);

		void AllocateCommandBuffers(VkDevice device, uint32_t bufferCount,
									std::vector<VkCommandBuffer> & outCommandBuffers) const;

		void Destroy(VkDevice device) const;
	};
}
