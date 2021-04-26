#include <Rendering/Queue/PresentQueue.h>

#include <stdexcept>


sandbox::PresentQueue::PresentQueue(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) : Queue()
{
	FindFamily(physicalDevice, surface);
}

void sandbox::PresentQueue::FindFamily(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
{
	uint32_t i = 0;
	for (const auto & queueFamily : GetQueueFamilies(physicalDevice))
	{
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
		if (queueFamily.queueCount > 0 && presentSupport)
		{
			family = i;
			return;
		}
		i++;
	}
	throw std::runtime_error("Could not find a present queue family");
}
