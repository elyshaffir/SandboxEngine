#pragma once

#include <Rendering/Window/Window.h>
#include <Rendering/Instance/Instance.h>
#include <Rendering/Surface/Surface.h>
#include <Rendering/Device/Device.h>
#include <Rendering/GraphicsPipeline/GraphicsPipeline.h>
#include <Rendering/Model/Model.h>

#include <vector>

namespace sandbox
{
	class Renderer
	{
	public:
		Renderer(const WindowConfigurationInfo & windowConfigurationInfo,
				 const GraphicsShaderPaths & graphicsShaderPaths, Model & model);

		~Renderer();

		bool ShouldClose();

		void DrawFrame();

		void RecreateSwapChain();

		void WaitIdle() const;

	private:
		uint32_t currentFrame;

		Instance instance;
		Window window;
		Surface surface;
		Device device;
		GraphicsPipeline pipeline;
		Model & model;
	};
}
