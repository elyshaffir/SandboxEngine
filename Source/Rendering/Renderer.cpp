#include <Rendering/Renderer.h>

sandbox::Renderer::Renderer(const WindowConfigurationInfo & windowConfigurationInfo,
							const GraphicsPipelineConfigurationInfo & pipelineConfigurationInfo,
							Model & model)
		: window(windowConfigurationInfo), instance(),
		  surface(instance.instance, window.window),
		  device(instance.instance, surface.surface, window.GenerateExtent()),
		  pipeline(), currentFrame(), model(model)
{
	pipeline = GraphicsPipeline(device.device, pipelineConfigurationInfo, device.GetRenderPass());
	device.AllocateVertexBuffer(model.vertexBuffer);
	device.RecordRenderPass(pipeline.pipeline, model);
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
	currentFrame++;
	device.DrawFrame();
}

void sandbox::Renderer::WaitIdle() const
{
	vkDeviceWaitIdle(device.device);
}
