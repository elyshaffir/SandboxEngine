#pragma once

#include <Rendering/Device/Device.h>
#include <Rendering/GraphicsPipeline/GraphicsShaderModules.h>
#include <Rendering/GraphicsPipeline/PipelineConfigurationInfo.h>

namespace sandbox
{
	class GraphicsPipeline
	{
	public:
		VkPipeline pipeline;

		GraphicsPipeline(VkDevice device, const GraphicsShaderPaths & shaderPaths,
						 const PipelineConfigurationInfo & configurationInfo, VkRenderPass renderPass);

		void Destroy(VkDevice device);

	private:
		GraphicsShaderModules shaderModules;
		VkPipelineLayout layout;

		void Create(VkDevice device, const PipelineConfigurationInfo & configurationInfo, VkRenderPass renderPass);

		void CreateLayout(VkDevice device);
	};
}
