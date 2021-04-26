#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace sandbox
{
	class Queue
	{
	public:
		VkQueue queue;
		uint32_t family;

		void Create(VkDevice device);

		void Destroy(VkDevice device) const;

	protected:
		std::vector<VkCommandBuffer> commandBuffers;
		VkCommandPool commandPool;

		Queue();

		static std::vector<VkQueueFamilyProperties> GetQueueFamilies(VkPhysicalDevice physicalDevice);

	private:
		void CreateCommandPool(VkDevice device);
	};
}
