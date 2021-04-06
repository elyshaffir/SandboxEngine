#include <Rendering/Renderer.h>

#include <utility>

sandbox::Renderer::Renderer(uint16_t windowWidth, uint16_t windowHeight, std::string windowTitle,
							GraphicsShaderPaths & shaderPaths, PipelineConfigurationInfo & pipelineConfigurationInfo)
		: window(windowWidth, windowHeight, std::move(windowTitle)), instance(),
		  surface(instance.instance, window.window),
		  device(instance.instance, surface.surface, window.GenerateExtent()),
		  pipeline(device.device, shaderPaths, pipelineConfigurationInfo), currentFrame()
{
}

sandbox::Renderer::~Renderer()
{
	surface.Destroy(instance.instance);
	pipeline.Destroy(device.device);
}

bool sandbox::Renderer::ShouldClose()
{
	return window.ShouldClose();
}
