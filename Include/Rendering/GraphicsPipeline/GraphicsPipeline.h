#pragma once

#include <Rendering/Device/Device.h>
#include <Rendering/GraphicsPipeline/GraphicsShaderModules.h>

namespace sandbox
{
	class GraphicsPipeline
	{
	public:
		VkPipeline pipeline;

		GraphicsPipeline() = default;

		GraphicsPipeline(VkDevice device, VkExtent2D windowExtent, const GraphicsShaderPaths & shaderPaths,
						 VkRenderPass renderPass);

		void Destroy(VkDevice device);

	private:
		GraphicsShaderPaths shaderPaths;
		VkViewport viewport;
		VkRect2D scissor;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationCreateInfo;
		VkPipelineMultisampleStateCreateInfo multisampleCreateInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilCreateInfo;
		VkPipelineLayout pipelineLayout;
		uint32_t subpass;
		GraphicsShaderModules shaderModules;
		VkPipelineLayout layout;

		void Create(VkDevice device, VkExtent2D windowExtent, VkRenderPass renderPass);

		void CreateLayout(VkDevice device);
	};
}
