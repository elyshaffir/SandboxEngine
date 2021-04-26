#pragma once

#include <Rendering/Window/Window.h>
#include <Rendering/Instance/Instance.h>
#include <Rendering/Surface/Surface.h>
#include <Rendering/Device/Device.h>
#include <Rendering/Queue/GraphicsQueue.h>
#include <Rendering/Queue/PresentQueue.h>
#include <Rendering/SwapChain/SwapChain.h>
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
		GraphicsQueue graphicsQueue;
		PresentQueue presentQueue;
		SwapChain swapChain;
		GraphicsPipeline pipeline;
		uint32_t currentFrame;
	};
}
