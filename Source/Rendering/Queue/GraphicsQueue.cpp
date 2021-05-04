#include <Rendering/Queue/GraphicsQueue.h>

#include <stdexcept>
#include <array>

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

void sandbox::GraphicsQueue::RecordRenderPass(VkRenderPass renderPass, const std::vector<VkFramebuffer> & framebuffers,
											  VkExtent2D chosenExtent, VkPipeline pipeline)
{
	for (size_t i = 0; i < commandBuffers.size(); i++)
	{
		VkCommandBufferBeginInfo beginInfo = { };
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to begin recording command buffer");
		}

		VkRenderPassBeginInfo renderPassBeginInfo = { };
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = renderPass;
		renderPassBeginInfo.framebuffer = framebuffers[i];
		renderPassBeginInfo.renderArea.extent = chosenExtent;

		std::array<VkClearValue, 2> clearValues = { };
		clearValues[0].color = {0.93f, 0.69f, 0.86f, 1.0f};
		clearValues[1].depthStencil = {1.0f, 0};
		renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassBeginInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
		vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);
		vkCmdEndRenderPass(commandBuffers[i]);
		if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to record command buffer");
		}
	}
}

VkCommandBuffer * sandbox::GraphicsQueue::GetCommandBuffer(uint32_t index)
{
	return &commandBuffers[index];
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
