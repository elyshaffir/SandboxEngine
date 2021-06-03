#include <Rendering/Renderer.h>

#include <glog/logging.h>

sandbox::Renderer::Renderer(const WindowConfigurationInfo & windowConfigurationInfo,
							const GraphicsShaderPaths & graphicsShaderPaths, Model & model)
		: window(windowConfigurationInfo), instance(),
		  surface(instance.instance, window.window),
		  device(instance.instance, surface.surface, {window.width, window.height}),
		  pipeline(device.device, graphicsShaderPaths, device.swapChain.renderPass.renderPass), currentFrame(1),
		  model(model)
{
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

	if (!TryDrawFrame())
	{
		window.Recreate();
		WaitIdle();
		device.RecreateSwapChain(surface.surface, {window.width, window.height});
		pipeline.Recreate(device.device, device.swapChain.renderPass.renderPass);
	}
}

void sandbox::Renderer::WaitIdle() const
{
	vkDeviceWaitIdle(device.device);
}

bool sandbox::Renderer::TryDrawFrame()
{
	bool preparationSuccessful = device.swapChain.BeginDrawFrame(device.device);
	if (preparationSuccessful)
	{
		pipeline.PrepareDrawFrame(device.swapChain.GetCurrentFrameCommandBuffer(), model);
		return device.FinalizeDrawFrame();
	}
	return false;
}
