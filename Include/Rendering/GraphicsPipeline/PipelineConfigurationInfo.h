#ifndef SANDBOXENGINE_PIPELINECONFIGURATIONINFO_H
#define SANDBOXENGINE_PIPELINECONFIGURATIONINFO_H

#include <vulkan/vulkan.h>

namespace sandbox
{
	class PipelineConfigurationInfo
	{
	public:
		PipelineConfigurationInfo(uint16_t width, uint16_t height);

		void Create(uint16_t width, uint16_t height);

		void PopulateGraphicsPipelineCreateInfo(VkGraphicsPipelineCreateInfo * graphicsPipelineCreateInfo) const;

	private:
		VkViewport viewport;
		VkRect2D scissor;
		VkPipelineViewportStateCreateInfo viewportCreateInfo;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationCreateInfo;
		VkPipelineMultisampleStateCreateInfo multisampleCreateInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilCreateInfo;
		VkPipelineLayout pipelineLayout;
		VkRenderPass renderPass;
		uint32_t subpass;
	};
}

#endif //SANDBOXENGINE_PIPELINECONFIGURATIONINFO_H
