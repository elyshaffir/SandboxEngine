#include <Rendering/Queue/GraphicsQueue.h>

#include <stdexcept>

bool sandbox::GraphicsQueue::FindFamily(VkPhysicalDevice physicalDevice)
{
	uint32_t i = 0;
	for (const auto & queueFamily : GetQueueFamilies(physicalDevice))
	{
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			family = i;
			return true;
		}
		i++;
	}
	return false;
}

void sandbox::GraphicsQueue::Create(VkDevice device, uint32_t imageCount)
{
	Queue::Create(device);
	CreateCommandBuffers(device, imageCount);
}

VkCommandBuffer sandbox::GraphicsQueue::BeginCommandBuffer(size_t imageIndex)
{
	VkCommandBufferBeginInfo beginInfo = { };
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to begin recording command buffer");
	}

	return commandBuffers[imageIndex];
}

void sandbox::GraphicsQueue::EndCommandBuffers(size_t imageIndex)
{
	if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to record command buffer");
	}
}

void sandbox::GraphicsQueue::CreateCommandBuffers(VkDevice device, uint32_t imageCount)
{
	commandBuffers.resize(imageCount);
	VkCommandBufferAllocateInfo allocateInfo = { };
	allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocateInfo.commandPool = commandPool;
	allocateInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

	if (vkAllocateCommandBuffers(device, &allocateInfo, commandBuffers.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocate command buffers");
	}
}
