#pragma once

#include <Rendering/Device/Device.h>
#include <Rendering/GraphicsPipeline/GraphicsShaderModules.h>
#include <Rendering/GraphicsPipeline/PushConstantData.h>

namespace sandbox
{
	class GraphicsPipeline
	{
	public:
		VkPipeline pipeline;
		VkPipelineLayout layout;

		GraphicsPipeline() = default;

		GraphicsPipeline(VkDevice device, const GraphicsShaderPaths & shaderPaths,
						 VkRenderPass renderPass);

		void Destroy(VkDevice device);

	private:
		uint32_t subpass;
		GraphicsShaderModules shaderModules;

		void Create(VkDevice device, VkRenderPass renderPass);

		void CreateLayout(VkDevice device);
	};
}
