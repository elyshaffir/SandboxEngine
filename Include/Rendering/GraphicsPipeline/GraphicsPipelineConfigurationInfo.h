#pragma once

#include <vulkan/vulkan.h>
#include <Rendering/GraphicsPipeline/GraphicsShaderModules.h>

namespace sandbox
{
	class GraphicsPipelineConfigurationInfo
	{
	public:
		GraphicsShaderPaths shaderPaths;

		GraphicsPipelineConfigurationInfo(VkExtent2D frameExtent, GraphicsShaderPaths & shaderPaths);

		void PopulateViewportCreateInfo(VkPipelineViewportStateCreateInfo * viewportCreateInfo) const;

		void PopulateGraphicsPipelineCreateInfo(VkGraphicsPipelineCreateInfo * graphicsPipelineCreateInfo) const;

	private:
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
	};
}
