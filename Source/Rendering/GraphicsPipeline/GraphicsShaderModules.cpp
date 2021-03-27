#include <Rendering/GraphicsPipeline/GraphicsShaderModules.h>
#include <IO/FileIO.h>

#include <stdexcept>

static void CreateShaderModule(VkDevice device, const std::vector<char> & code, VkShaderModule * module)
{
	VkShaderModuleCreateInfo createInfo = { };
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

	if (vkCreateShaderModule(device, &createInfo, nullptr, module) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create shader module");
	}
}

sandbox::GraphicsShaderModules::GraphicsShaderModules(VkDevice device, const GraphicsShaderPaths & shaderPaths)
		: vertexShaderModule(VK_NULL_HANDLE), fragmentShaderModule(VK_NULL_HANDLE)
{
	CreateShaderModule(device, FileIO::ReadFile(shaderPaths.vertexShaderPath), &vertexShaderModule);
	CreateShaderModule(device, FileIO::ReadFile(shaderPaths.fragmentShaderPath), &fragmentShaderModule);
}

std::array<VkPipelineShaderStageCreateInfo, 2> sandbox::GraphicsShaderModules::GetCreateInfos() const
{
	std::array<VkPipelineShaderStageCreateInfo, 2> createInfos = { };
	createInfos[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	createInfos[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	createInfos[0].module = vertexShaderModule;
	createInfos[0].pName = "main";

	createInfos[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	createInfos[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	createInfos[1].module = fragmentShaderModule;
	createInfos[1].pName = "main";

	return createInfos;
}

void sandbox::GraphicsShaderModules::Destroy(VkDevice device) const
{
	vkDestroyShaderModule(device, vertexShaderModule, nullptr);
	vkDestroyShaderModule(device, fragmentShaderModule, nullptr);
}
