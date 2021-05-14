#include <Rendering/Renderer.h>

#include <glog/logging.h>

sandbox::Renderer::Renderer(const WindowConfigurationInfo & windowConfigurationInfo,
							const GraphicsShaderPaths & graphicsShaderPaths, Model & model)
		: window(windowConfigurationInfo), instance(),
		  surface(instance.instance, window.window),
		  device(instance.instance, surface.surface, window.extent), graphicsShaderPaths(graphicsShaderPaths),
		  pipeline(), currentFrame(), model(model)
{
	pipeline = GraphicsPipeline(device.device, window.extent, graphicsShaderPaths, device.GetRenderPass());
	device.AllocateVertexBuffer(model.vertexBuffer);
}

sandbox::Renderer::~Renderer()
{
	pipeline.Destroy(device.device);
	model.Destroy(device.device);
	device.Destroy();
	surface.Destroy(instance.instance);
}

bool sandbox::Renderer::ShouldClose()
{
	return window.ShouldClose();
}

void sandbox::Renderer::DrawFrame()
{
	if (currentFrame % 1000 == 0)
	{
		LOG(INFO) << "1000 frame application update!";
	}
	currentFrame++;
	if (!device.DrawFrame(pipeline.pipeline, model) || window.WasResized())
	{
		window.ResetResized();
		RecreateSwapChain();
	}
}

void sandbox::Renderer::RecreateSwapChain()
{
	VkExtent2D newExtent = window.extent;
	while (newExtent.width == 0 || newExtent.height == 0)
	{
		newExtent = window.extent;
		glfwWaitEvents();
	}
	WaitIdle();
	device.RecreateSwapChain(surface.surface, newExtent);
	pipeline.Destroy(device.device);
	pipeline = GraphicsPipeline(device.device, newExtent, graphicsShaderPaths, device.GetRenderPass());
}

void sandbox::Renderer::WaitIdle() const
{
	vkDeviceWaitIdle(device.device);
}
