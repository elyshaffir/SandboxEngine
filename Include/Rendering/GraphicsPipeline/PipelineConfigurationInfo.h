#pragma once

#include <vulkan/vulkan.h>

namespace sandbox
{
	class PipelineConfigurationInfo
	{
	public:
		explicit PipelineConfigurationInfo(VkExtent2D frameExtent);

		void Create(VkExtent2D frameExtent);

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
