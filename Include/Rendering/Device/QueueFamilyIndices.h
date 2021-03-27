#ifndef SANDBOXENGINE_QUEUEFAMILYINDICES_H
#define SANDBOXENGINE_QUEUEFAMILYINDICES_H

#include <optional>
#include <vulkan/vulkan.h>

namespace sandbox
{
	struct QueueFamilyIndices
	{
	public:
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		static sandbox::QueueFamilyIndices FromDevice(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

		bool IsComplete() const;
	};
}

#endif //SANDBOXENGINE_QUEUEFAMILYINDICES_H
