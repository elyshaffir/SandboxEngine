#include <Rendering/Renderer.h>

#include <utility>
#include <stdexcept>
#include <array>

sandbox::Renderer::Renderer(uint16_t windowWidth, uint16_t windowHeight, std::string windowTitle,
							GraphicsShaderPaths & shaderPaths, PipelineConfigurationInfo & pipelineConfigurationInfo)
		: window(windowWidth, windowHeight, std::move(windowTitle)), instance(),
		  surface(instance.instance, window.window),
		  device(instance.instance, surface.surface, window.GenerateExtent()),
		  pipeline(device.device, shaderPaths, pipelineConfigurationInfo), currentFrame()
{
	CreatePipelineLayout();
	CreatePipeline();
	CreateCommandBuffers();
}

sandbox::Renderer::~Renderer()
{
	vkDestroyPipelineLayout(device.device, pipelineLayout, nullptr);
	surface.Destroy(instance.instance);
	pipeline.Destroy(device.device);
}

bool sandbox::Renderer::ShouldClose()
{
	return window.ShouldClose();
}

void sandbox::Renderer::DrawFrame()
{
	currentFrame++;
	uint32_t imageIndex = 0;
	VkResult result = device.swapChain.AcquireNextImage(device.device, currentFrame, &imageIndex);

	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("Failed to acquire swap chain image");
	}

	result = device.swapChain.SubmitCommandBuffers(device.device, currentFrame, &commandBuffers[imageIndex],
												   &imageIndex);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to present swap chain image");
	}
}

void sandbox::Renderer::WaitIdle() const
{
	vkDeviceWaitIdle(device.device);
}

void sandbox::Renderer::CreatePipeline()
{
	PipelineConfigurationInfo pipelineConfigurationInfo = PipelineConfigurationInfo(swapchain_extent);
	pipelineConfigurationInfo.renderPass = swapchain_render_pass;
	pipelineConfigurationInfo.pipelineLayout = pipelineLayout;
	pipeline = GraphicsPipeline(device.device, shader_paths, pipelineConfigurationInfo);
}

void sandbox::Renderer::CreateCommandBuffers()
{
	commandBuffers.resize(swapchain_image_count);
	VkCommandBufferAllocateInfo allocateInfo = { };
	allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocateInfo.commandPool = device_command_pool;
	allocateInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

	if (vkAllocateCommandBuffers(device.device, &allocateInfo, commandBuffers.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocate command buffers");
	}

	for (uint32_t i = 0; i < commandBuffers.size(); i++)
	{
		VkCommandBufferBeginInfo beginInfo = { };
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to begin recording command buffer");
		}

		VkRenderPassBeginInfo renderPassBeginInfo = { };
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = swapchain_render_pass;
		renderPassBeginInfo.framebuffer = swapchain.getFrameBuffer(i);
		renderPassBeginInfo.renderArea.extent = swapchain_extent;

		std::array<VkClearValue, 2> clearValues = { };
		clearValues[0].color = {0.93f, 0.69f, 0.86f, 1.0f};
		clearValues[1].depthStencil = {1.0f, 0};
		renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassBeginInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
		pipeline.Bind(commandBuffers[i]);
		vkCmdDraw(commandBuffers[0], 3, 1, 0, 0);
		vkCmdEndRenderPass(commandBuffers[0]);
		if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to record command buffer");
		}
	}
}
