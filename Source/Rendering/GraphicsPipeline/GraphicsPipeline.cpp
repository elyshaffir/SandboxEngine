#include <Rendering/GraphicsPipeline/GraphicsPipeline.h>

#include <stdexcept>

sandbox::GraphicsPipeline::GraphicsPipeline(VkDevice device, const sandbox::GraphicsShaderPaths & shaderPaths,
											const sandbox::PipelineConfigurationInfo & configurationInfo) :
		pipeline(VK_NULL_HANDLE), shaderModules(device, shaderPaths)
{
	Create(device, configurationInfo);
}

void sandbox::GraphicsPipeline::Create(VkDevice device, const PipelineConfigurationInfo & configurationInfo)
{
	VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo = { };
	vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	VkGraphicsPipelineCreateInfo createInfo = { };
	createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	createInfo.stageCount = 2;
	createInfo.pStages = shaderModules.GetCreateInfos().data();
	createInfo.pVertexInputState = &vertexInputCreateInfo;
	configurationInfo.PopulateGraphicsPipelineCreateInfo(&createInfo);
	createInfo.basePipelineIndex = -1;

	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &createInfo, nullptr, &pipeline) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create graphics pipeline");
	}
}

void sandbox::GraphicsPipeline::Destroy(VkDevice device)
{
	shaderModules.Destroy(device);
	vkDestroyPipeline(device, pipeline, nullptr);
}
