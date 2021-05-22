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

		GraphicsPipeline(VkDevice device, GraphicsShaderPaths shaderPaths, VkRenderPass renderPass);

		void Recreate(VkDevice device, VkRenderPass renderPass);

		void Destroy(VkDevice device) const;

	private:
		uint32_t subpass;
		GraphicsShaderPaths shaderPaths;

		void Create(VkDevice device, VkRenderPass renderPass);

		void CreateLayout(VkDevice device);
	};
}
