#include <Rendering/Device/QueueFamilyIndices.h>

#include <vector>

sandbox::QueueFamilyIndices sandbox::QueueFamilyIndices::FromDevice(VkPhysicalDevice physicalDevice,
																	VkSurfaceKHR surface)
{
	QueueFamilyIndices indices = { };
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto & queueFamily : queueFamilies)
	{
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphicsFamily = i;
		}
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
		if (queueFamily.queueCount > 0 && presentSupport)
		{
			indices.presentFamily = i;
		}
		if (indices.IsComplete())
		{
			break;
		}

		i++;
	}

	return indices;
}

bool sandbox::QueueFamilyIndices::IsComplete() const
{
	return graphicsFamily.has_value() && presentFamily.has_value();
}
