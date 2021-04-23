#include <Rendering/Renderer.h>

#include <stdexcept>
#include <array>

sandbox::Renderer::Renderer(const WindowConfigurationInfo & windowConfigurationInfo,
							const GraphicsShaderPaths & shaderPaths,
							const PipelineConfigurationInfo & pipelineConfigurationInfo)
		: window(windowConfigurationInfo), instance(),
		  surface(instance.instance, window.window),
		  device(instance.instance, surface.surface, window.GenerateExtent()),
		  pipeline(device.device, shaderPaths, pipelineConfigurationInfo, device.swapChain.renderPass), currentFrame()
{
	device.CreateCommandBuffers(pipeline.pipeline);
}

sandbox::Renderer::~Renderer()
{
	pipeline.Destroy(device.device);
	device.Destroy();
	surface.Destroy(instance.instance);
}

bool sandbox::Renderer::ShouldClose()
{
	return window.ShouldClose();
}

void sandbox::Renderer::DrawFrame()
{
	currentFrame++;
	uint32_t imageIndex = 0;
	VkResult result = device.swapChain.AcquireNextImage(device.device, &imageIndex);

	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("Failed to acquire swap chain image");
	}

	result = device.swapChain.SubmitCommandBuffers(device.device, &device.commandBuffers[imageIndex],
												   &imageIndex, device.graphicsQueue, device.presentQueue);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to present swap chain image");
	}
}

void sandbox::Renderer::WaitIdle() const
{
	vkDeviceWaitIdle(device.device);
}
