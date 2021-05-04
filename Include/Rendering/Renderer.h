#pragma once

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
				 const GraphicsPipelineConfigurationInfo & pipelineConfigurationInfo);

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
