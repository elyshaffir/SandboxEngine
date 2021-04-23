#ifndef SANDBOXENGINE_RENDERER_H
#define SANDBOXENGINE_RENDERER_H

#include <Rendering/Window/Window.h>
#include <Rendering/Instance/Instance.h>
#include <Rendering/Surface/Surface.h>
#include <Rendering/Device/Device.h>
#include <Rendering/GraphicsPipeline/GraphicsPipeline.h>

#include <vector>

namespace sandbox
{
	class Renderer
	{
	public:
		Renderer(const WindowConfigurationInfo & windowConfigurationInfo,
				 const GraphicsShaderPaths & shaderPaths, const PipelineConfigurationInfo & pipelineConfigurationInfo);

		~Renderer();

		bool ShouldClose();

		void DrawFrame();

		void WaitIdle() const;

	private:
		Instance instance;
		Window window;
		Surface surface;
		Device device;
		GraphicsPipeline pipeline;
		uint32_t currentFrame;
	};
}

#endif //SANDBOXENGINE_RENDERER_H
