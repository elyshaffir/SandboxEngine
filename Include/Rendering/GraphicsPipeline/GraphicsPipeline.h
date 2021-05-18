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

		GraphicsPipeline(VkDevice device, const GraphicsShaderPaths & shaderPaths,
						 VkRenderPass renderPass);

		void Destroy(VkDevice device);

	private:
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo;
		VkPipelineViewportStateCreateInfo viewportCreateInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationCreateInfo;
		VkPipelineMultisampleStateCreateInfo multisampleCreateInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilCreateInfo;
		VkPipelineLayout pipelineLayout;
		uint32_t subpass;
		GraphicsShaderModules shaderModules;
		VkPipelineLayout layout;
		std::vector<VkDynamicState> dynamicStateEnables;
		VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo;

		void Create(VkDevice device, VkRenderPass renderPass);

		void CreateLayout(VkDevice device);
	};
}
