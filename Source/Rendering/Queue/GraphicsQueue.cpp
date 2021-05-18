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

void sandbox::GraphicsQueue::RecordCommandBuffers(VkRenderPass renderPass, size_t imageIndex, VkFramebuffer framebuffer,
												  VkExtent2D chosenExtent, VkPipeline pipeline, const Model & model)
{
	VkCommandBufferBeginInfo beginInfo = { };
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to begin recording command buffer");
	}

	VkRenderPassBeginInfo renderPassBeginInfo = { };
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.renderPass = renderPass;
	renderPassBeginInfo.framebuffer = framebuffer;
	renderPassBeginInfo.renderArea.extent = chosenExtent;

	std::array<VkClearValue, 2> clearValues = { };
	clearValues[0].color = {0.93f, 0.69f, 0.86f, 1.0f};
	clearValues[1].depthStencil = {1.0f, 0};
	renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassBeginInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport viewport = { };
	viewport.width = static_cast<float>(chosenExtent.width);
	viewport.height = static_cast<float>(chosenExtent.height);
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {{}, chosenExtent};

	vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
	vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

	vkCmdBindPipeline(commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
	model.Bind(commandBuffers[imageIndex]);
	model.Draw(commandBuffers[imageIndex]);
	vkCmdEndRenderPass(commandBuffers[imageIndex]);
	if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to record command buffer");
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
