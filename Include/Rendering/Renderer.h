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
		Renderer(uint16_t windowWidth, uint16_t windowHeight, std::string windowTitle,
				 GraphicsShaderPaths & shaderPaths, PipelineConfigurationInfo & pipelineConfigurationInfo);

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

		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;
		uint32_t currentFrame;

		void CreatePipelineLayout();

		void CreatePipeline();

		void CreateCommandBuffers();
	};
}

#endif //SANDBOXENGINE_RENDERER_H
