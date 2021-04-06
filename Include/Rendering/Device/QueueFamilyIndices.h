#ifndef SANDBOXENGINE_QUEUEFAMILYINDICES_H
#define SANDBOXENGINE_QUEUEFAMILYINDICES_H

#include <optional>
#include <vector>

#include <vulkan/vulkan.h>

namespace sandbox
{
	class QueueFamilyIndices
	{
	public:
		QueueFamilyIndices() = default;

		QueueFamilyIndices(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

		bool IsComplete() const;

		void PopulateSwapChainCreateInfo(VkSwapchainCreateInfoKHR * swapChainCreateInfo) const;

		void FillQueueCreateInfos(std::vector<VkDeviceQueueCreateInfo> & queueCreateInfos) const;

		void GetDeviceQueues(VkDevice device, VkQueue * graphicsQueue, VkQueue * presentQueue) const;

		void PopulateGraphicsCommandPoolCreateInfo(VkCommandPoolCreateInfo * graphicsCommandPoolCreateInfo) const;

	private:
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;
	};
}

#endif //SANDBOXENGINE_QUEUEFAMILYINDICES_H
