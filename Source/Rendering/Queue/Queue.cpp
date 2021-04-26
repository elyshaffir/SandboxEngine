#include <Rendering/Queue/Queue.h>

#include <stdexcept>

sandbox::Queue::Queue() : queue(), family(), commandPool(VK_NULL_HANDLE)
{
}

std::vector<VkQueueFamilyProperties> sandbox::Queue::GetQueueFamilies(VkPhysicalDevice physicalDevice)
{
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

	return queueFamilies;
}

void sandbox::Queue::Create(VkDevice device)
{
	vkGetDeviceQueue(device, family, 0, &queue);
	CreateCommandPool(device);
}

void sandbox::Queue::Destroy(VkDevice device) const
{
	vkDestroyCommandPool(device, commandPool, nullptr);
}

void sandbox::Queue::CreateCommandPool(VkDevice device)
{
	VkCommandPoolCreateInfo poolInfo = { };
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = family;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create command pool");
	}
}