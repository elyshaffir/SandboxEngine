#include <Rendering/GraphicsPipeline/GraphicsPipeline.h>

#include <stdexcept>
#include <utility>

sandbox::GraphicsPipeline::GraphicsPipeline(VkDevice device, GraphicsShaderPaths shaderPaths, VkRenderPass renderPass) :
		pipeline(), layout(), subpass(), shaderPaths(std::move(shaderPaths))
{
	CreateLayout(device);
	Create(device, renderPass);
}

void sandbox::GraphicsPipeline::Recreate(VkDevice device, VkRenderPass renderPass)
{
	Destroy(device);
	CreateLayout(device);
	Create(device, renderPass);
}

void sandbox::GraphicsPipeline::Destroy(VkDevice device) const
{
	vkDestroyPipelineLayout(device, layout, nullptr);
	vkDestroyPipeline(device, pipeline, nullptr);
}

void sandbox::GraphicsPipeline::Create(VkDevice device, VkRenderPass renderPass)
{
	GraphicsShaderModules shaderModules = GraphicsShaderModules(device, shaderPaths);
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo = { };
	inputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

	VkPipelineViewportStateCreateInfo viewportCreateInfo = { };
	viewportCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportCreateInfo.viewportCount = 1;
	viewportCreateInfo.scissorCount = 1;

	VkPipelineRasterizationStateCreateInfo rasterizationCreateInfo = { };
	rasterizationCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizationCreateInfo.depthClampEnable = VK_FALSE;
	rasterizationCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	rasterizationCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizationCreateInfo.lineWidth = 1.0f;
	rasterizationCreateInfo.cullMode = VK_CULL_MODE_NONE;
	rasterizationCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizationCreateInfo.depthBiasEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multisampleCreateInfo = { };
	multisampleCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampleCreateInfo.sampleShadingEnable = VK_FALSE;
	multisampleCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineColorBlendAttachmentState colorBlendAttachment = { };
	colorBlendAttachment.colorWriteMask =
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
			VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo = { };
	colorBlendCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendCreateInfo.logicOpEnable = VK_FALSE;
	colorBlendCreateInfo.attachmentCount = 1;
	colorBlendCreateInfo.pAttachments = &colorBlendAttachment;

	VkPipelineDepthStencilStateCreateInfo depthStencilCreateInfo = { };
	depthStencilCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencilCreateInfo.depthTestEnable = VK_TRUE;
	depthStencilCreateInfo.depthWriteEnable = VK_TRUE;
	depthStencilCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencilCreateInfo.depthBoundsTestEnable = VK_FALSE;
	depthStencilCreateInfo.stencilTestEnable = VK_FALSE;

	std::vector<VkDynamicState> dynamicStateEnables = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
	VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = { };
	dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicStateCreateInfo.pDynamicStates = dynamicStateEnables.data();
	dynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables.size());

	std::vector<VkVertexInputBindingDescription> bindingDescriptions = Vertex::GetBindingDescriptions();
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions = Vertex::GetAttributeDescriptions();
	VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo = { };
	vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputCreateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
	vertexInputCreateInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
	vertexInputCreateInfo.pVertexBindingDescriptions = bindingDescriptions.data();

	VkGraphicsPipelineCreateInfo createInfo = { };
	createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	createInfo.stageCount = 2;
	createInfo.pStages = shaderModules.GetCreateInfos().data();
	createInfo.pVertexInputState = &vertexInputCreateInfo;
	createInfo.pViewportState = &viewportCreateInfo;
	createInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
	createInfo.pRasterizationState = &rasterizationCreateInfo;
	createInfo.pMultisampleState = &multisampleCreateInfo;
	createInfo.pColorBlendState = &colorBlendCreateInfo;
	createInfo.pDepthStencilState = &depthStencilCreateInfo;
	createInfo.pDynamicState = &dynamicStateCreateInfo;
	createInfo.layout = layout;
	createInfo.subpass = subpass;
	createInfo.renderPass = renderPass;
	createInfo.basePipelineIndex = -1;

	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &createInfo, nullptr, &pipeline) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create graphics pipeline");
	}

	shaderModules.Destroy(device);
}

void sandbox::GraphicsPipeline::CreateLayout(VkDevice device)
{
	VkPushConstantRange pushConstantRange = { };
	pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstantRange.size = sizeof(PushConstantData);

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = { };
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
	pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;

	if (vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &layout) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create pipeline layout");
	}
}
