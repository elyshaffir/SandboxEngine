#include <Rendering/Device/CommandPool.h>

#include <stdexcept>

sandbox::CommandPool::CommandPool(VkDevice device, uint32_t queueFamilyIndex)
{
	VkCommandPoolCreateInfo poolInfo = { };
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = queueFamilyIndex;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create command pool");
	}
}

void sandbox::CommandPool::AllocateCommandBuffers(VkDevice device, uint32_t bufferCount,
												  std::vector<VkCommandBuffer> & outCommandBuffers) const
{
	outCommandBuffers.resize(bufferCount);
	VkCommandBufferAllocateInfo allocateInfo = { };
	allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocateInfo.commandPool = commandPool;
	allocateInfo.commandBufferCount = static_cast<uint32_t>(outCommandBuffers.size());

	if (vkAllocateCommandBuffers(device, &allocateInfo, outCommandBuffers.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocate command buffers");
	}
}

void sandbox::CommandPool::Destroy(VkDevice device) const
{
	vkDestroyCommandPool(device, commandPool, nullptr);
}
