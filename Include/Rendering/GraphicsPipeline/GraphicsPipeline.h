#pragma once

#include <Rendering/Device/Device.h>
#include <Rendering/GraphicsPipeline/GraphicsShaderModules.h>
#include <Rendering/GraphicsPipeline/GraphicsPipelineConfigurationInfo.h>

namespace sandbox
{
	class GraphicsPipeline
	{
	public:
		VkPipeline pipeline;

		GraphicsPipeline() = default;

		GraphicsPipeline(VkDevice device, const GraphicsPipelineConfigurationInfo & configurationInfo,
						 VkRenderPass renderPass);

		void Destroy(VkDevice device);

	private:
		GraphicsShaderModules shaderModules;
		VkPipelineLayout layout;

		void Create(VkDevice device, const GraphicsPipelineConfigurationInfo & configurationInfo,
					VkRenderPass renderPass);

		void CreateLayout(VkDevice device);
	};
}
