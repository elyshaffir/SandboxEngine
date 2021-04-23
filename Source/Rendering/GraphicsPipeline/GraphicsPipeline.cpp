#include <Rendering/GraphicsPipeline/GraphicsPipeline.h>

#include <stdexcept>

sandbox::GraphicsPipeline::GraphicsPipeline(VkDevice device, const sandbox::GraphicsShaderPaths & shaderPaths,
											const sandbox::PipelineConfigurationInfo & configurationInfo,
											VkRenderPass renderPass) :
		pipeline(VK_NULL_HANDLE), shaderModules(device, shaderPaths), layout(VK_NULL_HANDLE)
{
	CreateLayout(device);
	Create(device, configurationInfo, renderPass);
}

void sandbox::GraphicsPipeline::Destroy(VkDevice device)
{
	vkDestroyPipelineLayout(device, layout, nullptr);
	shaderModules.Destroy(device);
	vkDestroyPipeline(device, pipeline, nullptr);
}

void sandbox::GraphicsPipeline::Create(VkDevice device, const PipelineConfigurationInfo & configurationInfo,
									   VkRenderPass renderPass)
{
	VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo = { };
	vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	VkPipelineViewportStateCreateInfo viewportCreateInfo = { };
	viewportCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportCreateInfo.viewportCount = 1;
	viewportCreateInfo.scissorCount = 1;
	configurationInfo.PopulateViewportCreateInfo(&viewportCreateInfo);

	VkGraphicsPipelineCreateInfo createInfo = { };
	createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	createInfo.stageCount = 2;
	createInfo.pStages = shaderModules.GetCreateInfos().data();
	createInfo.pVertexInputState = &vertexInputCreateInfo;
	createInfo.pViewportState = &viewportCreateInfo;
	configurationInfo.PopulateGraphicsPipelineCreateInfo(&createInfo);
	createInfo.renderPass = renderPass;
	createInfo.basePipelineIndex = -1;
	createInfo.layout = layout;

	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &createInfo, nullptr, &pipeline) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create graphics pipeline");
	}
}

void sandbox::GraphicsPipeline::CreateLayout(VkDevice device)
{
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = { };
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	if (vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &layout) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create pipeline layout");
	}
}
