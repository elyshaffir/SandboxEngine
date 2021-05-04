#include <Rendering/Queue/PresentQueue.h>

void sandbox::PresentQueue::Create(VkDevice device)
{
	Queue::Create(device);
}

bool sandbox::PresentQueue::FindFamily(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
{
	uint32_t i = 0;
	for (const auto & queueFamily : GetQueueFamilies(physicalDevice))
	{
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
		if (queueFamily.queueCount > 0 && presentSupport)
		{
			family = i;
			return true;
		}
		i++;
	}
	return false;
}
