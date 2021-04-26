#include <Rendering/Renderer.h>

#include <stdexcept>
#include <array>

sandbox::Renderer::Renderer(const WindowConfigurationInfo & windowConfigurationInfo,
							const GraphicsShaderPaths & shaderPaths,
							const PipelineConfigurationInfo & pipelineConfigurationInfo)
		: window(windowConfigurationInfo), instance(),
		  surface(instance.instance, window.window),
		  device(instance.instance, surface.surface, window.GenerateExtent()),
		  graphicsQueue(device.physicalDevice),
		  presentQueue(device.physicalDevice, surface.surface),
		  swapChain(),
		  pipeline(),
		  currentFrame()
{
	device.CreateLogicalDevice({graphicsQueue.family, presentQueue.family});
	swapChain = SwapChain(device.swapChainSupport, device.physicalDevice, device.device, surface.surface,
						  graphicsQueue.family, presentQueue.family);
	pipeline = GraphicsPipeline(device.device, shaderPaths, pipelineConfigurationInfo, swapChain.renderPass);
	graphicsQueue.Create(device.device);
	graphicsQueue.CreateCommandBuffers(device.device, device.swapChainSupport.imageCount);
	graphicsQueue.RecordRenderPass(swapChain.renderPass, swapChain.framebuffers, device.swapChainSupport.chosenExtent,
								   pipeline.pipeline);
	presentQueue.Create(device.device);
}

sandbox::Renderer::~Renderer()
{
	pipeline.Destroy(device.device);
	swapChain.Destroy(device.device);
	graphicsQueue.Destroy(device.device);
	presentQueue.Destroy(device.device);
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
	VkResult result = swapChain.AcquireNextImage(device.device, &imageIndex);

	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("Failed to acquire swap chain image");
	}

	result = swapChain.SubmitCommandBuffers(device.device, graphicsQueue.GetCommandBuffer(imageIndex), &imageIndex,
											graphicsQueue.queue, presentQueue.queue);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to present swap chain image");
	}
}

void sandbox::Renderer::WaitIdle() const
{
	vkDeviceWaitIdle(device.device);
}
