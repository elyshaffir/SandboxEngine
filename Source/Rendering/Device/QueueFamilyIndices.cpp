#include <Rendering/Device/QueueFamilyIndices.h>

#include <set>

sandbox::QueueFamilyIndices::QueueFamilyIndices(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
{
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto & queueFamily : queueFamilies)
	{
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			graphicsFamily = i;
		}
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
		if (queueFamily.queueCount > 0 && presentSupport)
		{
			presentFamily = i;
		}
		if (IsComplete())
		{
			break;
		}

		i++;
	}
}

bool sandbox::QueueFamilyIndices::IsComplete() const
{
	return graphicsFamily.has_value() && presentFamily.has_value();
}

void sandbox::QueueFamilyIndices::PopulateSwapChainCreateInfo(VkSwapchainCreateInfoKHR * swapChainCreateInfo) const
{
	if (graphicsFamily != presentFamily)
	{
		swapChainCreateInfo->imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		swapChainCreateInfo->queueFamilyIndexCount = 2;
		uint32_t queueFamilyIndices[2] = {graphicsFamily.value(), presentFamily.value()};
		swapChainCreateInfo->pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		swapChainCreateInfo->imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}
}

void sandbox::QueueFamilyIndices::FillQueueCreateInfos(std::vector<VkDeviceQueueCreateInfo> & queueCreateInfos) const
{
	std::set<uint32_t> uniqueQueueFamilies = {graphicsFamily.value(), presentFamily.value()};

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo = { };
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}
}

void sandbox::QueueFamilyIndices::GetDeviceQueues(VkDevice device, VkQueue * graphicsQueue,
												  VkQueue * presentQueue) const
{
	vkGetDeviceQueue(device, graphicsFamily.value(), 0, graphicsQueue);
	vkGetDeviceQueue(device, presentFamily.value(), 0, presentQueue);
}

void sandbox::QueueFamilyIndices::PopulateGraphicsCommandPoolCreateInfo(
		VkCommandPoolCreateInfo * graphicsCommandPoolCreateInfo) const
{
	graphicsCommandPoolCreateInfo->queueFamilyIndex = graphicsFamily.value();
}
