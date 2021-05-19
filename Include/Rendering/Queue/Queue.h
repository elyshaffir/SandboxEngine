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

		void FreeCommandBuffers(VkDevice device);

		void Destroy(VkDevice device) const;

	protected:
		std::vector<VkCommandBuffer> commandBuffers;

		VkCommandPool commandPool;

		static std::vector<VkQueueFamilyProperties> GetQueueFamilies(VkPhysicalDevice physicalDevice);

		Queue();

		virtual void Create(VkDevice device);

	private:
		void CreateCommandPool(VkDevice device);
	};
}
