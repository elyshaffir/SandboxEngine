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

		GraphicsPipeline(VkDevice device, GraphicsShaderPaths shaderPaths, VkRenderPass renderPass);

		void Recreate(VkDevice device, VkRenderPass renderPass);

		void PrepareDrawFrame(VkCommandBuffer commandBuffer, Model & model) const;

		void Destroy(VkDevice device) const;

	private:
		uint32_t subpass;
		VkPipelineLayout layout;
		GraphicsShaderPaths shaderPaths;

		void Create(VkDevice device, VkRenderPass renderPass);

		void CreateLayout(VkDevice device);
	};
}
