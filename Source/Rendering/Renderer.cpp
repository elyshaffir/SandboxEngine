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
	CreatePipeline();
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
