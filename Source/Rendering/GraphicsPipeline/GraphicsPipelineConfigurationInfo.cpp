#include <Rendering/GraphicsPipeline/GraphicsPipelineConfigurationInfo.h>

sandbox::GraphicsPipelineConfigurationInfo::GraphicsPipelineConfigurationInfo(VkExtent2D frameExtent,
																			  GraphicsShaderPaths & shaderPaths)
		: shaderPaths(shaderPaths), viewport(), scissor(), inputAssemblyCreateInfo(), rasterizationCreateInfo(),
		  multisampleCreateInfo(), colorBlendAttachment(), colorBlendCreateInfo(), depthStencilCreateInfo(),
		  pipelineLayout(), subpass()
{
	inputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

	viewport.width = static_cast<float>(frameExtent.width);
	viewport.height = static_cast<float>(frameExtent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	scissor.offset = {0, 0};
	scissor.extent = frameExtent;

	rasterizationCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizationCreateInfo.depthClampEnable = VK_FALSE;
	rasterizationCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	rasterizationCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizationCreateInfo.lineWidth = 1.0f;
	rasterizationCreateInfo.cullMode = VK_CULL_MODE_NONE;
	rasterizationCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizationCreateInfo.depthBiasEnable = VK_FALSE;

	multisampleCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampleCreateInfo.sampleShadingEnable = VK_FALSE;
	multisampleCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	colorBlendAttachment.colorWriteMask =
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
			VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;

	colorBlendCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendCreateInfo.logicOpEnable = VK_FALSE;
	colorBlendCreateInfo.attachmentCount = 1;
	colorBlendCreateInfo.pAttachments = &colorBlendAttachment;

	depthStencilCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencilCreateInfo.depthTestEnable = VK_TRUE;
	depthStencilCreateInfo.depthWriteEnable = VK_TRUE;
	depthStencilCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencilCreateInfo.depthBoundsTestEnable = VK_FALSE;
	depthStencilCreateInfo.stencilTestEnable = VK_FALSE;
}

void sandbox::GraphicsPipelineConfigurationInfo::PopulateViewportCreateInfo(
		VkPipelineViewportStateCreateInfo * viewportCreateInfo) const
{
	viewportCreateInfo->pViewports = &viewport;
	viewportCreateInfo->pScissors = &scissor;
}

void sandbox::GraphicsPipelineConfigurationInfo::PopulateGraphicsPipelineCreateInfo(
		VkGraphicsPipelineCreateInfo * graphicsPipelineCreateInfo) const
{
	graphicsPipelineCreateInfo->pInputAssemblyState = &inputAssemblyCreateInfo;
	graphicsPipelineCreateInfo->pRasterizationState = &rasterizationCreateInfo;
	graphicsPipelineCreateInfo->pMultisampleState = &multisampleCreateInfo;
	graphicsPipelineCreateInfo->pColorBlendState = &colorBlendCreateInfo;
	graphicsPipelineCreateInfo->pDepthStencilState = &depthStencilCreateInfo;
	graphicsPipelineCreateInfo->layout = pipelineLayout;
	graphicsPipelineCreateInfo->subpass = subpass;
}