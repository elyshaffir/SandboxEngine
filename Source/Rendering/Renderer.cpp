#include <Rendering/Renderer.h>

#include <glog/logging.h>

sandbox::Renderer::Renderer(const WindowConfigurationInfo & windowConfigurationInfo,
							const GraphicsShaderPaths & graphicsShaderPaths, Model & model)
		: window(windowConfigurationInfo), instance(),
		  surface(instance.instance, window.window),
		  device(instance.instance, surface.surface, {window.width, window.height}),
		  pipeline(device.device, graphicsShaderPaths, device.renderPass.renderPass), currentFrame(), model(model)
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
		pipeline.Recreate(device.device, device.renderPass.renderPass);
	}
}

void sandbox::Renderer::WaitIdle() const
{
	vkDeviceWaitIdle(device.device);
}

bool sandbox::Renderer::TryDrawFrame()
{
	VkCommandBuffer frameCommandBuffer = VK_NULL_HANDLE;
	uint32_t imageIndex = 0;
	bool preparationSuccessful = device.PrepareDrawFrame(frameCommandBuffer, imageIndex);
	if (preparationSuccessful)
	{
		pipeline.PrepareDrawFrame(frameCommandBuffer, model);
		return device.FinalizeDrawFrame(frameCommandBuffer, imageIndex);
	}
	return false;
}
