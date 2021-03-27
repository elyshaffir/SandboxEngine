#include <Rendering/GraphicsPipeline/GraphicsPipeline.h>

sandbox::GraphicsPipeline::GraphicsPipeline(VkDevice device, const sandbox::GraphicsShaderPaths & shaderPaths,
											const sandbox::PipelineConfigurationInfo & configurationInfo) :
											pipeline(VK_NULL_HANDLE), shaderModules(device, shaderPaths)
{
}

void sandbox::GraphicsPipeline::Destroy(VkDevice device)
{
	shaderModules.Destroy(device);
}
